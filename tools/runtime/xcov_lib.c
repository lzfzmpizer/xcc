
#include <sys/types.h>
#include <sys/stat.h>
/*#include <sys/locking.h>*/
#include <fcntl.h>
#include <errno.h>
#if defined(_WIN32) || defined(_WIN64)
#include <io.h>
#include <share.h>
#include <windows.h>
#else
#include <unistd.h>
#include <ctype.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef XccCov
#pragma XccCovSwitch off
#endif

#if defined(_MSC_VER)
#if !defined(strcasecmp)
#define strcasecmp stricmp
#endif
#if !defined(strncasecmp)
#define strncasecmp strnicmp
#endif
#if (_MSC_VER <= 1200)
#if !defined(vsnprintf)
#define vsnprintf _vsnprintf
#endif
#if !defined(snprintf)
#define snprintf _snprintf
#endif
#endif
#endif

#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif
#ifndef O_NOFOLLOW
#define O_NOFOLLOW 0
#endif
#ifndef O_BINARY
#define O_BINARY 0
#endif

#define XCOV_MAGIC16 (unsigned short)/*'CV'*/0x5643
#define XCOV_FILE_PRINTF_BUFFER_SIZE 256

#define XCOV_CONST_STR_LEN(_str) (sizeof((_str)) - 1)

typedef void *xcov_file_handle_t;

struct xcov_data_struct {
  const struct xcov_file_struct *next;
  volatile char linked;
  char checkpoints[1];
};

struct xcov_file_struct {
  unsigned short crc;
  unsigned short magic;
  unsigned int cnt;
  long ts;
  struct xcov_data_struct *data;
  const char *source;
  const char *xcovdat;
};

static const struct xcov_file_struct *xcov_list_g = 0;

static xcov_file_handle_t xcov_file_open(const char *file_path)
{
  xcov_file_handle_t file_handle;
#if defined(_WIN32) || defined(_WIN64)
  file_handle = (xcov_file_handle_t)CreateFile(
    file_path,
    GENERIC_READ | GENERIC_WRITE /*isReadWrite, else is GENERIC_READ only */,
    FILE_SHARE_READ | FILE_SHARE_WRITE /*None-exclusive, 0 if isExclusive*/, 0,
    OPEN_ALWAYS /*isCreate, CREATE_NEW if isExclusive*/,
    FILE_ATTRIBUTE_NORMAL,
    0);
  return ((HANDLE)file_handle == INVALID_HANDLE_VALUE) ? 0 : file_handle;
#else
  file_handle = (xcov_file_handle_t)open(file_path, \
    O_RDWR /*isReadWrite*/ \
    /*|(O_EXCL|O_NOFOLLOW)*/ /*isExclusive*/ \
    |O_CREAT /*isCreate*/ \
    |(O_BINARY|O_NOFOLLOW),  \
    0777);
  return ((int)file_handle < 0) ? 0 : file_handle;
#endif
}

static void xcov_file_close(xcov_file_handle_t file_handle)
{
#if defined(_WIN32) || defined(_WIN64)
  CloseHandle((HANDLE)file_handle);
#else
  close((int)file_handle);
#endif
}

long xcov_file_seek(xcov_file_handle_t file_handle, long offset, int whence)
{
#if defined(_WIN32) || defined(_WIN64)
  DWORD size_high = (DWORD)0;
  DWORD size_low = (DWORD)(offset);
  DWORD ptr;
  int _whence;
  
  switch(whence)
  {
  case SEEK_SET:
    _whence = FILE_BEGIN;
    break;
  case SEEK_CUR:
    _whence = FILE_CURRENT;
    break;
  case SEEK_END:
  default:
    _whence = FILE_END;
  }
  
  ptr = SetFilePointer((HANDLE)file_handle, size_low, (LONG *)&size_high, _whence);
  if(ptr == (DWORD)-1) 
    return -1;
  return (int)ptr;
#else
  return lseek64((int)file_handle, offset, whence);
#endif
}

static long xcov_file_size(xcov_file_handle_t file_handle)
{
#if defined(_WIN32) || defined(_WIN64)
  DWORD size_high = (DWORD)0;
  DWORD size_low = GetFileSize((HANDLE)file_handle, &size_high);
  if(size_low == (DWORD)-1) 
    return -1;
  return (int)size_low;
#else
  struct stat st;
  int rc = fstat((int)file_handle, &st);
  return ( rc!=0 ) ? -1 : st.st_size;
#endif
}

