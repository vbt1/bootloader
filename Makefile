#
#   SEGA SATURN Graphic library make file for GNU

# macro
CC = sh-elf-gcc
AS = gasm
LD = sh-elf-ld
RM = rm
CONV = sh-elf-objcopy


CMNDIR = ../common
####OBJECTS = ./saturn/obj
OBJECTS = ./objects

# option
#-mspace 
#CCFLAGS = -m2 -O4 -c  -I. -I./cpu
CCFLAGS =   -m2 -O2 -IL:/saturn/SBL6/SEGALIB/INCLUDE  -fomit-frame-pointer -Wall -c -I. -I./cpu

#CCFLAGS = -m2 -O3 -fomit-frame-pointer -c -I. -I./cpu


#CCFLAGS = -g -O0 -Wall -m2 -DMODEL_S -c -I. -I./cpu
# 
#-I$(SGLIDR)

SATURN  = l:/saturn/SBL6
SEGASMP = $(SATURN)/segasmp
LOCATE_FILE = saturn/saturn.lnk
ENTRY_POINT = START 

CONFIG_FILE = sample.cfg
SEGASMP = $(SATURN)/segasmp

include	$(CONFIG_FILE)

# -m2 must be specified in LDFLAGS so the linker will search the SH2 lib dirs
# Specify path of libsgl.a by using "-L" option

ASFLAGS =
#LDFLAGS = -T $(LOCATE_FILE) -Xlinker -Map \
#          -Xlinker $(MPFILE) -Xlinker -e -Xlinker P   -nostartfiles 

#-T $(LOCATE_FILE) -e $(ENTRY_POINT) -nostartfiles

LDFLAGS = -m2 -Xlinker -T$(LOCATE_FILE) -Xlinker -Map \
         -Xlinker $(MPFILE) -Xlinker -e -Xlinker START  -nostartfiles 



#LDFLAGS = -m2 -L$(SGLLDR) -Xlinker -T$(LDFILE) -Xlinker -Map \
#          -Xlinker $(MPFILE) -Xlinker -e $(ENTRY_POINT) -Xlinker ___Start   -nostartfiles 

####FLAGS = -I. -I./cpu
DFLAGS =
# source_program
include $(OBJECTS)

TARGET0  = fld/Smpfld01.o
TARGET   = root/sl.elf
TARGET1  = root/sl.bin
LDFILE	 = ./$(TARGET:.elf=.lnk)
MPFILE   = $(TARGET:.elf=.map)
MAKEFILE = Makefile

all: $(TARGET) $(TARGET1)

# Use gcc to link so it will automagically find correct libs directory

$(TARGET) : $(SYSOBJS) $(OBJS) $(MAKEFILE) $(OBJECTS) $(LDFILE)
	$(CC) $(LDFLAGS) $(SYSOBJS) $(OBJS) $(LIBS) -o $@


$(TARGET1) : $(SYSOBJS) $(OBJS) $(MAKEFILE) $(LDFILE)
	$(CONV) -O binary $(TARGET) $(TARGET1)

$(LDFILE) : $(MAKEFILE)
#	@echo Making $(LDFILE)
#	@echo SECTIONS {		> $@
#	@echo 	SLSTART 0x06004000 : {	>> $@
#	@echo 		___Start = .;	>> $@
#	@echo 		*(SLSTART)	>> $@
#	@echo 	}			>> $@
#	@echo 	.text ALIGN(0x20) :			>> $@
#	@echo 	{			>> $@
#	@echo 		* (.text)			>> $@
#	@echo 		*(.strings)			>> $@
#	@echo 		__etext = .;			>> $@
#	@echo 	}			>> $@
#	@echo 	SLPROG ALIGN(0x20): {	>> $@
#	@echo 		__slprog_start = .;	>> $@
#	@echo 		*(SLPROG)	>> $@
#	@echo 		__slprog_end = .;	>> $@
#	@echo 	}			>> $@
#	@echo 	.tors  ALIGN(0x10) :			>> $@
#	@echo 	{			>> $@
#	@echo 		___ctors = . ;			>> $@
#	@echo 		*(.ctors)			>> $@
#	@echo 		___ctors_end = . ;			>> $@
#	@echo 		___dtors = . ;			>> $@
#	@echo 		*(.dtors)			>> $@
#	@echo 		___dtors_end = . ;			>> $@
#	@echo 	}			>> $@
#	@echo 	.data ALIGN(0x10):			>> $@
#	@echo 	{			>> $@
#	@echo 		* (.data)			>> $@
#	@echo 		_edata = . ;			>> $@
#	@echo 	}			>> $@
#	@echo 	.bss ALIGN(0x10) (NOLOAD):			>> $@
#	@echo 	{			>> $@
#	@echo 		__bstart = . ;			>> $@
#	@echo 		*(.bss)			>> $@
#	@echo 		* ( COMMON )			>> $@
#	@echo 		__bend = . ;			>> $@
#	@echo 	_end = .;			>> $@
#	@echo 	}			>> $@
#	@echo }				>> $@


# suffix
.SUFFIXES: .asm

#.c.o:
   
#   $(CC) $< $(DFLAGS) $(CCFLAGS) -o $@

saturn/font.o:
	$(CC)  -m2 -Os -fomit-frame-pointer -Wall -c -Il:/saturn/SBL6/SEGASMP/PER/SMPCLIB -Il:/saturn/SBL6/SEGALIB/INCLUDE  -I. saturn/font.c -o $@


fld/Smpfld01.o:
	$(CC)  -m2 -Os -fomit-frame-pointer -Wall -c -Il:/saturn/SBL6/SEGASMP/PER/SMPCLIB -Il:/saturn/SBL6/SEGALIB/INCLUDE  -I. fld/Smpfld01.c -o $@

per_x1.o:
	$(CC) -O3 -fomit-frame-pointer  h:/saturn/SBL6/SEGASMP/PER/SMPCLIB/per_x1.c -o h:/saturn/SBL6/SEGASMP/PER/SMPCLIB/$@


per_x2.o:
	$(CC) -O3 -fomit-frame-pointer  h:/saturn/SBL6/SEGASMP/PER/SMPCLIB/per_x2.c -o h:/saturn/SBL6/SEGASMP/PER/SMPCLIB/$@



%.o: %.s 
	$(AS) $< $(ASFLAGS) $(_ASFLAGS) -o $@

clean:
	$(RM) fld/*.o 

	
#		$(RM) $(OBJS) $(TARGET:.elf=.*)