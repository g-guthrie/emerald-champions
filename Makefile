GAME_VERSION ?= EMERALD
TITLE        ?= EM CHAMPIONS
GAME_CODE    ?= BPEE
BUILD_NAME   ?= emerald
MAP_VERSION  ?= emerald

ifeq (firered, $(or $(BUILD), $(MAKECMDGOALS)))
  	GAME_VERSION 	:= FIRERED
	TITLE       	:= POKEMON FIRE
	GAME_CODE   	:= BPRE
	BUILD_NAME  	:= firered
	MAP_VERSION 	:= firered
else
ifeq (leafgreen, $(or $(BUILD), $(MAKECMDGOALS)))
	GAME_VERSION 	:= LEAFGREEN
	TITLE       	:= POKEMON LEAF
	GAME_CODE   	:= BPGE
	BUILD_NAME  	:= leafgreen
	MAP_VERSION 	:= firered
endif
endif

# GBA rom header
MAKER_CODE  := 01
REVISION    := 0
KEEP_TEMPS  ?= 0

# `File name`.gba
FILE_NAME := poke$(BUILD_NAME)
BUILD_DIR := build

# Compares the ROM to a checksum of the original - only makes sense using when non-modern
COMPARE     ?= 0
# Executes the Test Runner System that checks that all mechanics work as expected
TEST         ?= 0
EC_HEADLESS_FIXTURES ?= 0
# Enables -fanalyzer C flag to analyze in depth potential UBs
ANALYZE      ?= 0
# Count unused warnings as errors. Used by RH-Hideout's repo
UNUSED_ERROR ?= 0
# Count deprecated warnings as errors. Used by RH-Hideout's repo
DEPRECATED_ERROR ?= 0
# Adds -Og and -g flags, which optimize the build for debugging and include debug info respectively
DEBUG        ?= 0
# Adds -flto flag, which increases link time but results in a more efficient binary (especially in audio processing)
LTO          ?= 0
# Makes an optimized build for release, also enabling NDEBUG macro and disabling other debugging features
# Enables LTO by default, but can be changed in the config.mk file
RELEASE      ?= 0

ifeq (compare,$(MAKECMDGOALS))
  COMPARE := 1
endif
ifeq (check,$(MAKECMDGOALS))
  TEST := 1
endif
ifeq (debug,$(MAKECMDGOALS))
  DEBUG := 1
endif
ifneq (,$(filter release tidyrelease,$(MAKECMDGOALS)))
  RELEASE := 1
endif

include config.mk

# Default make rule
all: rom

# Toolchain selection
TOOLCHAIN := $(DEVKITARM)
# don't use dkP's base_tools anymore
# because the redefinition of $(CC) conflicts
# with when we want to use $(CC) to preprocess files
# thus, manually create the variables for the bin
# files, or use arm-none-eabi binaries on the system
# if dkP is not installed on this system
ifneq (,$(TOOLCHAIN))
  ifneq ($(wildcard $(TOOLCHAIN)/bin),)
    export PATH := $(TOOLCHAIN)/bin:$(PATH)
  endif
endif

PREFIX := arm-none-eabi-
OBJCOPY := $(PREFIX)objcopy
OBJDUMP := $(PREFIX)objdump
NM := $(PREFIX)nm
AS := $(PREFIX)as
LD := $(PREFIX)ld

EXE :=
ifeq ($(OS),Windows_NT)
  EXE := .exe
endif

CPP := $(PREFIX)cpp

ifeq ($(RELEASE),1)
	FILE_NAME := $(FILE_NAME)-release
endif

ROM_NAME := $(FILE_NAME).gba
OBJ_DIR_NAME := $(BUILD_DIR)/$(BUILD_NAME)
OBJ_DIR_NAME_TEST := $(BUILD_DIR)/$(BUILD_NAME)-test
OBJ_DIR_NAME_DEBUG := $(BUILD_DIR)/$(BUILD_NAME)-debug
OBJ_DIR_NAME_RELEASE := $(BUILD_DIR)/$(BUILD_NAME)-release
ASSETS_DIR_NAME := $(BUILD_DIR)/assets

ELF_NAME := $(ROM_NAME:.gba=.elf)
MAP_NAME := $(ROM_NAME:.gba=.map)
TESTELF := $(ROM_NAME:.gba=-test.elf)
HEADLESSELF := $(ROM_NAME:.gba=-test-headless.elf)

# Pick our active variables
ROM := $(ROM_NAME)
ifeq ($(TESTELF),$(MAKECMDGOALS))
  TEST := 1
endif
ifeq ($(TEST), 0)
  OBJ_DIR := $(OBJ_DIR_NAME)
else
  OBJ_DIR := $(OBJ_DIR_NAME_TEST)
endif
ifeq ($(DEBUG),1)
  OBJ_DIR := $(OBJ_DIR_NAME_DEBUG)
endif
ifeq ($(RELEASE),1)
  OBJ_DIR := $(OBJ_DIR_NAME_RELEASE)
endif
ELF := $(ROM:.gba=.elf)
UNFILTERED_TESTELF := $(OBJ_DIR)/$(notdir $(TESTELF))
MAP := $(ROM:.gba=.map)
PROVENANCE := $(ROM:.gba=.provenance.json)
SYM := $(ROM:.gba=.sym)

