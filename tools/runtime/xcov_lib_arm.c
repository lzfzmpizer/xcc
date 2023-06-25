
#ifdef XccCov
#pragma XccCovSwitch off
#endif

#define XCOV_MAGIC16 (unsigned short)/*'CV'*/0x5643
#define XCOV_LINE_MAX_CHARS 128
#define XCOV_BUFFER_SIZE (XCOV_LINE_MAX_CHARS + 1)

#define XCOV_CONST_STR_LEN(_str) (sizeof((_str)) - 1)

#define XCOV_MARCH_ARM_V7M 0
#define XCOV_MARCH_ARM_V7R 1
#define XCOV_MARCH_ARM_V8A 2
#define XCOV_MARCH_UNKNOWN 255

#ifndef XCOV_MARCH
#define XCOV_MARCH XCOV_MARCH_UNKNOWN
#endif

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
static unsigned char xcov_buffer_g[XCOV_BUFFER_SIZE+1];
static volatile int xcov_buffer_index_g = 0;

static void xcov_str_copy(const char* str)
{
  int i;
  for (i = 0; str[i] != 0 && xcov_buffer_index_g < XCOV_BUFFER_SIZE; i++)
    xcov_buffer_g[xcov_buffer_index_g++] = str[i];
  xcov_buffer_g[xcov_buffer_index_g] = 0;
}

static void xcov_str_format_dec(unsigned int value)
{
  unsigned char tmp_buf[31];
  int i, tmp_len = 0;
  do 
  {
    tmp_buf[tmp_len++] = (unsigned char)('0' + (value % 10));
    value /= 10;
  } while (value != 0);
  for (i = tmp_len-1; i >= 0; i--) 
  {
    xcov_buffer_g[xcov_buffer_index_g++] = tmp_buf[i];
  }
  xcov_buffer_g[xcov_buffer_index_g] = 0;
}

static void xcov_str_format_hex(unsigned int value)
{
  unsigned char tmp_buf[31];
  int i, tmp_len = 0;
  do 
  {
    unsigned char byte_value = (unsigned char)(value & 0x0F);
    tmp_buf[tmp_len++] = (unsigned char)((byte_value < 10) ? ('0' + byte_value) : ('a' + byte_value - 10));
    value >>= 4;
  } while (value != 0);
  for (i = tmp_len-1; i >= 0; i--) 
  {
    xcov_buffer_g[xcov_buffer_index_g++] = tmp_buf[i];
  }
  xcov_buffer_g[xcov_buffer_index_g] = 0;
}

static void xcov_dump_write(const void *data, int len)
{
  /* write to uart or flash */
}

static void *xcov_lock()
{
  void * __status = 0;
#if (XCOV_MARCH == XCOV_MARCH_ARM_V7M)
  #if defined(__CC_ARM)
    /* 
     * cpsid if = __disable_irq() + __disable_fault_irq(), and __status = (primask<<1) + (faultmask), 
     * but faultmask (pri=-1) > primask (pri=0), so simply as faultmask operating
     */
    __asm volatile
    {
      mrs __status, faultmask
      cpsid f
    }
  #else
    asm volatile ("mrs %0, faultmask" : "=r"(__status));
    asm volatile ("cpsid f");
  #endif

#elif (XCOV_MARCH == XCOV_MARCH_ARM_V7R)
  #if defined(__CC_ARM)
    __asm volatile
    {
      mrs __status, cpsr
      cpsid if
    }
  #else
    asm volatile ("mrs %0, cpsr" : "=r"(__status));
    asm volatile ("cpsid if");
  #endif
  
#else
  
#endif

  return __status;
}

