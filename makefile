
default: main

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SOURCESCPP := $(call rwildcard,src,*.cpp)
OBJCPP := $(SOURCESCPP:src/%.cpp=obj/%.o)
OBJCPP += obj/main.o

SOURCESC := $(call rwildcard,src,*.c)
OBJC := $(SOURCESC:src/%.c=obj/%.o)

CFLAGS = -g -Wall -Ofast

obj/%.o: src/%.cpp
	g++ -c -g src/$*.cpp -o obj/$*.o $(CFLAGS)

obj/%.o: src/%.c
	g++ -c -g src/$*.c -o obj/$*.o $(CFLAGS)

obj/main.o: main.cpp
	g++ -c -g main.cpp -o obj/main.o $(CFLAGS)

main: $(OBJCPP) $(OBJC)
	g++ $(OBJCPP) $(OBJC) -o main $(CFLAGS)

clean:
	rm -f obj/*.o main

reinstall: clean main