# Commonly used directories
C_SUBDIR = src
ASM_SUBDIR = asm
DATA_SRC_SUBDIR = src/data
DATA_ASM_SUBDIR = data
MID_SUBDIR = sound/songs/midi
TEST_SUBDIR = test

C_BUILDDIR = $(OBJ_DIR)/$(C_SUBDIR)
ASM_BUILDDIR = $(OBJ_DIR)/$(ASM_SUBDIR)
DATA_ASM_BUILDDIR = $(OBJ_DIR)/$(DATA_ASM_SUBDIR)
MID_BUILDDIR = $(OBJ_DIR)/$(MID_SUBDIR)
TEST_BUILDDIR = $(OBJ_DIR)/$(TEST_SUBDIR)

SHELL := bash -o pipefail

# Set flags for tools
ASFLAGS := -mcpu=arm7tdmi -march=armv4t -meabi=5 --defsym MODERN=1 --defsym $(GAME_VERSION)=1

INCLUDE_DIRS := include
INCLUDE_CPP_ARGS := $(INCLUDE_DIRS:%=-iquote %)
INCLUDE_SCANINC_ARGS := $(INCLUDE_DIRS:%=-I %)

ifeq ($(DEBUG),1)
O_LEVEL ?= g
else
O_LEVEL ?= 2
endif
CPPFLAGS := $(INCLUDE_CPP_ARGS) -Wno-trigraphs -DMODERN=1 -DTESTING=$(TEST) -D$(GAME_VERSION) -std=gnu17
CPPFLAGS += -DEC_HEADLESS_FIXTURES=$(EC_HEADLESS_FIXTURES)
ifeq ($(RELEASE),1)
ifneq ($(EC_HEADLESS_FIXTURES),0)
$(error EC_HEADLESS_FIXTURES must remain disabled for release builds)
endif
override CPPFLAGS += -DRELEASE
ifeq ($(USE_LTO_ON_RELEASE),1)
LTO := 1
endif
endif
ARMCC := $(PREFIX)gcc
PATH_ARMCC := PATH="$(PATH)" $(ARMCC)
CC1 = $(shell $(PATH_ARMCC) --print-prog-name=cc1) -quiet

override CFLAGS += -mthumb -mthumb-interwork -O$(O_LEVEL) -mabi=apcs-gnu -mtune=arm7tdmi -march=armv4t -Wno-pointer-to-int-cast -std=gnu17 -Werror -Wall -Wno-strict-aliasing -Wno-attribute-alias -Woverride-init -Wnonnull -Wenum-conversion

ifneq ($(LTO),0)
  ifneq ($(TEST),1)
    override CFLAGS += -flto=auto -fno-fat-lto-objects -fno-asynchronous-unwind-tables -ffunction-sections -fdata-sections
  endif
endif

ifeq ($(ANALYZE),1)
  override CFLAGS += -fanalyzer
endif
# Only throw an error for unused elements if its RH-Hideout's repo
ifeq ($(UNUSED_ERROR),0)
  ifneq ($(GITHUB_REPOSITORY_OWNER),rh-hideout)
    override CFLAGS += -Wno-error=unused-variable -Wno-error=unused-const-variable -Wno-error=unused-parameter -Wno-error=unused-function -Wno-error=unused-but-set-parameter -Wno-error=unused-but-set-variable -Wno-error=unused-value -Wno-error=unused-local-typedefs
  endif
endif

ifeq ($(DEPRECATED_ERROR),0)
  ifneq ($(GITHUB_REPOSITORY_OWNER),rh-hideout)
    override CFLAGS += -Wno-error=deprecated-declarations
  endif
endif

LIBPATH = -L "$(dir $(shell $(PATH_ARMCC) -mthumb -print-file-name=libgcc.a))" -L "$(dir $(shell $(PATH_ARMCC) -mthumb -print-file-name=libnosys.a))" -L "$(dir $(shell $(PATH_ARMCC) -mthumb -print-file-name=libc.a))"
LIB = $(LIBPATH) -lc -lnosys -lgcc -L../../libagbsyscall -lagbsyscall
# Lazy toolchain probes must not be expanded as inherited shell environment.
unexport CC1 LIBPATH LIB
# Enable debug info if set
ifeq ($(DINFO),1)
  override CFLAGS += -g
else
  ifeq ($(DEBUG),1)
    override CFLAGS += -g
  endif
endif

ifeq ($(NOOPT),1)
override CFLAGS := $(filter-out -O1 -Og -O2,$(CFLAGS))
override CFLAGS += -O0
endif