static int xcov_file_write(xcov_file_handle_t file_handle, char *buffer, int to_write_size)
{
#if defined(_WIN32) || defined(_WIN64)
  DWORD write_bytes = 0;
  if(!WriteFile((HANDLE)file_handle, buffer, to_write_size, &write_bytes, 0))
    return -1;
  return (int)write_bytes;
#else
  return write((int)file_handle, buffer, to_write_size);
#endif
}

static void xcov_file_vprintf(xcov_file_handle_t file_handle, const char *fmt, va_list args)
{
  char buffer[XCOV_FILE_PRINTF_BUFFER_SIZE + 1];
  int len;
  buffer[0] = buffer[XCOV_FILE_PRINTF_BUFFER_SIZE] = 0;
  len = vsnprintf(buffer, XCOV_FILE_PRINTF_BUFFER_SIZE, fmt, args);
  if (len > XCOV_FILE_PRINTF_BUFFER_SIZE || len == -1)
    len = XCOV_FILE_PRINTF_BUFFER_SIZE;
  xcov_file_write(file_handle, buffer, len);
}

static void xcov_file_printf(xcov_file_handle_t file_handle, const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  xcov_file_vprintf(file_handle, fmt, args);
  va_end(args);
}

static int xcov_file_read(xcov_file_handle_t file_handle, char *buffer, int to_read_size)
{
#if defined(_WIN32) || defined(_WIN64)
  DWORD read_bytes = 0;
  if(!ReadFile((HANDLE)file_handle, buffer, to_read_size, &read_bytes, 0))
    return -1;
  return (int)read_bytes;
#else
  return read((int)file_handle, buffer, to_read_size);
#endif
}

static int xcov_file_lock(xcov_file_handle_t file_handle)
{
#if defined(_WIN32) || defined(_WIN64)
  OVERLAPPED overlapped;
  BOOL res;
  ZeroMemory(&overlapped, sizeof(overlapped));
  res = LockFileEx((HANDLE)file_handle, LOCKFILE_EXCLUSIVE_LOCK, 0, 1, 0, &overlapped);
  return (int)res;
#else
  int rc = flock((int)file_handle, LOCK_EX/*|LOCK_NB*/);
  return (rc<0) ? 0 : 1;
#endif
}

static int xcov_file_unlock(xcov_file_handle_t file_handle)
{
#if defined(_WIN32) || defined(_WIN64)
  OVERLAPPED overlapped;
  BOOL res;
  ZeroMemory(&overlapped, sizeof(overlapped));
  res = UnlockFileEx((HANDLE)file_handle, 0, 1, 0, &overlapped);
  return (int)res;
#else
  int rc = flock((int)file_handle, LOCK_UN);
  return (rc<0) ? 0 : 1;
#endif
}

static int xcov_file_errno()
{
#if defined(_WIN32) || defined(_WIN64)
  return (int)GetLastError();
#else
  return errno;
#endif
}

static int xcov_atoi(const char *str)
{
  int value = atoi(str);
  if (value != 0)
  {
    return value;
  }
  
  if ( (str[0] == '0') && (tolower(str[1]) == 'x') )
  {
    if (str[2] == 0)
    {
      return 0;
    }
  
    sscanf(str, "%x", &value);
    return value;
  }
  
  return 0;
}

char *xcov_skip_spaces(char *str, char *end)
{
  if (str < end)
  {
    while(isspace(*str))
    {
      if( str >= end )
        break;
      str++;
    }
  }

  return str;
}

static void *xcov_lock()
{
  return 0;
}

static void xcov_unlock(void *status)
{
}

static int xcov_get_file_hit_count(const struct xcov_file_struct *file)
{
  int i = 0, count = 0;
  const char* checkpoints = file->data->checkpoints;
  while(i < (int)file->cnt)
  {
    count += checkpoints[i];
    i++;
  }
  return count;
}

#include <stdio.h>

