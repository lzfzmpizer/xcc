#ifdef XccCov
#pragma XccCovSection rodata = "cov.rodata", zidata="cov.zidata"
#endif

void func(int a, int b, int c)
  {
  if ( a )
    ;
  if ( a && b)
    ;
  if ( a && b && c )
    ;
  if ( a || b)
    ;
  if ( a || b || c )
    ;
  if ( a && (b || c))
    ;
  if ( (a && b) || c)
    ;
  if ( (a) && b)
    ;
  while ( a )
    ;
  while ( a && b)
    ;
  do
  {
    ;
  } while ( a );
  do
  {
    ;
  } while ( a && b );
  /*(for (; ;)
    ;*/
  for (; a ;)
    ;
  for (; a && b;)
    ;
  if (a)
  {
    if (b)
      ;
  }
  else if (b)
    ;
  else
    ;
  while ( a )
    while ( b )
      ;
  do
  {
    do
    {
      ;
    } while ( b );
  } while ( a );
  for (; a ;)
    for (; b ;)
      ;
  if (a)
  {
    while ( b )
      for (; c ;)
        ;
  }
  switch(a)
  {
    case 1: 
      break;
  }
  switch(b)
  {
    default: 
      break;
  }
  switch(a)
  {
    case 1: 
    case 2:
      switch(b)
      {
        case 1:
          break;
        case 2:
        default:
          switch(c)
          {
            default:
              break;
          }
          break;
      }
      break;
  }
  if (0)
    ;
  else if (1 || a)
    ;
  else if (a || 1)
    ;
  else if (0 && a)
    ;
  else if (a && 0)
    ;
  else if (a)
    ;
  while (1!=1)
    ;
}

void func2(int a, int b, int c)
{
#ifdef XccCov
#pragma XccCovSwitch off
#endif
  switch(a)
  {
    case 1: 
    case 2:
      switch(b)
      {
        case 1:
          break;
        case 2:
        default:
          switch(c)
          {
            default:
              break;
          }
          break;
      }
      break;
  }
#ifdef XccCov
#pragma XccCovSwitch on
#endif
}

void func3()
{
}
