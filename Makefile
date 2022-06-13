CC=gcc
CXX=g++
BINARY=medfuzz
TEST_BINARY=medfuzz-test
LANG=-std=c++17
ABI_FLAGS=-fPIC
BASE_DIR=./
LDIR=.
SRC_DIR=./src/
TEST_SRC_DIR=./tests/
BUILD_DIR=./build/
TEST_BUILD_DIR=$(BUILD_DIR)tests/
IDIR=-I${SRC_DIR} -I./include/
GTEST_DIR=-I./include/googletest/googletest/include/
GMOCK_DIR=-I./include/googletest/googlemock/include/
DBG_FLAGS=-Wall -g -O0 -fno-omit-frame-pointer
LIBS=-pthread
GTEST_LIB_DIR=-L./include/googletest/lib/
TEST_LIBS=-lgtest -lgmock -lpthread

CXXFLAGS=$(ABI_FLAGS) $(DBG_FLAGS) $(LANG) $(IDIR) $(LIBS)
LDFLAGS= $(DBG_FLAGS) -L$(LDIR) $(LIBS)

BUILD_SUB_FOLDERS=$(shell find ${SRC_DIR} -mindepth 1 -type d -printf "${BUILD_DIR}%P\n")
$(shell mkdir -p ${BUILD_SUB_FOLDERS})
BUILD_TEST_SUB_FOLDERS=$(shell find ${TEST_SRC_DIR} -mindepth 1 -type d -printf "${TEST_BUILD_DIR}%P\n")
$(shell mkdir -p ${BUILD_TEST_SUB_FOLDERS})

SOURCES=$(shell find ${SRC_DIR} -name "*.cpp" -printf "%p\n")
HEADERS=$(shell find ${SRC_DIR} -name "*.h" -printf "%p\n")
SOURCES_PATH=$(shell find ${SRC_DIR} -name "*.cpp" -printf "%P ")
OBJECTS=$(shell find ${SRC_DIR} -name "*.cpp" ! -name "${BINARY}.cpp" -printf "${BUILD_DIR}%P\n" | sed -e 's/.cpp$$/.o/g')
TEST_SOURCES=$(shell find ${TEST_SRC_DIR} -name "*.cpp" -printf "%p\n")
TEST_HEADERS=$(shell find ${TEST_SRC_DIR} -name "*.h" -printf "%p\n")
TEST_SOURCES_PATH=$(shell find ${TEST_SRC_DIR} -name "*.cpp" -printf "%P ")
TEST_OBJECTS=$(shell find ${TEST_SRC_DIR} -name "*.cpp" -printf "${TEST_BUILD_DIR}%P\n" | sed -e 's/.cpp$$/.o/g')

$(info $$SOURCES":" [${SOURCES}])
$(info $$HEADERS":" [${HEADERS}])
$(info $$OBJECTS":" [${OBJECTS}])
$(info $$TEST_OBJECTS":" [${TEST_OBJECTS}])

#@test "$(SOURCES)" != ""

all: ${OBJECTS} ${BINARY} ${TEST_BINARY} preload

${BINARY}: $(OBJECTS) $(HEADERS)
	${CXX} ${CXXFLAGS} ${SRC_DIR}${BINARY}.cpp -o ${BINARY} $(OBJECTS) $(LDFLAGS)

$(BUILD_DIR)%.o: $(SRC_DIR)%.cpp
	$(CXX) -c $(IDIR) -o $@ $< $(CXXFLAGS)
#$(OBJECTS): $(SOURCES)

# rebuild objects if headers changed
$(OBJECTS): $(HEADERS)

${TEST_BINARY}: $(TEST_OBJECTS) $(TEST_HEADERS)
	${CXX} $(GMOCK_DIR) $(GTEST_DIR) $(IDIR) ${CXXFLAGS} $(GTEST_LIB_DIR) -o ${TEST_BINARY} $(OBJECTS) $(TEST_OBJECTS) $(TEST_LIBS)

$(TEST_BUILD_DIR)%.o: $(TEST_SRC_DIR)%.cpp
	$(CXX) -c $(GMOCK_DIR) $(GTEST_DIR) $(IDIR) $(CXXFLAGS) $(SOCKPL_DIR) -o $@ $<

$(TEST_OBJECTS): $(HEADERS) $(TEST_HEADERS)

preload:
	${CXX} -shared -fPIC -O0 -g ${LANG} ${IDIR} -o libsockpl.so so/sockpl.cpp $(OBJECTS) -lc -ldl -pthread

clean:
	rm -f ${BINARY} ${TEST_BINARY} ${TEST_OBJECTS} $(OBJECTS) libsockpl.so


