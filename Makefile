BIN = Hell
CC  = g++
LD  = g++
STD = -std=c++20

SDIR = src
ODIR = obj/debug

LIBS  = SFML box2d rusty
DLNKS = sfml-audio-s-d sfml-graphics-s-d sfml-window-s-d sfml-system-s-d openal32 flac vorbisenc vorbisfile vorbis ogg freetype ws2_32 gdi32 opengl32 winmm box2d rusty
RLNKS = sfml-audio-s   sfml-graphics-s   sfml-window-s   sfml-system-s   openal32 flac vorbisenc vorbisfile vorbis ogg freetype ws2_32 gdi32 opengl32 winmm box2d rusty

DEFINES = -DSFML_STATIC -DDEBUG
INCDIRS = $(patsubst %, -Ilib/%/include, $(LIBS)) -I$(SDIR)
LIBDIRS = $(patsubst %, -Llib/%/bin, $(LIBS))
DLINKS  = $(patsubst %, -l%, $(DLNKS))
RLINKS  = $(patsubst %, -l%, $(RLNKS))

CFLAGS = $(INCDIRS) $(DEFINES) $(STD) -Wall
LFLAGS = $(LIBDIRS) $(DLINKS)  $(STD) -Wall

DEPS = $(wildcard $(SDIR)/*.h) \
	   $(wildcard $(SDIR)/entity/*.h) \
	   $(wildcard $(SDIR)/graphics/*.h) \
	   $(wildcard $(SDIR)/physics/*.h) \
	   src/pch.h.gch
SRCS = $(wildcard $(SDIR)/*.cpp) \
	   $(wildcard $(SDIR)/entity/*.cpp) \
	   $(wildcard $(SDIR)/graphics/*.cpp) \
	   $(wildcard $(SDIR)/physics/*.cpp)
OBJS = $(patsubst $(SDIR)/%.cpp, $(ODIR)/%.o, $(SRCS))


$(BIN): $(OBJS)
	@echo "  linking:" $@
	@$(LD) -o $@ $^ $(LFLAGS)

$(BIN)-r: $(OBJS)
	@echo "  linking:" $@
	@$(LD) -o $@ $^ $(LFLAGS)

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	@echo "compiling:" $<
	@$(CC) -o $@ -c $< $(CFLAGS)


$(SDIR)/pch.h.gch: $(SDIR)/pch.h
	@echo "compiling: pch.h"
	@$(CC) -x c++-header $< $(CFLAGS)


.PHONY: clean

clean:
	rm -f $(SDIR)/pch.h.gch
	rm -f $(OBJS)
	rm -f $(BIN)
	rm -f $(BIN)-r


.PHONY: release

release: ODIR = obj/release
release: OBJS = $(patsubst $(SDIR)/%.cpp, $(ODIR)/%.o, $(SRCS))
release: CFLAGS = $(INCDIRS) $(DEFINES) $(STD) -Wall -O3
release: LFLAGS = $(LIBDIRS) $(DLINKS)  $(STD) -Wall -O3
release: $(BIN)-r


.PHONY: run

run: $(BIN)
	@echo "  running: ----------"
	@./$(BIN)


preproc/main.o: preproc/main.cpp
	@echo "compiling: preproc"
	@$(CC) -o preproc/main.o -c preproc/main.cpp -Ilib/rustjson -DDEBUG -std=c++20 -Wall

preproc: preproc/main.o
	@echo "  linking: preproc"
	@$(LD) -o preproc preproc/main.o -std=c++20 -Wall
	@echo "  running: preproc"
	@./preproc.exe res/testlevel.preproc.json res/testlevel.json

