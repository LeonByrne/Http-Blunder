export OBJ_DEST = $(shell pwd)/obj

SOURCES := $(shell find $(OBJ_DEST) -name '*.o')
# target macros
TARGET := bin/test # FILL: target filename
MAIN_SRC := src/main.cpp # FILL: src file which contains `main()`

# compile macros
DIRS := src #Fort rendering # FILL: only the dirs which contain the src files, in this case, `src` should be added
OBJS := #main.o #Window.o # FILL: only the objects in current dir, and do not include the one contains `main()`

# intermedia compile macros
# NOTE: ALL_OBJS are intentionally left blank, no need to fill it
ALL_OBJS := $(OBJS)
CLEAN_FILES := $(TARGET) $(OBJS)
DIST_CLEAN_FILES := $(OBJS)

# recursive wildcard
rwildcard=$(foreach d,$(wildcard $(addsuffix *,$(1))),$(call rwildcard,$(d)/,$(2))$(filter $(subst *,%,$(2)),$(d)))

# default target
default: show-info all

# non-phony targets
$(TARGET): build-subdirs $(OBJS) find-all-objs 
	# echo $(SOURCES)
	@echo -e "\t" CXX $(CXXFLAGS) $(SOURCES) -o $@
	@$(CXX) -o $@ $(SOURCES) $(CXXFLAGS)
	# @rm -f $(ALL_OBJS)
	# @mv $(ALL_OBJS) obj

# phony targets
.PHONY: all
all: $(TARGET)
	@echo Target $(TARGET) build finished.

.PHONY: clean
clean: clean-subdirs
	@echo CLEAN $(CLEAN_FILES)
	@rm -f $(CLEAN_FILES)

.PHONY: distclean
distclean: clean-subdirs
	@echo CLEAN $(DIST_CLEAN_FILES)
	@rm -f $(DIST_CLEAN_FILES)

# phony funcs
.PHONY: find-all-objs
find-all-objs:
	$(eval ALL_OBJS += $(call rwildcard,$(DIRS),*.o))

.PHONY: show-info
show-info:
	@echo Building Project

# need to be placed at the end of the file
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
export PROJECT_PATH := $(patsubst %/,%,$(dir $(mkfile_path)))
export MAKE_INCLUDE=$(PROJECT_PATH)/config/make.global
export SUB_MAKE_INCLUDE=$(PROJECT_PATH)/config/submake.global
include $(MAKE_INCLUDE)