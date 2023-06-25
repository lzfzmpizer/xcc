
#include <stdio.h>

#define MY_ASSERT(_cond) my_assert((_cond))
#define MY_ASSERT2(_cond) my_assert2((_cond), #_cond)

void my_assert(int condition)
{
  if(!condition)
  {
    *((int *)0) = 0;
  }
}

void my_assert2(int condition, const char *expr)
{
  if(!condition)
  {
    *((int *)0) = 0;
  }
}

void call_assert_func1(int x)
{
  MY_ASSERT(x > 0);
  MY_ASSERT2(x > 0);
  MY_ASSERT2(x != 0);
}

