
rem generate the databae for assert scanning
..\tools\xcc.exe --cmd=assertscandb --assertfunc=my_assert --assertfunc=my_assert2 --assertdbfile=assertscandb.c.i gcc -c -D MY_ASSERT(_cond)=my_assert((_cond)) -D MY_ASSERT2(_cond)=my_assert2((_cond),#_cond) assertscandb.c

rem assert scanning, code scanning, cov
make -f test.mak clean build XCCASSERTSCAN=on XCCCODESCAN=on XCCCOV=on

rem execute
test.exe

rem generate xcov test report
make -f test.mak xcov-report

rem open test report
xcov\index.html