# Variable filled out in other make files
AUTO_GEN_TARGETS :=
include make_tools.mk
# Tool executables
SMOLTM       := $(TOOLS_DIR)/compresSmol/compresSmolTilemap$(EXE)
SMOL         := $(TOOLS_DIR)/compresSmol/compresSmol$(EXE)
GFX          := $(TOOLS_DIR)/gbagfx/gbagfx$(EXE)
WAV2AGB      := $(TOOLS_DIR)/wav2agb/wav2agb$(EXE)
MID          := $(TOOLS_DIR)/mid2agb/mid2agb$(EXE)
SCANINC      := $(TOOLS_DIR)/scaninc/scaninc$(EXE)
PREPROC      := $(TOOLS_DIR)/preproc/preproc$(EXE)
FIX          := $(TOOLS_DIR)/gbafix/gbafix$(EXE)
MAPJSON      := $(TOOLS_DIR)/mapjson/mapjson$(EXE)
JSONPROC     := $(TOOLS_DIR)/jsonproc/jsonproc$(EXE)
TRAINERPROC  := $(TOOLS_DIR)/trainerproc/trainerproc$(EXE)
PATCHELF     := $(TOOLS_DIR)/patchelf/patchelf$(EXE)
ifeq ($(shell uname),Darwin)
    ROMTEST ?= $(shell command -v mgba-rom-test-mac 2>/dev/null || echo $(TOOLS_DIR)/mgba/mgba-rom-test-mac)
    ROMTESTHYDRA := $(shell command -v mgba-rom-test-hydra 2>/dev/null || echo $(TOOLS_DIR)/mgba-rom-test-hydra/mgba-rom-test-hydra)
else ifeq ($(shell uname),Linux)
    ROMTEST ?= $(shell command -v mgba-rom-test 2>/dev/null || echo $(TOOLS_DIR)/mgba/mgba-rom-test)
    ROMTESTHYDRA := $(shell command -v mgba-rom-test-hydra 2>/dev/null || echo $(TOOLS_DIR)/mgba-rom-test-hydra/mgba-rom-test-hydra)
else
    ROMTEST ?= $(TOOLS_DIR)/mgba/mgba-rom-test$(EXE)
    ROMTESTHYDRA := $(TOOLS_DIR)/mgba-rom-test-hydra/mgba-rom-test-hydra$(EXE)
endif

# Learnset helper is a Python script
LEARNSET_HELPERS_DIR := $(TOOLS_DIR)/learnset_helpers
LEARNSET_HELPERS_DATA_DIR := $(LEARNSET_HELPERS_DIR)/porymoves_files
LEARNSET_HELPERS_BUILD_DIR := $(LEARNSET_HELPERS_DIR)/build
ALL_LEARNABLES_JSON := $(DATA_SRC_SUBDIR)/pokemon/all_learnables.json
MOVE_ACCESS_REVIEW_JSON := data/emerald_champions/emerald_champions_move_access_review.json
PREPARATION_FORM_LEARNSETS_JSON := data/emerald_champions/emerald_champions_preparation_form_learnsets.json
EC_PREPARATION_LEARNSETS := $(DATA_SRC_SUBDIR)/pokemon/emerald_champions_preparation_learnsets.h
AUTO_GEN_TARGETS += $(EC_PREPARATION_LEARNSETS)
ALL_TUTORS_JSON := $(LEARNSET_HELPERS_BUILD_DIR)/all_tutors.json
ALL_TEACHING_TYPES_JSON := $(LEARNSET_HELPERS_BUILD_DIR)/all_teaching_types.json

# wild_encounters.h is generated by a Python script
WILD_ENCOUNTERS_TOOL_DIR := $(TOOLS_DIR)/wild_encounters
AUTO_GEN_TARGETS += $(DATA_SRC_SUBDIR)/wild_encounters.h

MISC_TOOL_DIR := $(TOOLS_DIR)/misc
AUTO_GEN_TARGETS +=  $(INCLUDE_DIRS)/constants/script_commands.h

$(DATA_SRC_SUBDIR)/wild_encounters.h: $(DATA_SRC_SUBDIR)/wild_encounters.json $(WILD_ENCOUNTERS_TOOL_DIR)/wild_encounters_to_header.py $(INCLUDE_DIRS)/config/overworld.h $(INCLUDE_DIRS)/config/dexnav.h
	python3 $(WILD_ENCOUNTERS_TOOL_DIR)/wild_encounters_to_header.py

$(INCLUDE_DIRS)/constants/script_commands.h: $(MISC_TOOL_DIR)/make_scr_cmd_constants.py $(DATA_ASM_SUBDIR)/script_cmd_table.inc
	python3  $(MISC_TOOL_DIR)/make_scr_cmd_constants.py

PERL := perl
SHA1 := $(shell { command -v sha1sum || command -v shasum; } 2>/dev/null) -c

MAKEFLAGS += --no-print-directory

# Clear the default suffixes
.SUFFIXES:
# Don't delete intermediate files
.SECONDARY:
# Delete files that weren't built properly
.DELETE_ON_ERROR:

RULES_NO_SCAN += libagbsyscall clean clean-assets tidy tidymodern tidycheck tidyrelease generated clean-generated clean-teachables clean-teachables_intermediates
.PHONY: all rom agbcc modern compare check patch-test-filter debug release
.PHONY: $(RULES_NO_SCAN)

infoshell = $(foreach line, $(shell $1 | sed "s/ /__SPACE__/g"), $(info $(subst __SPACE__, ,$(line))))

