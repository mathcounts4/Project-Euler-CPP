# Accomplishes two goals.
# This checks the ALL_SRC file to see if each source has been processed.
# If this already has, then ignore it.
# For processing:
# 1. Adds this source file to the ALL_SRC file
# 2. If this source file contains LINK_PAT, add that to the EXTRA_LNK file
# 3. Searches for additional source files based on the corresponding dependencies file, calling prebuild on them

MY_PATH := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))

include $(abspath $(MY_PATH)/common.mk)

SRC := $(MAKECMDGOALS)

ifneq (1,$(words $(SRC)))
$(error "add_src should only be called with one source at a time")
endif

include $(abspath $(MY_PATH)/validate_src.mk)

DEP := $(call TO_PAT,$(PAT_DEP),$(SRC))
MISSING_DEP := $(call NOT_EXIST,$(DEP))
ifneq (,$(MISSING_DEP))
$(error "Dependency file missing when calling add_src on $(SRC): $(MISSING_DEP)")
endif

LINK_PAT := REQUIRES_LINK
NEEDED_LINK := $(shell cat $(SRC) | egrep -o '$(LINK_PAT) .*' | sed -E 's/$(LINK_PAT) //')
NEEDED_LINK := $(abspath $(addprefix $(dir $(SRC)),$(NEEDED_LINK)))
MISSING_LINK := $(call NOT_EXIST,$(NEEDED_LINK))
ifneq (,$(MISSING_LINK))
$(error "Link file(s) missing when calling add_src on $(SRC):$(NEWLINE)$(MISSING_LINK)")
endif

ifeq (,$(NEEDED_LINK))
ADD_LINK_CMD := :
else
ADD_LINK_CMD := echo "\# Added from $(SRC):\nLNK += $(NEEDED_LINK)" >> "$(EXTRA_LNK)"
endif

CONTENT := $(shell cat $(DEP) <<< '')
DEP_HDR := $(call SELECT,$(PAT_HDR),$(CONTENT))
POS_SRC := $(sort $(abspath $(call HDR_TO_SRC,$(DEP_HDR))))
FND_SRC := $(filter-out $(abspath $(SRC)),$(wildcard $(POS_SRC)))
FND_PRE := $(call TO_PAT,$(PAT_PRE),$(FND_SRC))
.PHONY: $(SRC)

$(SRC): .FORCE
	@grep "$@" "$(ALL_SRC)" > /dev/null 2>&1 \
	|| ($(IF_VERBOSE) echo "$(STARS) Including source file $@ $(STARS)" \
	&& echo "SRC += $@" >> "$(ALL_SRC)" \
	&& $(ADD_LINK_CMD) \
	&& $(MAKE) -f $(abspath $(MY_PATH)/prebuild.mk) $(FND_PRE))


