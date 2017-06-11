# This file needs avra version >= 1.2.0

PROJECT=akl-port
DEVICE=t2313
LFUSE=0x64
HFUSE=0xDF

all: isp eeprom fuses

par:
	avra $(PROJECT).asm
	avrpp -fl$(LFUSE) -fh$(HFUSE) $(PROJECT).hex $(PROJECT).eep.hex

hispai:
	avra $(PROJECT).asm
	hispai -m -i 100k -wp $(PROJECT).hex -we $(PROJECT).eep.hex -fl $(LFUSE) -fh $(HFUSE)

stk:
	avra $(PROJECT).asm
	avrdude -c stk500pp -p $(DEVICE) -U flash:w:$(PROJECT).hex -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m
	#-U eeprom:w:$(PROJECT).eep.hex

isp:
	avra $(PROJECT).asm
	avrdude -c butterfly -p $(DEVICE) -U flash:w:$(PROJECT).hex
	
eeprom:
	avra $(PROJECT).asm
	avrdude -c butterfly -p $(DEVICE) -U eeprom:w:$(PROJECT).eep.hex
	
fuses:
	avrdude -c avr910 -p $(DEVICE) -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m