# Check if we need to scan dependencies based on the chosen rule OR user preference
NODEP ?= 0
# Check if we need to pre-build tools and generate assets based on the chosen rule.
SETUP_PREREQS ?= 1
# Disable dependency scanning for rules that don't need it.
ifneq (,$(MAKECMDGOALS))
  ifeq (,$(filter-out $(RULES_NO_SCAN),$(MAKECMDGOALS)))
    NODEP := 1
    SETUP_PREREQS := 0
  endif
endif

ifeq ($(SETUP_PREREQS),1)
  # If set on: Default target or a rule requiring a scan
  # Forcibly execute `make tools` since we need them for what we are doing.
  # GNU Make 3.81 (macOS) has no .SHELLSTATUS. With our pipefail shell,
  # append a success marker only after the entire prerequisite pipeline passes.
  TOOLS_SETUP_OUTPUT := $(shell $(MAKE) -f make_tools.mk | sed "s/ /__SPACE__/g" && printf '\n__EC_PREREQUISITES_OK__')
  $(foreach line, $(filter-out __EC_PREREQUISITES_OK__,$(TOOLS_SETUP_OUTPUT)), $(info $(subst __SPACE__, ,$(line))))
  ifneq ($(lastword $(TOOLS_SETUP_OUTPUT)),__EC_PREREQUISITES_OK__)
    $(error Errors occurred while building tools. See error messages above for more details)
  endif
  # Oh and also generate mapjson sources before we use `SCANINC`.
  GENERATED_SETUP_OUTPUT := $(shell $(MAKE) MAP_VERSION=$(MAP_VERSION) generated | sed "s/ /__SPACE__/g" && printf '\n__EC_PREREQUISITES_OK__')
  $(foreach line, $(filter-out __EC_PREREQUISITES_OK__,$(GENERATED_SETUP_OUTPUT)), $(info $(subst __SPACE__, ,$(line))))
  ifneq ($(lastword $(GENERATED_SETUP_OUTPUT)),__EC_PREREQUISITES_OK__)
    $(error Errors occurred while generating map-related sources. See error messages above for more details)
  endif
endif

# Collect sources
C_SRCS_IN := $(wildcard $(C_SUBDIR)/*.c $(C_SUBDIR)/*/*.c $(C_SUBDIR)/*/*/*.c)
C_SRCS := $(foreach src,$(C_SRCS_IN),$(if $(findstring .inc.c,$(src)),,$(src)))
C_OBJS := $(patsubst $(C_SUBDIR)/%.c,$(C_BUILDDIR)/%.o,$(C_SRCS))

# A release/PR job may provide a curated source allowlist.  The normal default
# selects the retained focused test corpus for manual builds.
TEST_SOURCE_ALLOWLIST ?=
TEST_SUPPORT_SRCS := $(TEST_SUBDIR)/test_runner.c $(TEST_SUBDIR)/test_runner_args.c $(TEST_SUBDIR)/test_runner_battle.c
ifeq (,$(strip $(TEST_SOURCE_ALLOWLIST)))
TEST_SRCS_IN := $(wildcard $(TEST_SUBDIR)/*.c $(TEST_SUBDIR)/*/*.c $(TEST_SUBDIR)/*/*/*.c)
else
TEST_SRCS_IN := $(sort $(TEST_SUPPORT_SRCS) $(TEST_SOURCE_ALLOWLIST))
endif
TEST_SRCS := $(foreach src,$(TEST_SRCS_IN),$(if $(findstring .inc.c,$(src)),,$(src)))
TEST_OBJS := $(patsubst $(TEST_SUBDIR)/%.c,$(TEST_BUILDDIR)/%.o,$(TEST_SRCS))
TEST_OBJS_REL := $(patsubst $(OBJ_DIR)/%,%,$(TEST_OBJS))

C_ASM_SRCS := $(wildcard $(C_SUBDIR)/*.s $(C_SUBDIR)/*/*.s $(C_SUBDIR)/*/*/*.s)
C_ASM_OBJS := $(patsubst $(C_SUBDIR)/%.s,$(C_BUILDDIR)/%.o,$(C_ASM_SRCS))

