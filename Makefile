# libSolace build file
# Please note currently project use cmake as a build system and
# this file is just a convenience wrapper for common tasks.
PROJECT = solace

BUILD_DIR = build
INCLUDE_DIR = include
SRC_DIR = src
TEST_DIR = test

MODULE_HEADERS = ${INCLUDE_DIR}/*
MODULE_SRC = ${SRC_DIR}/*

GENERATED_MAKE = ${BUILD_DIR}/Makefile

LIBNAME = libsolace.a
LIB_TAGRET = ${BUILD_DIR}/${LIBNAME}

TESTNAME = test_solace
TEST_TAGRET = $(BUILD_DIR)/$(TEST_DIR)/$(TESTNAME)

DOC_DIR = doc
DOC_TARGET = $(DOC_DIR)/html


# First tagret that starts not with '.'' - is a default target to run
.PHONY: codecheck verify clean

all: lib


#-------------------------------------------------------------------------------
# CMake wrapper
#-------------------------------------------------------------------------------
${BUILD_DIR}:
	mkdir -p ${BUILD_DIR}

${GENERATED_MAKE}: ${BUILD_DIR}
	cd ${BUILD_DIR} && cmake ..


#-------------------------------------------------------------------------------
# Build lib
#-------------------------------------------------------------------------------
$(LIB_TAGRET): ${GENERATED_MAKE}
	$(MAKE) -C ${BUILD_DIR} solace

lib: $(LIB_TAGRET)

#-------------------------------------------------------------------------------
# Build unit tests
#-------------------------------------------------------------------------------
$(TEST_TAGRET): ${GENERATED_MAKE}
	$(MAKE) -C ${BUILD_DIR} $(TESTNAME)

test: $(LIB_TAGRET) $(TEST_TAGRET)
	cd ${BUILD_DIR} && ./${TEST_DIR}/$(TESTNAME)

#-------------------------------------------------------------------------------
# Build docxygen documentation
#-------------------------------------------------------------------------------
$(DOC_TARGET):
	doxygen doc/Doxyfile

# Build project doxygen docs doxyfile.inc 
doc: $(MODULE_HEADERS) $(MODULE_SRC) $(DOC_TARGET)


#-------------------------------------------------------------------------------
# Code quality assurance
#-------------------------------------------------------------------------------
libs/cppcheck:
	#./libs/dependencies
	git clone --depth 3 https://github.com/danmar/cppcheck.git libs/cppcheck

libs/cppcheck/cppcheck: libs/cppcheck
	$(MAKE) -C libs/cppcheck cppcheck


cpplint: $(MODULE_HEADERS) $(MODULE_SRC)
	cpplint --recursive --exclude=test/ci/* include/ src/ test/

#	--enable=style,unusedFunctions,exceptNew,exceptRealloc,possibleError 
#	cppcheck --std=c++11 --enable=all -v -I $(MODULE_HEADERS) $(MODULE_SRC) 
cppcheck: $(MODULE_HEADERS) $(MODULE_SRC) libs/cppcheck/cppcheck
	#--inconclusive
	libs/cppcheck/cppcheck --std=c++11 --std=posix -D __linux__ --inline-suppr -q --error-exitcode=2 \
	--enable=warning,performance,portability,missingInclude,information,unusedFunction \
	-I include -i test/ci src test examples


codecheck: cpplint cppcheck

#-------------------------------------------------------------------------------
# Code quality assurance
#-------------------------------------------------------------------------------

verify: $(TEST_TAGRET)
	# > 3.10 (not avaliable on trusty) --expensive-definedness-checks=yes
	valgrind --trace-children=yes --track-fds=yes --read-var-info=yes --redzone-size=128 --error-exitcode=4 \
	--tool=exp-sgcheck $(TEST_TAGRET)

	valgrind --trace-children=yes --track-fds=yes --read-var-info=yes --redzone-size=128 --error-exitcode=3 \
	--tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --partial-loads-ok=no \
	$(TEST_TAGRET)

#-------------------------------------------------------------------------------
# Insatall
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Cleanup
#-------------------------------------------------------------------------------

# Phony targets

clean:
	$(RM) -rf $(DOC_TARGET) $(BUILD_DIR)
