MY_PATH := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))

include $(abspath $(MY_PATH)/common.mk)

BUILD_TARGETS  := all run r exe e recompile rc relink rl rebuild rb compile c link l build b
HEADER_FILE_TARGETS := %.h %.hpp
HEADER_TARGETS := headers h hpp $(HEADER_FILE_TARGETS)
HDR_GOALS      := $(sort $(filter $(HEADER_TARGETS),$(MAKECMDGOALS)))
HDR_FILE_GOALS := $(sort $(filter $(HEADER_FILE_TARGETS),$(HDR_GOALS)))
PWD := $(shell pwd)
ALL_H := $(wildcard $(PWD)/*.h)
ALL_HPP := $(wildcard $(PWD)/*.hpp)
PHONY_TARGETS  := $(BUILD_TARGETS) $(HDR_GOALS)
TARGETS        := $(EXE) $(PHONY_TARGETS)
UNKNOWN        := $(filter-out $(TARGETS),$(MAKECMDGOALS))

ifneq (,$(UNKNOWN))
$(error "Unknown file type(s) or goal(s):$(NEWLINE)$(UNKNOWN)")
endif

.PHONY: $(PHONY_TARGETS)

# To call this makefile, the sources file must have been written to
ifeq (,$(wildcard $(SRCS_FL)))
$(error "build.mk called without $(SRCS_FL) existing")
endif
ifeq (,$(wildcard $(ALL_SRC)))
$(error "build.mk called without $(ALL_SRC) existing")
endif

ifeq (,$(HDR_GOALS))
# normal build

include $(SRCS_FL)
include $(ALL_SRC)
include $(EXTRA_LNK)

else

# Header targets: explicitly compile the header(s) to check for correct #includes
ifneq ($(HDR_GOALS),$(MAKECMDGOALS))
$(error No other goals can be specified with header targets)
endif
h: $(ALL_H) .FORCE
	@:
hpp: $(ALL_HPP) .FORCE
	@:
headers: h hpp
	@:
#$(PAT_HDR): .FORCE
#	@echo Unknown header file $@
#	@false
$(sort $(ALL_H) $(ALL_HPP) $(HDR_FILE_GOALS)): .FORCE
	$(RUN_VERBOSE_PRINT) $(CC) -c $(FLAGS) -fsyntax-only -o /dev/null $@

endif

SRC := $(sort $(SRC))
LNK := $(sort $(LNK))

# Default target:
all run r exe e: $(EXE) .FORCE
	$(call SECTION_START,"running $(EXE)")
	$(RUN_VERBOSE_PRINT) $(EXE)
	$(call SECTION_END,"running $(EXE)")

ifneq (,$(SRC))
$(SRC): %:
	@echo Missing source file $@
	@false
endif

ifneq (,$(LNK))
$(LNK): %:
	@echo Missing user-provided link file $@
	@false
endif

# Now that sources are fixed, define object and dependency files
OBJ            := $(call TO_PAT,$(PAT_OBJ),$(SRC))
DEP            := $(call TO_PAT,$(PAT_DEP),$(SRC))

MISSING_DEP    := $(call NOT_EXIST,$(DEP))
ifneq (,$(MISSING_DEP))
$(error "build.mk called, but missing dependency file(s):$(NEWLINE)$(MISSING_DEP)")
endif

include $(DEP)

.SECONDEXPANSION:

OBJ_TO_SRC = $(call FROM_PAT,$(PAT_OBJ),$@)
$(OBJ): %: $$(call FROM_PAT,$$(PAT_OBJ),%)
	$(call SECTION_START,"compiling $(OBJ_TO_SRC)")
	$(RUN_IF_NOT_VERBOSE) echo "Compiling $(OBJ_TO_SRC)"
	$(RUN_VERBOSE_PRINT) $(CC) -c $(FLAGS) -o $@ $(OBJ_TO_SRC)
	$(call SECTION_END,"compiling $(OBJ_TO_SRC)")
compile c: $(OBJ)
	@:
recompile rc: .FORCE
	@touch $(SRC)
	@$(MAKE) -f $(abspath $(MY_PATH)/build.mk) compile

link l build b: $(EXE)
	@:
$(EXE): $(OBJ) $(LNK)
	$(call SECTION_START,"linking $(EXE)")
	$(RUN_IF_NOT_VERBOSE) echo "Linking $(EXE)"
	$(RUN_VERBOSE_PRINT) $(CC) $(FLAGS) $(LNK_FLG) -o $(EXE) $(OBJ) $(LNK)
	$(call SECTION_END,"linking $(EXE)")
relink rl rebuild rb: .FORCE
	@touch $(OBJ) $(LNK)
	@$(MAKE) -f $(abspath $(MY_PATH)/build.mk) link

