CUR_DIR := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
-include $(CUR_DIR)/../makefile.iter

DEP_DIR=/project/grades/Fall-2020/csci3081/dependencies
ifeq ($(USE_REPO_DIR),1)
ROOT_DIR := /home/user/repo
else
ROOT_DIR := $(shell git rev-parse --show-toplevel)
endif
BUILD_DIR=${ROOT_DIR}-build
CXX = g++
-include $(DEP_DIR)/env

TESTEXE = ${BUILD_DIR}/test/csci3081tests
LIBFILE = ${BUILD_DIR}/lib/libcsci3081proj.so

all: $(TESTEXE) 

$(TESTEXE): ${BUILD_DIR}/test $(LIBFILE)
	$(CXX) -std=c++11 -fPIC $(ITERATION_DEFINES) -Wl,-rpath,$(BUILD_DIR)/lib -I$(DEP_DIR)/include -I../src -I. -I.. -L$(DEP_DIR)/lib -L$(BUILD_DIR)/lib *.cc -lgtest_main -lgtest -lgmock -pthread -lcsci3081proj -lEntityProject$(ITERATION) -lCppWebServer -lwebsockets -lssl -lcrypto -lz -o $@

${BUILD_DIR}/test:
	mkdir -p ${BUILD_DIR}/test

clean:
	rm -rf ${BUILD_DIR}/test

