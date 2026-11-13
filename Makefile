EE_BIN = ejectdvd_menu.elf
EE_OBJS = main.o
EE_LIBS = -ldebug -lc -lpad

all: $(EE_BIN) 
	$(EE_STRIP) --strip-all $(EE_BIN)


run: $(EE_BIN)
	ps2client execee host:$(EE_BIN)

reset:
	ps2client reset

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal

