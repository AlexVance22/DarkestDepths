BIN = Hell
EXT = cpp
STD = -std=c++17
CC = g++
LD = g++

SDIR = src
ODIR = obj

DEFS = SFML_STATIC
LIBS = SFML box2d
DLNKS = sfml-audio-s-d sfml-graphics-s-d sfml-window-s-d sfml-system-s-d openal32 flac vorbisenc vorbisfile vorbis ogg freetype ws2_32 gdi32 opengl32 winmm box2d
RLNKS = sfml-audio-s   sfml-graphics-s   sfml-window-s   sfml-system-s   openal32 flac vorbisenc vorbisfile vorbis ogg freetype ws2_32 gdi32 opengl32 winmm box2d

DEFINES = $(patsubst %, -D %, $(DEFS))
INCDIRS = $(patsubst %, -Ilib/%/include, $(LIBS)) -I$(SDIR) -Ilib/rustjson
LIBDIRS = $(patsubst %, -Llib/%/bin, $(LIBS))
DLINKS  = $(patsubst %, -l%, $(DLNKS))
RLINKS  = $(patsubst %, -l%, $(RLNKS))

CFLAGS = $(INCDIRS) $(DEFINES) $(STD) -Wall
LFLAGS = $(LIBDIRS) $(DLINKS) $(STD) -Wall

DEPS = $(wildcard $(SDIR)/*.h) \
	   $(wildcard $(SDIR)/core/*.h)
SRCS = $(wildcard $(SDIR)/*.$(EXT)) \
       $(wildcard $(SDIR)/core/*.$(EXT))
OBJS = $(patsubst $(SDIR)/%.$(EXT), $(ODIR)/%.o, $(SRCS))


$(BIN): $(OBJS)
	@echo "  linking:" $@
	@$(LD) -o $@ $^ $(LFLAGS)

$(BIN)-r: $(OBJS)
	@echo "  linking:" $@
	@$(LD) -o $@ $^ $(LFLAGS)

$(ODIR)/%.o: $(SDIR)/%.$(EXT) $(DEPS)
	@echo "compiling:" $<
	@$(CC) -o $@ -c $< $(CFLAGS)


.PHONY: clean

clean:
	rm -f $(OBJS)
	rm -f $(BIN)
	rm -f $(BIN)-r


.PHONY: all

all: $(BIN)


.PHONY: release

release: CFLAGS = $(INCDIRS) -DNDEBUG  $(STD) -Wall -O2
release: LFLAGS = $(LIBDIRS) $(RLINKS) $(STD) -Wall -O2
release: clean
release: $(BIN)-r


.PHONY: run

run: $(BIN)
	@echo "  running: ----------"
	@./$(BIN)


preproc/main.o: preproc/main.cpp
	@echo "compiling: preproc"
	@g++ -o preproc/main.o -c preproc/main.cpp -std=c++20 -Wall -Ilib/rustjson -DNDEBUG

preproc: preproc/main.o
	@echo "  linking: preproc"
	@g++ -o preproc preproc/main.o -std=c++20 -Wall
	@echo "  running: preproc"
	@./preproc.exe res/testlevel.preproc.json res/testlevel.json

