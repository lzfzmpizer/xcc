
#include "cscan_a.h"

extern char func_proto();

void cc_for5(int x)
{
  int i,j,k,l,m;
  for (i = 0; i < x; i++)
    for (j = 0; j < x; j++)
      for (k = 0; k < x; k++)
        for (l = 0; l < x; l++)
          for (m = 0; m < x; m++)
            ;
}

void cc_if5(int x)
{
  int i,j,k,l,m;
  if (x < 100)
    if (x < 90)
      if (x < 80)
        if (x < 70)
          if (x < 60)
            ;
}