static void xcov_unlock(void *__status)
{
#if (XCOV_MARCH == XCOV_MARCH_ARM_V7M)
  #if defined(__CC_ARM)
    __asm volatile
    {
      msr faultmask, __status
    }
  #else
    asm volatile ("msr faultmask, %0" : : "r"(__status));
  #endif
  
#elif (XCOV_MARCH == XCOV_MARCH_ARM_V7R)
  #if defined(__CC_ARM)
    __asm volatile
    {
      msr cpsr_cxsf, __status
    }
  #else
    asm volatile ("msr cpsr_cxsf, %0" : : "r"(__status));
  #endif
  
#else
  
#endif
}

int xcov_dump_file(const struct xcov_file_struct *file)
{
  int i, point_index;
  
  xcov_buffer_index_g = 0;
  xcov_buffer_g[0] = 0;
  
  /* dump head */
  xcov_buffer_index_g = 0;
  xcov_str_copy("xcov_dump_start\n");
  xcov_dump_write(xcov_buffer_g, xcov_buffer_index_g);
  
  xcov_buffer_index_g = 0;
  xcov_str_copy("ts=");
  xcov_str_format_dec((unsigned int)file->ts);
  xcov_str_copy("\n");
  xcov_dump_write(xcov_buffer_g, xcov_buffer_index_g);
  
  xcov_buffer_index_g = 0;
  xcov_str_copy("crc=0x");
  xcov_str_format_hex((unsigned int)file->crc);
  xcov_str_copy("\n");
  xcov_dump_write(xcov_buffer_g, xcov_buffer_index_g);
  
  xcov_buffer_index_g = 0;
  xcov_str_copy("cnt=");
  xcov_str_format_dec((unsigned int)file->cnt);
  xcov_str_copy("\n");
  xcov_dump_write(xcov_buffer_g, xcov_buffer_index_g);
  
  xcov_buffer_index_g = 0;
  xcov_str_copy("source=");
  xcov_str_copy(file->source);
  xcov_str_copy("\n");
  xcov_dump_write(xcov_buffer_g, xcov_buffer_index_g);
  
  /* dump data */
  xcov_buffer_index_g = 0;
  xcov_str_copy("data={\n");
  xcov_dump_write(xcov_buffer_g, xcov_buffer_index_g);
  point_index = 0;
  while (point_index < (int)file->cnt)
  {
    xcov_buffer_index_g = 0;
    while ((xcov_buffer_index_g < XCOV_LINE_MAX_CHARS) && (point_index < (int)file->cnt))
    {
      unsigned char value = 0, byte_value;
      for (i = 0; i < 8; i++) 
      {
        value <<= 1;
        if (point_index < (int)file->cnt)
          value |= file->data->checkpoints[point_index++];
      }
      byte_value = (value & 0xF0)>>4;
      xcov_buffer_g[xcov_buffer_index_g++] = (unsigned char)((byte_value < 10) ? ('0' + byte_value) : ('a' + byte_value - 10));
      byte_value = (value & 0x0F);
      xcov_buffer_g[xcov_buffer_index_g++] = (unsigned char)((byte_value < 10) ? ('0' + byte_value) : ('a' + byte_value - 10));
    }
    xcov_buffer_g[xcov_buffer_index_g] = 0;
    xcov_str_copy("\n");
    xcov_dump_write(xcov_buffer_g, xcov_buffer_index_g);
  }
  xcov_buffer_index_g = 0;
  xcov_str_copy("}\n");
  xcov_str_copy("end\n");
  xcov_dump_write(xcov_buffer_g, xcov_buffer_index_g);
  return 0;
}

void xcov_dump()
{
  const struct xcov_file_struct *file = (const struct xcov_file_struct *)xcov_list_g;
  while(file != 0)
  {
    /*if (file->xcovdat[0] != 0)*/
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
      /*if (xcov_list_g == 0)
      {
        atexit(xcov_finish);
      }*/
      _file->data->linked = 1;
      _file->data->next = xcov_list_g;
      xcov_list_g = (const struct xcov_file_struct *)_file;
    }
    xcov_unlock(status);
  }
  _file->data->checkpoints[index] = 1;
}

