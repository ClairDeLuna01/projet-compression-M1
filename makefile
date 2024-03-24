
default: main

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SOURCESCPP := $(call rwildcard,src,*.cpp)
OBJCPP := $(SOURCESCPP:src/%.cpp=obj/%.o)
OBJCPP += obj/main.o

SOURCESC := $(call rwildcard,src,*.c)
OBJC := $(SOURCESC:src/%.c=obj/%.o)

CFLAGS = -O3 -Wall -Iinclude -mavx -mavx2 -Wno-strict-aliasing -Wno-maybe-uninitialized

EXE = mosaic

obj/%.o: src/%.cpp
	g++ -c src/$*.cpp -o obj/$*.o $(CFLAGS)

obj/%.o: src/%.c
	g++ -c src/$*.c -o obj/$*.o $(CFLAGS)

obj/main.o: main.cpp
	g++ -c main.cpp -o obj/main.o $(CFLAGS)

main: $(OBJCPP) $(OBJC)
	g++ $(OBJCPP) $(OBJC) -o $(EXE) $(CFLAGS)

clean:
	rm -f obj/*.o main

reinstall: clean main

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