PROGNAME= yamlparcer

SRC = aliascontent.cpp conversion.cpp emitter.cpp emitterstate.cpp \
	emitterutils.cpp exp.cpp iterator.cpp map.cpp node.cpp null.cpp \
	ostream.cpp parser.cpp parserstate.cpp regex.cpp scalar.cpp \
	scanner.cpp scanscalar.cpp scantag.cpp scantoken.cpp sequence.cpp \
	simplekey.cpp stream.cpp tag.cpp

OBJ = $(SRC:.cpp=.o)
OUTDIR = ../../libs/
OUT = lib$(PROGNAME).a

# include directories
INCLUDES = -I. -I../include/

# C++ compiler flags (-g -O2 -Wall)
CCFLAGS = -g

# compiler
CCC = g++

# compile flags
LDFLAGS = -g

.SUFFIXES: .cpp

default: $(OUT)

.cpp.o:
	$(CCC) $(INCLUDES) $(CCFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	mkdir -p $(OUTDIR)
	ar rcs $(OUTDIR)$(OUT) $(OBJ)

clean:
	rm -f $(OBJ) $(OUT)