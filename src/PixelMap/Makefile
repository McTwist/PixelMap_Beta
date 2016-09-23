# Flags
CC = g++
#DEBUG = 1
ifdef DEBUG
	_DEBUG = -g
endif
LIBRARIES = -lpng -lpthread
PROGRAM = pixelmap
DEFINES = -D UNIX=1

# Append flags
CFLAGS += $(DEFINES) $(_DEBUG)
LFLAGS += $(LIBRARIES)

# Prepare files
SOURCES := $(wildcard *.cpp)
HEADERS := $(wildcard *.h) $(wildcard *.hpp)
OBJS := $(patsubst %.cpp, %.o, $(SOURCES))
DEPS := $(patsubst %.o, %.d, $(OBJS))

# Force the commands to work
.PHONY: make install clean debug test

# Create dependencies
make: $(SOURCES) $(HEADERS)
	$(CC) -MD -E $(SOURCES) > /dev/null
#	make -k --no-print-directory install

# Linking
install: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $(PROGRAM)

# Individual compilation
$(OBJS):
	$(CC) $(CFLAGS) -c $<

# Clean up
clean:
	rm -f *.o *.d

#Debug
GDB_ARGUMENTS = --quiet --batch -ex "run -w 1"
debug:
	gdb $(GDB_ARGUMENTS) $(PROGRAM)

# Run some tests
_W = World1
test:
	./$(PROGRAM) -w $(_W) -t -l testlog.txt -f $(_W)-log.png --no-gui
	./$(PROGRAM) -w $(_W) -t -x w l -f $(_W)-exclude.png --no-gui
	./$(PROGRAM) -w $(_W) -t -n -f $(_W)-night.png --no-gui
	./$(PROGRAM) -w $(_W) -t -s 64 1 -f $(_W)-single-slice64.png --no-gui
	./$(PROGRAM) -w $(_W) -t -h g -f $(_W)-gray.png --no-gui
	./$(PROGRAM) -w $(_W) -t -h c -f $(_W)-color.png --no-gui
	./$(PROGRAM) -w $(_W) -t -h s -f $(_W)-solid.png --no-gui
	./$(PROGRAM) -w $(_W) -t -c -f $(_W)-cave.png --no-gui
	./$(PROGRAM) -w $(_W) -t -r 90 -f $(_W)-rot90.png --no-gui
	./$(PROGRAM) -w $(_W) -t --cache -f $(_W)-cache.png --no-gui

run:
	./$(PROGRAM) -w 1

# Include dependencies
-include $(DEPS)
