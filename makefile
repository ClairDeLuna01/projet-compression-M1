
default: all

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SOURCESCPP := $(call rwildcard,src,*.cpp)
OBJCPP := $(SOURCESCPP:src/%.cpp=obj/%.o)
OBJCPP += obj/main.o

SOURCESC := $(call rwildcard,src,*.c)
OBJC := $(SOURCESC:src/%.c=obj/%.o)

DEPDIR := .deps
DEPFLAGS_BASE = -MT $@ -MMD -MP -MF $(DEPDIR)
DEPFLAGS = $(DEPFLAGS_BASE)/$*.d
DEPFLAGSMAIN = $(DEPFLAGS_BASE)/main.d

ifeq ($(OS),Windows_NT)
	CFLAGS = -g -O0 -Wall -Iinclude -mavx -mavx2 -Wno-strict-aliasing -Wno-maybe-uninitialized -Wno-array-bounds -Wno-unused-function
	FLTKFLAGS = -lfltk -lm -lgdi32 -lole32 -lfltk -lcomctl32 -luuid -lws2_32 
else
	CFLAGS = -g -O3 -Wall -Iinclude -mavx -mavx2 -Wno-strict-aliasing -Wno-maybe-uninitialized -Wno-array-bounds -Wno-unused-function
	FLTKFLAGS = -lfltk -lm -lX11 -lXext -lXft 
endif

EXE = mosaic
EXE_GUI = mosaic_gui

ifeq ($(OS),Windows_NT)
	RM = del /s /f /q
else
	RM = rm -f
endif

obj/%.o: src/%.cpp
	g++ -c $(DEPFLAGS_BASE) $(DEPFLAGS) src/$*.cpp -o obj/$*.o $(CFLAGS)

obj/%.o: src/%.c
	g++ -c $(DEPFLAGS_BASE) $(DEPFLAGS) src/$*.c -o obj/$*.o $(CFLAGS)

obj/main.o: main.cpp
	g++ -c $(DEPFLAGS_BASE) $(DEPFLAGSMAIN) main.cpp -o obj/main.o $(CFLAGS)

$(EXE): $(OBJCPP) $(OBJC)
	g++ $(OBJCPP) $(OBJC) -o $(EXE) $(CFLAGS)

$(EXE_GUI): $(OBJCPP) $(OBJC)
	g++ GUImain.cpp obj/STB_IMPLEMENTATION.o -o $(EXE_GUI) $(CFLAGS) $(FLTKFLAGS)

all: $(EXE) $(EXE_GUI)

clean:
ifeq  ($(OS),Windows_NT)
	$(RM) obj\*.o $(EXE).exe $(EXE_GUI).exe $(DEPDIR)\*.d
else
	$(RM) obj/*.o $(EXE) $(EXE_GUI) $(DEPDIR)/*.d
endif

reinstall: clean default

$(DEPDIR): ; @mkdir $@

DEPFILES := $(SOURCES:$(SDIR)/%.cpp=$(DEPDIR)/%.d)
DEPFILES += $(DEPDIR)/main.d
# $(info $(DEPFILES))
$(DEPFILES):


include $(wildcard $(DEPFILES))

.PHONY: clean reinstall all

CStestFox32 :
	.\mosaic.exe .\data\in\RedFox.png 32 RGB
	.\mosaic.exe .\data\in\RedFox.png 32 HSL
	.\mosaic.exe .\data\in\RedFox.png 32 HWB
	.\mosaic.exe .\data\in\RedFox.png 32 LRGB
	.\mosaic.exe .\data\in\RedFox.png 32 XYZD50
	.\mosaic.exe .\data\in\RedFox.png 32 LAB
	.\mosaic.exe .\data\in\RedFox.png 32 LCH
	.\mosaic.exe .\data\in\RedFox.png 32 XYZD65
	.\mosaic.exe .\data\in\RedFox.png 32 OKLAB
	.\mosaic.exe .\data\in\RedFox.png 32 OKLCH
	.\mosaic.exe .\data\in\RedFox.png 32 SRGB
	.\mosaic.exe .\data\in\RedFox.png 32 DISPLAYP3
	.\mosaic.exe .\data\in\RedFox.png 32 A98RGB
	.\mosaic.exe .\data\in\RedFox.png 32 PROPHOTORGB
	.\mosaic.exe .\data\in\RedFox.png 32 REC2020

CStestCat48 :
	.\mosaic.exe .\data\in\cat.png 48 RGB
	.\mosaic.exe .\data\in\cat.png 48 HSL
	.\mosaic.exe .\data\in\cat.png 48 HWB
	.\mosaic.exe .\data\in\cat.png 48 LRGB
	.\mosaic.exe .\data\in\cat.png 48 XYZD50
	.\mosaic.exe .\data\in\cat.png 48 LAB
	.\mosaic.exe .\data\in\cat.png 48 LCH
	.\mosaic.exe .\data\in\cat.png 48 XYZD65
	.\mosaic.exe .\data\in\cat.png 48 OKLAB
	.\mosaic.exe .\data\in\cat.png 48 OKLCH
	.\mosaic.exe .\data\in\cat.png 48 SRGB
	.\mosaic.exe .\data\in\cat.png 48 DISPLAYP3
	.\mosaic.exe .\data\in\cat.png 48 A98RGB
	.\mosaic.exe .\data\in\cat.png 48 PROPHOTORGB
	.\mosaic.exe .\data\in\cat.png 48 REC2020

CStestGradient :
	.\mosaic.exe .\data\in\gradient.png 20 RGB
	.\mosaic.exe .\data\in\gradient.png 20 HSL
	.\mosaic.exe .\data\in\gradient.png 20 HWB
	.\mosaic.exe .\data\in\gradient.png 20 LRGB
	.\mosaic.exe .\data\in\gradient.png 20 XYZD50
	.\mosaic.exe .\data\in\gradient.png 20 LAB
	.\mosaic.exe .\data\in\gradient.png 20 LCH
	.\mosaic.exe .\data\in\gradient.png 20 XYZD65
	.\mosaic.exe .\data\in\gradient.png 20 OKLAB
	.\mosaic.exe .\data\in\gradient.png 20 OKLCH
	.\mosaic.exe .\data\in\gradient.png 20 SRGB
	.\mosaic.exe .\data\in\gradient.png 20 DISPLAYP3
	.\mosaic.exe .\data\in\gradient.png 20 A98RGB
	.\mosaic.exe .\data\in\gradient.png 20 PROPHOTORGB
	.\mosaic.exe .\data\in\gradient.png 20 REC2020