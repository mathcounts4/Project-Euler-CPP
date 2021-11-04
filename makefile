MY_PATH := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))

include $(abspath $(MY_PATH)/common.mk)

MAKECMDGOALS := $(filter-out all,$(MAKECMDGOALS))
.PHONY: all $(MAKECMDGOALS)

# Determine user-specified input files
ROOT_SRC  := $(abspath $(call SELECT,$(PAT_SRC),$(MAKECMDGOALS)))
LNK  := $(abspath $(call SELECT,$(PAT_LNK),$(MAKECMDGOALS)))
USR  := $(strip $(ROOT_SRC) $(LNK))
REST := $(filter-out $(PAT_SRC) $(PAT_LNK),$(MAKECMDGOALS))
REST ?= all

MISSING_SRC := $(filter-out $(wildcard $(ROOT_SRC)),$(ROOT_SRC))
MISSING_LNK := $(filter-out $(wildcard $(LNK)),$(LNK))

ifneq (,$(MISSING_SRC))
$(error "Missing specified source file(s):$(NEWLINE)$(MISSING_SRC)")
endif

ifneq (,$(MISSING_LNK))
$(error "Missing specified link file(s):$(NEWLINE)$(MISSING_LNK)")
endif

# Check for user-specified source/link files
ifneq (,$(USR))

# If there are user-specified input files,
# reset the sources file and add the link files to it
$(shell rm $(EXE) >/dev/null 2>&1)
$(shell echo "ROOT_SRC := $(ROOT_SRC)" > "$(SRCS_FL)")
$(shell echo "LNK := $(LNK)" >> "$(SRCS_FL)")
$(shell rm $(EXTRA_LNK) >/dev/null 2>&1)

else

# If none, import the sources file
ifeq (,$(wildcard $(SRCS_FL)))
ifeq (,$(SPECIAL_GOAL))
$(error "No input files given and previous history ($(SRCS_FL)) not found")
endif
else
include $(SRCS_FL)
endif

endif

# Default target:
all: .FORCE
	@rm -f $(ALL_SRC) 2>&1
	@touch $(EXTRA_LNK)
	@$(MAKE) -R -f $(abspath $(MY_PATH)/prebuild.mk) $(call TO_PAT,$(PAT_PRE),$(ROOT_SRC))
	@$(MAKE) -R -f $(abspath $(MY_PATH)/build.mk) $(REST)

ifneq (,$(MAKECMDGOALS))
$(MAKECMDGOALS): all
	@:
endif
