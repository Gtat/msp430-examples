TARGET     = iegas
MCU        = msp430g2553

SOURCES  = main.c         \
           utility.c      \
           adc.c          \
           flash.c        \
           usci.c         \
           protocol.c     \
           parameter.c    \
           processing.c   \
           ram_symbols.c

VPATH    = src/drivers:src
BUILDDIR = build/
ASMDIR   = asm
INCLUDES = -Iinclude
LDSCRIPT = -Tlink/msp430g2553.ld
###############################################################################
CFLAGS   = -mmcu=$(MCU) $(LDSCRIPT) -g -Wall $(INCLUDES) -fshort-enums -Os
ASFLAGS  = -mmcu=$(MCU) $(LDSCRIPT) -x assembler-with-cpp -Wa,-gstabs
LDFLAGS  = -mmcu=$(MCU) $(LDSCRIPT) -Wl,-Map=$(TARGET).map
###############################################################################
PREFIX   = msp430-elf
CC       = $(PREFIX)-gcc
LD       = $(PREFIX)-ld
AR       = $(PREFIX)-ar
AS       = $(PREFIX)-gcc
GASP     = $(PREFIX)-gasp
NM       = $(PREFIX)-nm
OBJCOPY  = $(PREFIX)-objcopy
RANLIB   = $(PREFIX)-ranlib
STRIP    = $(PREFIX)-strip
SIZE     = $(PREFIX)-size
READELF  = $(PREFIX)-readelf
MAKETXT  = srec_cat
CP       = cp -p
RM       = rm -f
MV       = mv
################################################################################
OBJECTS  = $(SOURCES:.c=.o)
all: $(TARGET).elf $(TARGET).hex $(TARGET).text Makefile clean
$(TARGET).elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $@
	$(SIZE) $(TARGET).elf

%.hex: %.elf
	@$(OBJCOPY) -O ihex $< $@
%.text: %.hex
	@$(MAKETXT) -O $@ -TITXT $< -I
	@unix2dos $(TARGET).text
#  The above line is required for the DOS based TI BSL tool to be able to read
#  the txt file generated from linux/unix systems.

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

# include the dependencies unless we're going to clean, then forget about them.
#ifneq ($(MAKECMDGOALS), clean)
#	-include $(DEPEND)
#endif
%.d: %.c
	$(CC) -M ${CFLAGS} $< >$@

.PHONY:	clean
clean:
	@$(RM) $(OBJECTS)
#	@$(RM) $(TARGET).*
#	@$(RM) $(SOURCES:.c=.lst)
	@$(RM) $(DEPEND)
