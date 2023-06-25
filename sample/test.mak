ifneq ($(SILENT),off)
MAKEFLAGS += --silent
endif

TARGET = test.exe libtest.a

#XCCASSERTSCAN := on
#XCCCODESCAN := on
#XCCCOV := on

# env
CC_EXEC := gcc.exe
LINK_EXEC := gcc.exe
XCC_EXEC := ../tools/xcc.exe
XCOVPOST_EXEC := ../tools/xcovpost.exe
XCOVHTML_EXEC := ../tools/xcovhtml.exe

# options
CFLAGS = 
CFLAGS += -D _UNICODE -D UNICODE
CFLAGS += -D WIN32 -D OPEN_CPP_COVERAGE_EXPORTS -D _DEBUG -D _CONSOLE -D _LIB

LFLAGS = 
LFLAGS += -lstdc++

CFLAGS += -fprofile-arcs -ftest-coverage
LFLAGS += -fprofile-arcs -ftest-coverage

SOURCES = 
SRCDIRS = 

SOURCES += \
  main.c \
  funcs.c \
  cscan2.c \
  cscan.c \
  assertscan.c

# framework
ifeq ($(XCCCODESCAN),on)
XCC_CMD_OPTS += --cmd=codescan --resultfile=./xcc-cscan.log --protodbfile=./xcc-protodb.dat
ifneq ($(XCCCODESCAN_RULEFILE),)
XCC_CMD_OPTS += --rulefile=$(XCCCODESCAN_RULEFILE)
endif
endif
ifeq ($(XCCASSERTSCAN),on)
XCC_CMD_OPTS += --cmd=assertscan --assertfunc=my_assert --assertfunc=my_assert2 --assertdbfile=assertscandb.c.i
endif
ifeq ($(XCCCOV),on)
XCC_CMD_OPTS += --cmd=cov --keep-prec
endif
ifeq ($(XCCDRYSCAN),on)
XCC_CMD_OPTS += --cmd=dryscan
endif
ifneq ($(wildcard $(XCC_EXEC)),)
ifneq ($(XCC_CMD_OPTS),)
CC_EXEC := $(XCC_EXEC) $(XCC_CMD_OPTS) $(CC_EXEC)
endif
endif

OBJECTS += $(foreach source, $(SOURCES), obj/$(basename $(source)).o)
GCNOFILES += $(foreach source, $(SOURCES), obj/$(basename $(source)).gcno)

$(shell mkdir -p obj)
$(shell mkdir -p xcov)

vpath %.c . $(SRCDIRS)
vpath %.cxx . $(SRCDIRS)
vpath %.cpp . $(SRCDIRS)

.PHONY: build
build : init-build $(TARGET)

test.exe : $(OBJECTS)
	echo Link $@ ... 2>&1 | tee -a build.log 
ifneq ($(wildcard $(XCC_EXEC)),)
	$(XCC_EXEC) $(LINK_EXEC) $(OBJECTS) $(LFLAGS) -o $@ 2>&1 | tee -a build.log 
else
	$(LINK_EXEC) $(OBJECTS) $(LFLAGS) -o $@ 2>&1 | tee -a build.log 
endif

libtest.a : $(OBJECTS)
	echo Archive $@ ... 2>&1 | tee -a build.log 
	ar rcs $@ $(OBJECTS) 2>&1 | tee -a build.log 

.PHONY: init-build
init-build:
	-mkdir -p obj
	echo 2>&1 | tee build.log 

.PHONY: clean
clean : 
	echo Clean ...
	rm -f $(TARGET)
	rm -f xcc-cscan.log
	rm -f xcc-protodb.dat
	rm -rf obj xcov

xcov-report : force
	$(XCOVHTML_EXEC) -d obj -o xcov
  
obj/%.o : %.c
	echo Compiling $<
	$(CC_EXEC) -c $(CFLAGS) $< -o $@

obj/%.o : %.cxx
	echo Compiling $<
	$(CC_EXEC) -c $(CFLAGS) $< -o $@

obj/%.o : %.cpp
	echo Compiling $<
	$(CC_EXEC) -c $(CFLAGS) $< -o $@

force:
	