int xcov_dump_file(const struct xcov_file_struct *file)
{
  xcov_file_handle_t xcovdat_handle;
  long file_size;
  char *file_buffer;
  char *ts_pos;
  char *crc_pos;
  char *cnt_pos;
  char *data_start;
  char *data_end;
  int hit_cnt = 0;
  
  xcovdat_handle = xcov_file_open(file->xcovdat);
  if (xcovdat_handle == 0)
    return -1;
  
  xcov_file_lock(xcovdat_handle);
  file_size = (long)xcov_file_size(xcovdat_handle);
  file_buffer = (char *)malloc(file_size+1);
  if(file_buffer == 0)
  {
    xcov_file_unlock(xcovdat_handle);
    xcov_file_close(xcovdat_handle);
    return -1;
  }
  
  file_buffer[0] = file_buffer[file_size] = 0;
  xcov_file_read(xcovdat_handle, file_buffer, file_size);
  ts_pos = strstr(file_buffer, "ts=");
  crc_pos = strstr(file_buffer, "crc=");
  cnt_pos = strstr(file_buffer, "cnt=");
  data_start = strstr(file_buffer, "data={");
  data_end = strstr(file_buffer, "}");
  if ((crc_pos != 0) 
    && (cnt_pos != 0) 
    && (xcov_atoi(ts_pos+XCOV_CONST_STR_LEN("ts=")) == (int)file->ts) 
    && (xcov_atoi(crc_pos+XCOV_CONST_STR_LEN("crc=")) == (int)file->crc) 
    && (xcov_atoi(cnt_pos+XCOV_CONST_STR_LEN("cnt=")) == (int)file->cnt) 
    && (data_start != 0) 
    && (data_end != 0))
  {
    int data_start_offset = data_start - file_buffer;
    int index;
    char *checkpoints = (char *)malloc(sizeof(char) * file->cnt);
    for (index=0; index < (int)file->cnt; index++)
    {
      checkpoints[index] = 0;
    }
    data_start += XCOV_CONST_STR_LEN("data={");
    while (data_start < data_end)
    {
      char *status_pos;
      char status;
      data_start = xcov_skip_spaces(data_start, data_end);
      if (data_start >= data_end)
        break;
      index = xcov_atoi(data_start);
      status_pos = strchr(data_start, ':');
      if ((index >= (int)file->cnt) || (status_pos == 0))
        break;
      status_pos += XCOV_CONST_STR_LEN(":");
      status_pos = xcov_skip_spaces(status_pos, data_end);
      if (status_pos >= data_end)
        break;
      status = (char)xcov_atoi(status_pos);
      if ((status != 0) && (status != 1))
        break;
      checkpoints[index] |= status;
      data_start = strchr(data_start, '\n');
      if (data_start == 0)
        break;
    }
    for (index=0; index < (int)file->cnt; index++)
    {
      checkpoints[index] |= file->data->checkpoints[index];
    }
    xcov_file_seek(xcovdat_handle, 0, SEEK_SET);
    xcov_file_write(xcovdat_handle, file_buffer, data_start_offset);
    xcov_file_write(xcovdat_handle, "data={\n", XCOV_CONST_STR_LEN("data={\n"));
    for (index=0; index < (int)file->cnt; index++)
    {
      hit_cnt += (checkpoints[index] ? 1 : 0);
      xcov_file_printf(xcovdat_handle, " %d:%d\n", index, checkpoints[index] ? 1 : 0);
    }
    xcov_file_write(xcovdat_handle, "}\n", XCOV_CONST_STR_LEN("}\n"));
    free(checkpoints);
  }

  free(file_buffer);
  xcov_file_unlock(xcovdat_handle);
  xcov_file_close(xcovdat_handle);
  return hit_cnt;
}

void xcov_dump()
{
#if 0
  const struct xcov_file_struct *file = (const struct xcov_file_struct *)xcov_list_g;
  printf("xcov result: \n");
  while(file != 0)
  {
    int hit_cnt = xcov_get_file_hit_count(file);
    int total_hit_cnt = hit_cnt;
    if (file->xcovdat[0] != 0)
      total_hit_cnt = xcov_dump_file(file);
    if (total_hit_cnt != hit_cnt)
      printf("  %s : hit %d / %d, total hit %d / %d \n", file->source, 
        hit_cnt, file->cnt, 
        total_hit_cnt, file->cnt);
    else
      printf("  %s : hit %d / %d \n", file->source, hit_cnt, file->cnt);
    file = file->data->next;
  }
#endif
  const struct xcov_file_struct *file = (const struct xcov_file_struct *)xcov_list_g;
  while(file != 0)
  {
    if (file->xcovdat[0] != 0)
      xcov_dump_file(file);
    file = file->data->next;
  }
}

static void xcov_finish(void)
{
  xcov_dump();
}

void xcov_probe(const void *file,int index)
{
  const struct xcov_file_struct *_file = (const struct xcov_file_struct *)file;
  if (_file->magic != XCOV_MAGIC16)
    return;
  if (!_file->data->linked)
  {
    void *status = xcov_lock();
    if (!_file->data->linked)
    {
      if (xcov_list_g == 0)
      {
        atexit(xcov_finish);
      }
      _file->data->linked = 1;
      _file->data->next = xcov_list_g;
      xcov_list_g = (const struct xcov_file_struct *)_file;
    }
    xcov_unlock(status);
  }
  _file->data->checkpoints[index] = 1;
}

