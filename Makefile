CUR_DIR := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
-include $(CUR_DIR)/makefile.iter

DEP_DIR=/project/grades/Fall-2020/csci3081/dependencies
ifeq ($(USE_REPO_DIR),1)
ROOT_DIR := /home/user/repo
else
ROOT_DIR := $(shell git rev-parse --show-toplevel)
endif
BUILD_DIR=${ROOT_DIR}-build
CXX = g++
CXXFLAGS = -std=c++11 -g -fPIC $(ITERATION_DEFINES)
INCLUDES = -I.. -I$(DEP_DIR)/include -Isrc -I.
LIBDIRS = -L$(DEP_DIR)/lib 
LIBS = -lEntityProject$(ITERATION)
SOURCES = $(wildcard src/*.cc)  $(wildcard src/*/*.cc)
OBJFILES = $(notdir $(SOURCES:.cc=.o))
OBJDIR = $(BUILD_DIR)/obj
OBJECTS = $(addprefix $(OBJDIR)/, $(OBJFILES))
-include $(DEP_DIR)/env

LIBFILE = ${BUILD_DIR}/lib/libcsci3081proj.so
EXEFILE = ${BUILD_DIR}/bin/web-app

all: $(EXEFILE)

# Compile source files
$(OBJDIR)/%.o: src/%.cc
	$(call make-depend-cxx,$<,$@,$(subst .o,.d,$@))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile source files
$(OBJDIR)/%.o: src/config/%.cc
	$(call make-depend-cxx,$<,$@,$(subst .o,.d,$@))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Generate dependencies
make-depend-cxx=$(CXX) -MM -MF $3 -MP -MT $2 $(CXXFLAGS) $(INCLUDES) $1
-include $(addprefix $(OBJDIR)/,$(OBJFILES:.o=.d))

# Build shared library
$(LIBFILE): $(BUILD_DIR) $(OBJECTS)
	$(CXX) $(CXXFLAGS) -shared $(INCLUDES) $(LIBDIRS) $(LIBS) $(OBJECTS) -o $@

data-files:
	rsync -au $(DEP_DIR)/share/EntityProject/models web/
	rsync -au $(DEP_DIR)/share/EntityProject/textures web/
	rsync -au $(DEP_DIR)/share/EntityProject/data ./

# Build web viewer
$(EXEFILE): $(BUILD_DIR) $(LIBFILE) data-files
	$(CXX) $(CXXFLAGS) -Wl,-rpath,$(BUILD_DIR)/lib $(INCLUDES) $(LIBDIRS) -L$(BUILD_DIR)/lib src/main.cc -lcsci3081proj $(LIBS) -lCppWebServer -lwebsockets -lssl -lcrypto -lz -o $@ 

# Build tests
test: $(LIBFILE)
	cd tests; make

# Make build directory
$(BUILD_DIR):
	mkdir -p ${BUILD_DIR}/obj
	mkdir -p ${BUILD_DIR}/lib
	mkdir -p ${BUILD_DIR}/bin

clean-data:
	rm -rf web/models
	rm -rf web/textures
	rm -rf web/data

# Clean build directory
clean:
	rm -rf ${BUILD_DIR}
