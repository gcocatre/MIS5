EXENAME	= MIS5

SRCDIR	= src
INCDIR	= include
OBJDIR	= obj
BINDIR	= bin

SOURCES	= $(wildcard $(SRCDIR)/*.cpp)
OBJECTS	= $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
HEADERS	= $(wildcard include/*.hpp)
EXEFILE	= $(BINDIR)/$(EXENAME)

SHELL			= /bin/bash
CXX				= g++
#CXXFLAGS		=
REQ_CXXFLAGS	= -std=c++11 -D_THREAD_SAFE
DEBUG_CXXFLAGS	= -g -O -Wall
PROD_CXXFLAGS	= -O3
BUILD_CXXFLAGS	= $(REQ_CXXFLAGS) $(DEBUG_CXXFLAGS)
IFLAGS			= -I /usr/local/include
LDFLAGS			= -L /usr/local/lib
LDLIBS			= -lSDL2 -lSDL2main -lSDL2_ttf -lpthread

all: $(EXEFILE)

production: prod

prod: BUILD_CXXFLAGS = $(REQ_CXXFLAGS) $(PROD_CXXFLAGS)
prod: clean all

debug: BUILD_CXXFLAGS = $(REQ_CXXFLAGS) $(DEBUG_CXXFLAGS)
debug: clean all

$(EXEFILE): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(OBJECTS) $(BUILD_CXXFLAGS) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -o $@

# Generic rule for producing .o files from .cpp files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	@mkdir -p $(OBJDIR)
	$(CXX) -c $(BUILD_CXXFLAGS) $(CXXFLAGS) -I $(INCDIR) $(IFLAGS) -o $@ $<

clean:
	rm -rf $(OBJDIR) $(BINDIR)




# CXX      = g++

# INCLUDE  = -I/usr/local/include
# LIB      = -L/usr/local/lib
# CXXFLAGS = -O3 -Wall -std=c++11
# SDLFLAGS = `sdl2-config --cflags --libs`
# LDFLAGS  = -O3 -Wall

# EXE = MIS

# SRC = src
# DEP = include
# OBJ = obj
# BIN = bin

# SRCS    = $(wildcard $(SRC)/*.cpp)
# OBJS    = $(SRCS:$(SRC)/%.cpp=$(OBJ)/%.o)
# DEPS    = $(wildcard include/*.hpp)
# LIBS    = -lSDL2 -lSDL2main -lSDL2_ttf -lpthread

# all: $(BIN)/$(EXE)

# $(BIN)/$(EXE): $(OBJS)
# 	@mkdir -p $(BIN)
# 	$(CXX) $(LDFLAGS) $(LIB) $(SDLFLAGS) -o $(BIN)/$(EXE) $(OBJS) $(LIBS)

# $(OBJ)/%.o:$(SRC)/%.cpp $(DEPS)
# 	@mkdir -p $(OBJ)
# 	$(CXX) -o $@ $(CXXFLAGS) $< -c -I $(DEP)/ $(INCLUDE)

# clean:
# 	rm -f *~ $(SRC)/*~ $(DEP)/*~
# 	rm -fr $(OBJ) $(BIN)
