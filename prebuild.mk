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
DEP_TO_DEBUG = $(call TO_PAT,$(PAT_DEBUG),$(DEP_TO_OBJ))
ALWAYS_FORCE_REBUILD_PAT := ALWAYS_FORCE_REBUILD
FORCE_REBUILD_DEBUG_CHANGED_PAT := FORCE_REBUILD_DEBUG_CHANGED

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
	@if grep "$(ALWAYS_FORCE_REBUILD_PAT)" "$(DEP_TO_SRC)"  2>&1 > /dev/null; then echo "$(DEP_TO_OBJ): .FORCE # due to use of $(ALWAYS_FORCE_REBUILD_PAT)" >> $@; fi
	@if grep "$(FORCE_REBUILD_DEBUG_CHANGED_PAT)" "$(DEP_TO_SRC)"  2>&1 > /dev/null; then echo "# due to use of $(FORCE_REBUILD_DEBUG_CHANGED_PAT):\n$(DEP_TO_OBJ): $(DEP_TO_DEBUG)\nifeq (\$$(DEBUG),\$$(shell cat \"$(DEP_TO_DEBUG)\" 2>/dev/null))\n$(DEP_TO_DEBUG): \n\t@:\nelse\n$(DEP_TO_DEBUG): .FORCE\n\t\$$(RUN_VERBOSE_PRINT) echo \"\$$(DEBUG)\" > \"\$$@\"\nendif" >> $@; fi
	$(call SECTION_END,"creating dependency $@")

# Each PRE target depends on the dependency being created,
# then calls add_src with this source file to add this source to the ALL_SRC file
# and look for additional dependencies
$(PRE): %: $$(call TO_PAT,$$(PAT_DEP),$$(call FROM_PAT,$$(PAT_PRE),%)) .FORCE
	@$(MAKE) -f $(abspath $(MY_PATH)/add_src.mk) $(call FROM_PAT,$(PAT_PRE),$@)

