MY_PATH := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))

include $(abspath $(MY_PATH)/common.mk)

# SRC should use full paths
NON_ABS_SRC := $(filter-out $(abspath $(SRC)),$(SRC))
ifneq (,$(NON_ABS_SRC))
$(error "Invalid source files. All should have full paths, but these do not:$(NEWLINE)$(NON_ABS_SRC)")
endif

# SRC should match the source pattern
NON_PAT_SRC := $(call SELECT_OUT,$(PAT_SRC),$(SRC))
ifneq (,$(NON_PAT_SRC))
$(error "Invalid source files. All should match the pattern $(PAT_SRC), but these do not:$(NEWLINE)$(NON_PAT_SRC)")
endif

# SRC files should all exist
MISSING_SRC := $(call NOT_EXIST,$(SRC))
ifneq (,$(MISSING_SRC))
$(error "Missing source files:$(NEWLINE)$(MISSING_SRC)")
endif
