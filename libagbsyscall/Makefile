TOOLCHAIN := $(DEVKITARM)

ifneq (,$(TOOLCHAIN))
ifneq ($(wildcard $(TOOLCHAIN)/bin),)
export PATH := $(TOOLCHAIN)/bin:$(PATH)
endif
endif

PREFIX := arm-none-eabi-
AS := $(PREFIX)as
AR := $(PREFIX)ar

ASFLAGS := -mcpu=arm7tdmi
ARFLAGS := rc

SYSCALLS := IntrWait \
            RegisterRamReset \
            Sqrt \
            MusicPlayerOpen \
            SoundBiasReset \
            SoundDriverVSyncOn \
            Mod \
            VBlankIntrWait \
            MusicPlayerStart \
            SoundDriverVSyncOff \
            HuffUnComp \
            SoftResetExram \
            MusicPlayerFadeOut \
            LZ77UnCompWram \
            SoundDriverMain \
            SoundBiasChange \
            LZ77UnCompVram \
            ArcTan2 \
            MusicPlayerStop \
            DivArm \
            ModArm \
            SoundDriverVSync \
            SoundDriverInit \
            BgAffineSet \
            Diff8bitUnFilterWram \
            MultiBoot \
            MidiKey2Freq \
            Div \
            Diff8bitUnFilterVram \
            ArcTan \
            ObjAffineSet \
            SoftResetRom \
            SoundDriverMode \
            RLUnCompWram \
            BitUnPack \
            SoundChannelClear \
            CpuFastSet \
            CpuSet \
            Diff16bitUnFilter \
            SoundBiasSet \
            MusicPlayerContinue \
            SoftReset \
            RLUnCompVram

ASM_SRCS := libagbsyscall.s
ASM_INCLUDES := ../constants/gba_constants.inc ../asm/macros/function.inc
ASM_OBJS := $(foreach syscall, $(SYSCALLS), $(syscall).o)

LIB := libagbsyscall.a

.DELETE_ON_ERROR:
.PHONY: all clean FORCE

all: $(LIB)
	@:

clean:
	rm -f $(LIB) $(ASM_OBJS) $(ASM_CONFIG) $(AR_CONFIG)

# Track effective commands independently so archive-only changes do not assemble.
ASM_CONFIG := .assembly-config.json
AR_CONFIG := .archive-config.json
CONFIG_HELPER := ../scripts/update_build_config.py
shell_quote = '$(subst ','"'"',$(1))'
ASSEMBLE = $(AS) $(ASFLAGS) --defsym L_$(*F)=1 -o $@ $<
ARCHIVE = rm -f $@ && $(AR) $(ARFLAGS) $@ $(ASM_OBJS)

$(ASM_CONFIG): FORCE
	@python3 $(CONFIG_HELPER) $@ --tool=$(call shell_quote,$(AS)) --value=$(call shell_quote,$(ASFLAGS)) --value=$(call shell_quote,$(value ASSEMBLE)) $(foreach input,$(ASM_INCLUDES),--file=$(call shell_quote,$(input)))

$(AR_CONFIG): FORCE
	@python3 $(CONFIG_HELPER) $@ --tool=$(call shell_quote,$(AR)) --value=$(call shell_quote,$(ARFLAGS)) --value=$(call shell_quote,$(value ARCHIVE)) --value=$(call shell_quote,$(ASM_OBJS))

FORCE:

$(LIB): $(ASM_OBJS) $(AR_CONFIG)
	$(ARCHIVE)

$(ASM_OBJS): libagbsyscall.s $(ASM_INCLUDES) $(ASM_CONFIG)
	$(ASSEMBLE)
