TARGET     = iegas
MCU        = msp430g2553

SOURCES  = main.c usci.c protocol.c parameter.c
VPATH    = src/drivers:src
BUILDDIR = build/
INCLUDES = -Iinclude
###############################################################################
CFLAGS   = -mmcu=$(MCU) -g -Wall -Wunused $(INCLUDES) -fshort-enums -O2
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
OBJECTS = $(SOURCES:.c=.o)
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

