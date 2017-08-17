ProjectName := server_rbt
MajorVersion := 0
SubVersion   := 1

objs += \
main.o \
fsm_main.o \
comm_main.o \
oa.o \
hardware_main.o \
hardware_gpio.o \
hardware_i2c.o \
hardware_tca1116.o \
hardware_spi.o \
hardware_uart.o \
hardware_camera.o

AS       = $(CROSS_COMPILE)as
AR       = $(CROSS_COMPILE)ar
CC       = $(CROSS_COMPILE)gcc
LD       = $(CROSS_COMPILE)ld
NM       = $(CROSS_COMPILE)nm
OBJCOPY  = $(CROSS_COMPILE)objcopy
OBJDUMP  = $(CROSS_COMPILE)objdump
RANLIB   = $(CROSS_COMPILE)ranlib
READELF  = $(CROSS_COMPILE)readelf
SIZE     = $(CROSS_COMPILE)size
STRINGS  = $(CROSS_COMPILE)strings
STRIP    = $(CROSS_COMPILE)strip

VPATH += \
	./ \
	./FSM \
	./Comm \
	./Hardware \
	./OrderArbitrate

IncludePath += \
	-I./ \
	-I./FSM \
	-I./Comm \
	-I./Hardware \
	-I./OrderArbitrate

LDFLAGS = -lpthread -lm
LDFLAGS += `pkg-config --libs opencv`
#LDFLAGS += `pkg-config --libs gtk+-2.0`
#LDFLAGS += `pkg-config --libs gthread-2.0`


CFLAGS += -w -fpic -g $(IncludePath) -D MAJOR_VER=$(MajorVersion) -D SUB_VER=$(SubVersion)
CFLAGS += `pkg-config --cflags opencv` 
#CFLAGS += `pkg-config --cflags gtk+-2.0`
#CFLAGS += `pkg-config --cflags gthread-2.0`
#CFLAGS += -pthread

#CFLAGS += `pkg-config --cflags libjpeg`
#CFLAGS += -O3


$(ProjectName) : $(objs) *.h
	$(CC) $(CFLAGS) -o $(ProjectName) $(objs) $(LDFLAGS)

clean : 
	rm *.o $(ProjectName)
