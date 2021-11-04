include ../common.mk

MAKECMDGOALS := $(filter-out all,$(MAKECMDGOALS))
.PHONY: all $(MAKECMDGOALS)

# For each goal that looks like a file (contains .), replace with its full path
GOALS := $(foreach goal,$(MAKECMDGOALS),$(if $(findstring .,$(goal)),$(abspath $(goal)),$(goal)))

all: .FORCE
	@cd .. && $(MAKE) $(GOALS)

ifneq (,$(MAKECMDGOALS))
$(MAKECMDGOALS): all
	@:
endif
