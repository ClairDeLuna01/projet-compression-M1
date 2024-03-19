
default: main

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SOURCESCPP := $(call rwildcard,src,*.cpp)
OBJCPP := $(SOURCESCPP:src/%.cpp=obj/%.o)
OBJCPP += obj/main.o

SOURCESC := $(call rwildcard,src,*.c)
OBJC := $(SOURCESC:src/%.c=obj/%.o)

CFLAGS = -O3 -Wall -Iinclude -mavx -mavx2

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
