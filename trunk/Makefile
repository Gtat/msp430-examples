#
# # Makefile for msp430
#  #
# 'make' builds everything
#  # 'make clean' deletes everything except source files and Makefile
# You need to set TARGET, MCU and SOURCES for your project.
#  # TARGET is the name of the executable file to be produced 
# $(TARGET).elf $(TARGET).hex and $(TARGET).txt nad $(TARGET).map are all generated.
#  # The TXT file is used for BSL loading, the ELF can be used for JTAG use
# 
TARGET     = skeleton
MCU        = msp430g2553
#    # List all the source files here
# eg if you have a source file foo.c then list it here
SOURCES  = main.c usci.c
VPATH    = src/drivers:src
BUILDDIR = build/
#   # Include are located in the Include directory
INCLUDES = -Iinclude
# Add or subtract whatever MSPGCC flags you want. There are plenty more
###############################################################################
CFLAGS   = -mmcu=$(MCU) -g -O2 -Wall -Wunused $(INCLUDES) -fshort-enums
ASFLAGS  = -mmcu=$(MCU) -x assembler-with-cpp -Wa,-gstabs
LDFLAGS  = -mmcu=$(MCU) -Wl,-Map=$(TARGET).map
###############################################################################
CC       = msp430-gcc
LD       = msp430-ld
AR       = msp430-ar
AS       = msp430-gcc
GASP     = msp430-gasp
NM       = msp430-nm
OBJCOPY  = msp430-objcopy
RANLIB   = msp430-ranlib
STRIP    = msp430-strip
SIZE     = msp430-size
READELF  = msp430-readelf
MAKETXT  = srec_cat
CP       = cp -p
RM       = rm -f
MV       = mv
################################################################################
# the file which will include dependencies
#  DEPEND = $(SOURCES:.c=.d)
#   # all the object files
OBJECTS = $(SOURCES:.c=.o)
all: $(TARGET).elf $(TARGET).hex $(TARGET).text Makefile clean
$(TARGET).elf: $(OBJECTS)
	echo
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $@
	$(SIZE) $(TARGET).elf

%.hex: %.elf
	@$(OBJCOPY) -O ihex $< $@
%.text: %.hex
	@$(MAKETXT) -O $@ -TITXT $< -I
	@unix2dos $(TARGET).text
#  The above line is required for the DOS based TI BSL tool to be able to read 
#  the txt file generated from linux/unix systems.
$(BUILDDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

%.lst: %.c
	$(CC) -c $(ASFLAGS) -Wa,-anlhd $< > $@

# include the dependencies unless we're going to clean, then forget about them.
ifneq ($(MAKECMDGOALS), clean)
	-include $(DEPEND)
endif
%.d: %.c
	$(CC) -M ${CFLAGS} $< >$@

.PHONY:	clean
clean:
	@$(RM) $(OBJECTS)
#	@$(RM) $(TARGET).*
	@$(RM) $(SOURCES:.c=.lst)
	@$(RM) $(DEPEND)

