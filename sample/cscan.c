
#include "cscan_a.h"

extern int func_proto();

int var_g = 0;
int g_var2 = 0;
int var3_REG_g = 0;
int var4_REGs_g = 0;

void cc_sub_func()
{
  ;
}

void cc4(int *A)
{
  int i = 0;
  int n = 5;
  int j = 0;
  while (i < (n - 1)) 
  {
    j = i + 1;
    while (j < n) 
    {
      if (A[i] < A[j]) 
      {
        /*swap(A[i], A[j]);*/;
      }
    }
    i = i + 1;
  }
}

