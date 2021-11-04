# Handles prebuild tasks:
# 1. Creating dependency files
# 2. Adding all needed sources to the ALL_SRC file

MY_PATH := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))

include $(abspath $(MY_PATH)/common.mk)

PRE := $(sort $(MAKECMDGOALS))
NOT_PRE := $(call SELECT_OUT,$(PAT_PRE),$(PRE))
ifneq (,$(NOT_PRE))
$(error "Invalid target to prebuild. All should match the pattern $(PAT_PRE), but these do not:$(NEWLINE)$(NOT_PRE)")
endif

SRC := $(call FROM_PAT,$(PAT_PRE),$(PRE))

include $(abspath $(MY_PATH)/validate_src.mk)

DEP := $(call TO_PAT,$(PAT_DEP),$(SRC))
DEP_TO_SRC = $(call FROM_PAT,$(PAT_DEP),$@)
DEP_TO_OBJ = $(call TO_PAT,$(PAT_OBJ),$(DEP_TO_SRC))
ALWAYS_FORCE_REBUILD_PAT := ALWAYS_FORCE_REBUILD

ALWAYS_FORCE_REBUILD := 
.PHONY: $(PRE)

# Some of these may have been created in the past, carrying additional dependencies.
include $(wildcard $(DEP))

# default goal (if no sources specified)
all: .FORCE
	@:

.SECONDEXPANSION:

# Each dep file depends on the corresponding source file
# 1. precompile dependencies for the .o
# 2. precompile dependencies for this dependencies file
$(DEP): %: $$(call FROM_PAT,$$(PAT_DEP),%)
	$(call SECTION_START,"creating dependency $@")
	$(RUN_VERBOSE_PRINT) $(CC) -MM -MQ "$(DEP_TO_OBJ)" $(FLAGS) $(DEP_TO_SRC) > $@
	@$(CC) -MM -MQ $@ $(FLAGS) $(DEP_TO_SRC) 2>/dev/null >> $@
	@if grep "$(ALWAYS_FORCE_REBUILD_PAT)" "$(DEP_TO_SRC)"; then echo "$(DEP_TO_OBJ): .FORCE # due to use of $(ALWAYS_FORCE_REBUILD_PAT)" >> $@; fi
	$(call SECTION_END,"creating dependency $@")

# Each PRE target depends on the dependency being created,
# then calls add_src with this source file to add this source to the ALL_SRC file
# and look for additional dependencies
$(PRE): %: $$(call TO_PAT,$$(PAT_DEP),$$(call FROM_PAT,$$(PAT_PRE),%)) .FORCE
	@$(MAKE) -f $(abspath $(MY_PATH)/add_src.mk) $(call FROM_PAT,$(PAT_PRE),$@)

