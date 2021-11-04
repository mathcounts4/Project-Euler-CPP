MY_PATH := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
PARENT := $(abspath $(MY_PATH)/../makefile)

include $(abspath $(MY_PATH)/../common.mk)

MAKECMDGOALS := $(filter-out all,$(MAKECMDGOALS))
.PHONY: all $(MAKECMDGOALS)

# For each goal that looks like a file (contains .), replace with its full path
GOALS := $(foreach goal,$(MAKECMDGOALS),$(if $(findstring .,$(goal)),$(abspath $(goal)),$(goal)))

all: .FORCE
	@$(MAKE) -f $(PARENT) $(GOALS)

ifneq (,$(MAKECMDGOALS))
$(MAKECMDGOALS): all
	@:
endif
