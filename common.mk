ifeq (,$(COMMON_MK))
COMMON_MK := true

MY_PATH := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))

# .FORCE is a sepcial target
# Forces any target with .FORCE as a dependency to always be rebuilt
.FORCE:

# Causes targets to be deleted if the recipe fails
.DELETE_ON_ERROR:



# Handling "debug"
ifneq (,$(filter debug,$(MAKECMDGOALS)))
MAKECMDGOALS := $(filter-out debug,$(MAKECMDGOALS))
.PHONY: debug
DEBUG := 1
debug: .FORCE
	@:
ifeq (,$(MAKECMDGOALS))
MAKECMDGOALS := all
debug: all
endif
endif



# Handling "verbose"
ifneq (,$(filter verbose,$(MAKECMDGOALS)))
MAKECMDGOALS := $(filter-out verbose,$(MAKECMDGOALS))
.PHONY: verbose
VERBOSE := verbose
verbose: .FORCE
	@:
ifeq (,$(MAKECMDGOALS))
MAKECMDGOALS := all
verbose: all
endif
endif

# Use these commands to control behavior depending on verbose
# $(VERBOSE_ECHO) stuff to echo if verbose
# $(RUN_VERBOSE_PRINT) stuff to always run, and print this line if verbose
# $(RUN_IF_VERBOSE) stuff to run only if verbose (and prints)
# $(RUN_IF_NOT_VERBOSE) stuff to run only if NOT verbose (never prints)
# $(IF_VERBOSE) stuff to run only if verbose (doesn't prevent printing)
ifneq (,$(VERBOSE))
VERBOSE_ECHO       := @echo
RUN_VERBOSE_PRINT  :=
RUN_IF_VERBOSE     :=
RUN_IF_NOT_VERBOSE := @:
IF_VERBOSE         :=
else
VERBOSE_ECHO       := @:
RUN_VERBOSE_PRINT  := @
RUN_IF_VERBOSE     := @:
RUN_IF_NOT_VERBOSE := @
IF_VERBOSE         := :
endif

# Exported variables
export ERROR_LIMIT
export VERBOSE
export DEBUG

# Common variables
ifeq (,$(ERROR_LIMIT))
ERROR_LIMIT := 5
endif

ifeq (,$(DEBUG))
DEBUG := 0
endif

ifeq (0,$(DEBUG))
BUILD_T := -O3 # -ffast-math
else
BUILD_T := -O0 -g -DDEBUG
endif

CC      := clang++
VER     := -std=c++20 -stdlib=libc++
# -Wall -Wextra -Werror -Wpedantic -pedantic-errors
# These are all included in -Weverything
GOOD_WARN := -Wshadow-field-in-constructor -Wshorten-64-to-32 -Wsign-conversion -Wmissing-noreturn -Wconversion -Wsign-conversion -Wheader-hygiene -Wcomma -Wshadow -Wrange-loop-analysis -Wdeprecated -Wundefined-func-template -Wredundant-parens -Wmissing-braces -Wunusable-partial-specialization -Wunused-variable -Wcomma -Wunused-member-function -Wunused-value -Wreturn-type -Wparentheses -Wmissing-variable-declarations -Wimplicit-int-conversion -Wreorder -Wmismatched-tags -Wunreachable-code -Wcovered-switch-default
BAD_WARN  := -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-exit-time-destructors -Wno-global-constructors -Wno-padded -Wno-self-assign-overloaded -Wno-unused-lambda-capture -Wno-float-conversion -Wno-float-equal -Wno-poison-system-directories -Wno-switch-enum -Wno-weak-vtables -Wno-switch-default -Wno-ordered-compare-function-pointers
OPTS    := -ferror-limit=$(ERROR_LIMIT) -ftemplate-backtrace-limit=0 -Weverything -Werror $(BAD_WARN)
FLAGS   := $(VER) $(BUILD_T) $(OPTS)
LNK_FLG := # .cpp files can specify REQUIRES_LINK_FLAG to add libraries to this
EXE     := $(abspath $(MY_PATH)/run.me)
STARS   := **************************
SRCS_FL := $(abspath $(MY_PATH)/.mk_src)
ALL_SRC := $(abspath $(MY_PATH)/.all_mk_src)
EXTRA_LNK := $(abspath $(MY_PATH)/.extra_link)

# new line character for make printing (in $(info ...) or $(error ...))
define NEWLINE


endef



# Patterns
PAT_PRE := %.prebuild
PAT_SRC := %.c %.cpp
PAT_HDR := %.h %.hpp
PAT_LNK := %.o %.a %.dylib %.tbd
PAT_DEP := .%.d
PAT_OBJ := .%.o
PAT_CLN := %.clean
PAT_DEBUG := %.debug




# Utility functions
PAT_OR_EMPTY = $(if $(filter $1,$(notdir $2)),$2,)
NOT_PAT_OR_EMPTY = $(if $(filter-out $1,$(notdir $2)),$2,)

# Conversion functions
FROM_PAT   = $(foreach file,$2,$(dir $(file))$(patsubst $1,%,$(notdir $(file))))
TO_PAT     = $(foreach file,$2,$(dir $(file))$(patsubst %,$1,$(notdir $(file))))
HDR_TO_SRC = $(patsubst %.h,%.c,$(patsubst %.hpp,%.cpp,$1))

# Existence functions
EXIST      = $(wildcard $1)
NOT_EXIST  = $(filter-out $(call EXIST,$1),$1)

# Selection functions
SELECT     = $(strip $(foreach file,$2,$(call PAT_OR_EMPTY,$1,$(file))))
SELECT_OUT = $(strip $(foreach file,$2,$(call NOT_PAT_OR_EMPTY,$1,$(file))))

# Section printing functions
SECTION_START = $(VERBOSE_ECHO) "\n$(STARS) .... $1 $(STARS)"
SECTION_END   = $(VERBOSE_ECHO) "$(STARS) Done $1 $(STARS)\n"



# Handling "clean"
ifneq (,$(filter clean,$(MAKECMDGOALS)))
ifneq (clean,$(MAKECMDGOALS))
$(error No other goals can be specified with "clean")
endif
MAKECMDGOALS :=
SPECIAL_GOAL := true
.PHONY: clean

SRC :=
ifneq (,$(call EXIST,$(ALL_SRC)))
include $(ALL_SRC)
endif
include $(abspath $(MY_PATH)/validate_src.mk)
SRC := $(sort $(SRC))
OBJ := $(call TO_PAT,$(PAT_OBJ),$(SRC))
DEP := $(call TO_PAT,$(PAT_DEP),$(SRC))
SRC := # ensure we don't accidentally delete the source files

FILES_TO_CLEAN := $(call EXIST,$(DEP) $(OBJ) $(EXE) $(ALL_SRC) $(EXTRA_LNK))
GOALS_TO_CLEAN := $(call TO_PAT,$(PAT_CLN),$(FILES_TO_CLEAN))
.PHONY: $(GOALS_TO_CLEAN)

.SECONDEXPANSION:
$(GOALS_TO_CLEAN) : %: .FORCE
	$(RUN_VERBOSE_PRINT) rm -f $(call FROM_PAT,$(PAT_CLN),$@)
clean: .FORCE $(GOALS_TO_CLEAN)
ifeq (,$(GOALS_TO_CLEAN))
	$(VERBOSE_ECHO) "nothing to clean" $(FILES_TO_CLEAN)
endif
	$(RUN_IF_NOT_VERBOSE) clear
	$(RUN_IF_NOT_VERBOSE) clear
endif



endif