ASM_SRCS := $(wildcard $(ASM_SUBDIR)/*.s)
ASM_OBJS := $(patsubst $(ASM_SUBDIR)/%.s,$(ASM_BUILDDIR)/%.o,$(ASM_SRCS))

DATA_ASM_SRCS := $(wildcard $(DATA_ASM_SUBDIR)/*.s)
DATA_ASM_OBJS := $(patsubst $(DATA_ASM_SUBDIR)/%.s,$(DATA_ASM_BUILDDIR)/%.o,$(DATA_ASM_SRCS))

MID_SRCS := $(wildcard $(MID_SUBDIR)/*.mid)
MID_OBJS := $(patsubst $(MID_SUBDIR)/%.mid,$(MID_BUILDDIR)/%.o,$(MID_SRCS))

OBJS     := $(C_OBJS) $(C_ASM_OBJS) $(ASM_OBJS) $(DATA_ASM_OBJS) $(MID_OBJS)
OBJS_REL := $(patsubst $(OBJ_DIR)/%,%,$(OBJS))

SUBDIRS  := $(sort $(dir $(OBJS) $(dir $(TEST_OBJS))))
$(shell mkdir -p $(SUBDIRS))

# Pretend rules that are actually flags defer to `make all`
modern: all
compare: all
debug: all
release: all
# Uncomment the next line, and then comment the 4 lines after it to reenable agbcc.
#agbcc: all
agbcc:
	@echo "'make agbcc' is deprecated as of pokeemerald-expansion 1.9 and will be removed in 1.10."
	@echo "Search for 'agbcc: all' in Makefile to reenable agbcc."
	@exit 1

LD_SCRIPT_TEST := ld_script_test.ld

$(OBJ_DIR)/ld_script_test.ld: $(LD_SCRIPT_TEST)
	cd $(OBJ_DIR) && sed "s#tools/#../../tools/#g" ../../$(LD_SCRIPT_TEST) > ld_script_test.ld

$(UNFILTERED_TESTELF): $(OBJ_DIR)/ld_script_test.ld $(OBJS) $(TEST_OBJS) $(OBJ_DIR)/.test-link-config.json
	@echo "cd $(OBJ_DIR) && $(LD) -T ld_script_test.ld -o $(notdir $@) <objects> <test-objects> <lib>"
	@cd $(OBJ_DIR) && $(LD) $(TESTLDFLAGS) -T ld_script_test.ld -o $(notdir $@) $(OBJS_REL) $(TEST_OBJS_REL) $(LIB)
	$(FIX) $@ -t"$(TITLE)" -c$(GAME_CODE) -m$(MAKER_CODE) -r$(REVISION) -d0 --silent

$(TESTELF): $(UNFILTERED_TESTELF) FORCE_TEST_EXPORT | check-tools
	@python3 scripts/export_test_elf.py $(UNFILTERED_TESTELF) $@ --receipt=$(OBJ_DIR)/.test-filter.json --patchelf=$(call shell_quote,$(PATCHELF)) --filter=$(call shell_quote,$(TESTS:%*=%))

# Filter export validates actual bytes, so direct/check/direct requests stay
# coherent even when an earlier command patched the public test image.
patch-test-filter: $(TESTELF)
	@:

ifeq ($(GITHUB_REPOSITORY_OWNER),rh-hideout)
TEST_SKIP_IS_FAIL := \x01
else
TEST_SKIP_IS_FAIL := \x00
endif

check: patch-test-filter
	@cp $(TESTELF) $(HEADLESSELF)
	$(PATCHELF) $(HEADLESSELF) gTestRunnerHeadless '\x01' gTestRunnerSkipIsFail "$(TEST_SKIP_IS_FAIL)"
	$(ROMTESTHYDRA) $(ROMTEST) $(OBJCOPY) $(HEADLESSELF)

# Other rules
rom: $(ROM)
ifeq ($(COMPARE),1)
	@$(SHA1) rom.sha1
endif

syms: $(SYM)

clean: tidy clean-tools clean-check-tools clean-generated clean-assets
	@$(MAKE) clean -C libagbsyscall

clean-assets:
	rm -rf $(ASSETS_DIR_NAME)
	rm -f $(MID_SUBDIR)/*.s
	rm -f $(DATA_ASM_SUBDIR)/layouts/layouts.inc $(DATA_ASM_SUBDIR)/layouts/layouts_table.inc
	rm -f $(DATA_ASM_SUBDIR)/maps/connections.inc $(DATA_ASM_SUBDIR)/maps/events.inc $(DATA_ASM_SUBDIR)/maps/groups.inc $(DATA_ASM_SUBDIR)/maps/headers.inc $(DATA_SRC_SUBDIR)/map_group_count.h
	rm -f .map_version
	find sound -iname '*.bin' -exec rm {} +
	find . \( -iname '*.1bpp' -o -iname '*.4bpp' -o -iname '*.8bpp' -o -iname '*.gbapal' -o -iname '*.lz' -o -iname '*.smol' -o -iname '*.fastSmol' -o -iname '*.smolTM' -o -iname '*.rl' -o -iname '*.latfont' -o -iname '*.hwjpnfont' -o -iname '*.fwjpnfont' \) -exec rm {} +
	find $(DATA_ASM_SUBDIR)/maps \( -iname 'connections.inc' -o -iname 'events.inc' -o -iname 'header.inc' \) -exec rm {} +

tidy: tidymodern tidycheck tidydebug tidyrelease

tidymodern:
	rm -f poke*.gba poke*.elf poke*.map poke*.provenance.json
	rm -rf $(OBJ_DIR_NAME)

tidycheck:
	rm -f $(TESTELF) $(HEADLESSELF)
	rm -rf $(OBJ_DIR_NAME_TEST)

tidydebug:
	rm -rf $(OBJ_DIR_NAME_DEBUG)

tidyrelease:
ifeq ($(RELEASE),1)
	rm -f $(ROM_NAME) $(ELF_NAME) $(MAP_NAME) $(PROVENANCE)
else # Manually remove the release files on clean/tidy
	rm -f $(FILE_NAME)-release.gba $(FILE_NAME)-release.elf $(FILE_NAME)-release.map $(FILE_NAME)-release.provenance.json
endif
	rm -rf $(OBJ_DIR_NAME_RELEASE)

# Other rules
include graphics_file_rules.mk
include map_data_rules.mk
include spritesheet_rules.mk
include json_data_rules.mk
include audio_rules.mk
include trainer_rules.mk

# NOTE: Tools must have been built prior (FIXME)
# so you can't really call this rule directly
generated: $(AUTO_GEN_TARGETS)
	@: # Silence the "Nothing to be done for `generated'" message, which some people were confusing for an error.


%.s:   ;
%.png: ;
%.pal: ;
%.wav: ;

%.1bpp:     %.png  ; $(GFX) $< $@
%.4bpp:     %.png  ; $(GFX) $< $@
%.8bpp:     %.png  ; $(GFX) $< $@
%.gbapal:   %.pal  ; $(GFX) $< $@
%.gbapal:   %.png  ; $(GFX) $< $@
%.lz:       %      ; $(GFX) $< $@
%.smolTM:   %      ; $(SMOLTM) $< $@
%.fastSmol: %      ; $(SMOL) -w $< $@ false false false
%.smol:     %      ; $(SMOL) -w $< $@
%.rl:       %      ; $(GFX) $< $@

clean-teachables_intermediates:
	rm -f $(DATA_SRC_SUBDIR)/tutor_moves.h
	rm -f $(DATA_SRC_SUBDIR)/pokemon/teachable_learnsets.h
	@rm -Rf $(LEARNSET_HELPERS_BUILD_DIR)
	@echo "rm -Rf <LEARNSET_HELPERS_BUILD_DIR>"

clean-generated: clean-teachables_intermediates
	@rm -f $(AUTO_GEN_TARGETS)
	@echo "rm -f <AUTO_GEN_TARGETS>"

clean-teachables: clean-teachables_intermediates
	rm -f $(ALL_LEARNABLES_JSON)
	@touch $(C_SUBDIR)/pokemon.c

# Freeze global settings before any object can lend target-specific CFLAGS to
# the shared receipt. Per-target overrides and recipes live in compile_rules.mk.
CONFIG_CPPFLAGS := $(CPPFLAGS)
CONFIG_CFLAGS := $(CFLAGS)
CONFIG_ASFLAGS := $(ASFLAGS)
CONFIG_SCANFLAGS := $(INCLUDE_SCANINC_ARGS)
CONFIG_KEEP_TEMPS := $(KEEP_TEMPS)
CONFIG_CPP := $(CPP)
CONFIG_AS := $(AS)
CONFIG_ARMCC := $(ARMCC)
CONFIG_PREPROC := $(PREPROC)
CONFIG_SCANINC := $(SCANINC)
CONFIG_SCAN_TOOL = $(if $(filter 1,$(NODEP)),,--tool=$(call shell_quote,$(CONFIG_SCANINC)))
OBJECT_CONFIG := $(OBJ_DIR)/.objects-config.json
BUILD_CONFIG_HELPER := scripts/update_build_config.py
shell_quote = '$(subst ','"'"',$(1))'

.PHONY: FORCE_BUILD_CONFIG FORCE_TEST_EXPORT
FORCE_BUILD_CONFIG:
FORCE_TEST_EXPORT:

$(OBJECT_CONFIG): FORCE_BUILD_CONFIG
	@python3 $(BUILD_CONFIG_HELPER) $@ --value=$(call shell_quote,CPPFLAGS=$(CONFIG_CPPFLAGS)) --value=$(call shell_quote,CFLAGS=$(CONFIG_CFLAGS)) --value=$(call shell_quote,ASFLAGS=$(CONFIG_ASFLAGS)) --value=$(call shell_quote,SCANFLAGS=$(CONFIG_SCANFLAGS)) --value=$(call shell_quote,KEEP_TEMPS=$(CONFIG_KEEP_TEMPS)) --value=$(call shell_quote,SHELL=$(SHELL)) --tool=$(call shell_quote,$(CONFIG_ARMCC)) --tool=$(call shell_quote,$(CC1)) --tool=$(call shell_quote,$(CONFIG_CPP)) --tool=$(call shell_quote,$(CONFIG_AS)) --tool=$(call shell_quote,$(CONFIG_PREPROC)) $(CONFIG_SCAN_TOOL) --file=compile_rules.mk --file=map_data_rules.mk --file=audio_rules.mk --file=charmap.txt --env=CPATH --env=C_INCLUDE_PATH --env=COMPILER_PATH --env=GCC_EXEC_PREFIX --env=SOURCE_DATE_EPOCH

$(OBJS): $(OBJECT_CONFIG)
ifeq ($(TEST),1)
$(TEST_OBJS): $(OBJECT_CONFIG)
endif
ifneq ($(NODEP),1)
$(OBJS:.o=.d): $(OBJECT_CONFIG)
ifeq ($(TEST),1)
$(TEST_OBJS:.o=.d): $(OBJECT_CONFIG)
endif
endif

include compile_rules.mk

TEACHABLE_DEPS := $(ALL_LEARNABLES_JSON) $(MOVE_ACCESS_REVIEW_JSON) $(INCLUDE_DIRS)/constants/tms_hms.h $(INCLUDE_DIRS)/config/pokemon.h $(DATA_SRC_SUBDIR)/pokemon/special_movesets.json $(INCLUDE_DIRS)/config/pokedex_plus_hgss.h $(LEARNSET_HELPERS_DIR)/make_teachables.py

$(LEARNSET_HELPERS_BUILD_DIR):
	@mkdir -p $@

$(ALL_LEARNABLES_JSON):
	python3 $(LEARNSET_HELPERS_DIR)/make_learnables.py $(LEARNSET_HELPERS_DATA_DIR) $@

$(ALL_TUTORS_JSON): $(shell find data/ -type f -name '*.inc')  $(LEARNSET_HELPERS_DIR)/make_tutors.py | $(LEARNSET_HELPERS_BUILD_DIR)
	python3 $(LEARNSET_HELPERS_DIR)/make_tutors.py $@

$(ALL_TEACHING_TYPES_JSON): $(wildcard $(DATA_SRC_SUBDIR)/pokemon/species_info/*_families.h)  $(LEARNSET_HELPERS_DIR)/make_teaching_types.py | $(LEARNSET_HELPERS_BUILD_DIR)
	python3 $(LEARNSET_HELPERS_DIR)/make_teaching_types.py $@

$(DATA_SRC_SUBDIR)/pokemon/teachable_learnsets.h: $(TEACHABLE_DEPS) | $(ALL_TUTORS_JSON) $(ALL_TEACHING_TYPES_JSON)
	python3 $(LEARNSET_HELPERS_DIR)/make_teachables.py $(LEARNSET_HELPERS_BUILD_DIR)

$(EC_PREPARATION_LEARNSETS): $(ALL_LEARNABLES_JSON) $(MOVE_ACCESS_REVIEW_JSON) $(PREPARATION_FORM_LEARNSETS_JSON) $(INCLUDE_DIRS)/constants/species.h $(INCLUDE_DIRS)/constants/moves.h $(LEARNSET_HELPERS_DIR)/make_teachables.py
	python3 $(LEARNSET_HELPERS_DIR)/make_teachables.py --preparation

$(DATA_SRC_SUBDIR)/tutor_moves.h: $(DATA_SRC_SUBDIR)/pokemon/special_movesets.json $(MOVE_ACCESS_REVIEW_JSON) | $(ALL_TUTORS_JSON)
	python3 $(LEARNSET_HELPERS_DIR)/make_teachables.py  --tutors $(LEARNSET_HELPERS_BUILD_DIR)

# Linker script
LD_SCRIPT := ld_script_modern.ld

# Build provenance. One identifier for the source (commit plus uncommitted
# build inputs), configuration and toolchain is assembled into the ROM as the
# kept symbol gEcBuildProvenance; after the ROM exists the same script checks
# the embedded identifier, fingerprints the compiled save layout and writes
# $(PROVENANCE) beside the ROM. See scripts/build_provenance.py.
BUILD_PROVENANCE := scripts/build_provenance.py
PROVENANCE_PREPARED := $(OBJ_DIR)/.build-provenance.json
PROVENANCE_ASM := $(OBJ_DIR)/ec_build_provenance.s
PROVENANCE_OBJ := $(OBJ_DIR)/ec_build_provenance.o
PROVENANCE_OBJ_REL := $(patsubst $(OBJ_DIR)/%,%,$(PROVENANCE_OBJ))

$(PROVENANCE_ASM): FORCE_BUILD_CONFIG
	@python3 $(BUILD_PROVENANCE) prepare --asm $@ --json $(PROVENANCE_PREPARED) --value=$(call shell_quote,ROM=$(ROM)) --value=$(call shell_quote,HEADER=$(TITLE)|$(GAME_CODE)|$(MAKER_CODE)|$(REVISION)) --value=$(call shell_quote,GAME_VERSION=$(GAME_VERSION)) --value=$(call shell_quote,MAP_VERSION=$(MAP_VERSION)) --value=$(call shell_quote,MODES=RELEASE=$(RELEASE) DEBUG=$(DEBUG) TEST=$(TEST) LTO=$(LTO) EC_HEADLESS_FIXTURES=$(EC_HEADLESS_FIXTURES)) --value=$(call shell_quote,CPPFLAGS=$(CONFIG_CPPFLAGS)) --value=$(call shell_quote,CFLAGS=$(CONFIG_CFLAGS)) --value=$(call shell_quote,ASFLAGS=$(CONFIG_ASFLAGS)) --value=$(call shell_quote,LDFLAGS=$(LDFLAGS)) --tool=$(call shell_quote,$(CONFIG_ARMCC)) --tool=$(call shell_quote,$(CC1)) --tool=$(call shell_quote,$(CONFIG_AS)) --tool=$(call shell_quote,$(LD)) --tool=$(call shell_quote,$(OBJCOPY)) --tool=$(call shell_quote,$(FIX)) --tool=$(call shell_quote,$(CONFIG_PREPROC))

$(PROVENANCE_OBJ): $(PROVENANCE_ASM)
	$(AS) $(ASFLAGS) -o $@ - < $<

# Final rules

libagbsyscall:
	@$(MAKE) -C libagbsyscall TOOLCHAIN=$(TOOLCHAIN) MODERN=1

# Enable LTO LDFLAGS if set
ifneq ($(LTO),0)
LDFLAGS := -march=armv4t -mabi=apcs-gnu -mcpu=arm7tdmi -Xlinker -Map=../../$(MAP) -Xlinker --print-memory-usage -Xassembler -meabi=5 -Xassembler -march=armv4t -Xassembler -mcpu=arm7tdmi -Xlinker --gc-sections
LDFLAGS += -Xlinker -flto=auto
$(ELF): $(LD_SCRIPT) $(OBJS) $(OBJ_DIR)/.link-config.json $(PROVENANCE_OBJ)
	@echo "cd $(OBJ_DIR) && $(ARMCC) $(LDFLAGS) -T ../../$< -o ../../$@ <objs> <libs>"
	+@cd $(OBJ_DIR) && $(ARMCC) $(LDFLAGS) -T ../../$< -o ../../$@ $(OBJS_REL) $(PROVENANCE_OBJ_REL) $(LIB)
	$(FIX) $@ -t"$(TITLE)" -c$(GAME_CODE) -m$(MAKER_CODE) -r$(REVISION) --silent
else
# Output .map file, memory usage readout and gc sections to clean-up unused data
LDFLAGS = -Map ../../$(MAP) --print-memory-usage --gc-sections
$(ELF): $(LD_SCRIPT) $(OBJS) $(OBJ_DIR)/.link-config.json $(PROVENANCE_OBJ)
	@cd $(OBJ_DIR) && $(LD) $(LDFLAGS) -T ../../$<  -o ../../$@ $(OBJS_REL) $(PROVENANCE_OBJ_REL) $(LIB) | cat
	@echo "cd $(OBJ_DIR) && $(LD) $(LDFLAGS) -T ../../$< -o ../../$@ <objs> <libs> | cat"
	$(FIX) $@ -t"$(TITLE)" -c$(GAME_CODE) -m$(MAKER_CODE) -r$(REVISION) --silent
endif

# Builds the rom from the elf file
$(ROM): $(ELF)
	@rm -f $(PROVENANCE)
	$(OBJCOPY) -O binary $< $@
	$(FIX) $@ -p --silent
	@python3 $(BUILD_PROVENANCE) finalize --prepared $(PROVENANCE_PREPARED) --rom $@ --elf $< --out $(PROVENANCE) --cc=$(call shell_quote,$(ARMCC)) --objdump=$(call shell_quote,$(OBJDUMP)) --nm=$(call shell_quote,$(NM)) --flags=$(call shell_quote,$(CONFIG_CPPFLAGS) $(CONFIG_CFLAGS)) --work $(OBJ_DIR)/save-layout

emerald: all
firered: all
leafgreen: all
# Symbol file (`make syms`)
$(SYM): $(ELF)
	$(OBJDUMP) -t $< | sort -u | grep -E "^0[2389]" | $(PERL) -p -e 's/^(\w{8}) (\w).{6} \S+\t(\w{8}) (\S+)$$/\1 \2 \3 \4/g' > $@

# Library builds are order-only here: their content identity, not a phony
# prerequisite, decides whether the linked images need rebuilding.
$(OBJ_DIR)/.link-config.json: FORCE_BUILD_CONFIG | libagbsyscall
	@python3 $(BUILD_CONFIG_HELPER) $@ --value=$(call shell_quote,LDFLAGS=$(LDFLAGS)) --value=$(call shell_quote,LIB=$(LIB)) --value=$(call shell_quote,OBJECTS=$(OBJS_REL)) --value=$(call shell_quote,LTO=$(LTO)) --value=$(call shell_quote,HEADER=$(TITLE)|$(GAME_CODE)|$(MAKER_CODE)|$(REVISION)) --tool=$(call shell_quote,$(LD)) --tool=$(call shell_quote,$(ARMCC)) --tool=$(call shell_quote,$(FIX)) --file=Makefile --file=$(LD_SCRIPT) --libraries=$(call shell_quote,$(LIB)) --library-directory=$(OBJ_DIR) --env=LIBRARY_PATH --env=COMPILER_PATH --env=GCC_EXEC_PREFIX

$(OBJ_DIR)/.test-link-config.json: FORCE_BUILD_CONFIG | libagbsyscall tools check-tools
	@python3 $(BUILD_CONFIG_HELPER) $@ --value=$(call shell_quote,TESTLDFLAGS=$(TESTLDFLAGS)) --value=$(call shell_quote,LIB=$(LIB)) --value=$(call shell_quote,OBJECTS=$(OBJS_REL)|$(TEST_OBJS_REL)) --value=$(call shell_quote,HEADER=$(TITLE)|$(GAME_CODE)|$(MAKER_CODE)|$(REVISION)) --tool=$(call shell_quote,$(LD)) --tool=$(call shell_quote,$(FIX)) --file=Makefile --file=$(LD_SCRIPT_TEST) --libraries=$(call shell_quote,$(LIB)) --library-directory=$(OBJ_DIR) --env=LIBRARY_PATH
