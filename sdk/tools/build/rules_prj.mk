
# Search dir inlcude LDSCRIPT's dir
LDDIR += $(dir $(LDSCRIPT))

# to -Iinclude and -Ddefine
INCLUDES_I = $(patsubst %,-I%,$(INCLUDES))
SOURCES_R = $(subst $(ROOT_DIR),,$(SOURCES))
DEFINES_D = $(patsubst %,-D%,$(DEFINES))
LDDIR_L	= $(patsubst %,-L%,$(LDDIR))

# CPU
CPU = -mthumb -mcpu=cortex-m4 # M4

# build obj files direcotry
BUILDDIR = .build_$(PROJECT)

# verbose mode
ifeq ($(V),1)
TRACE_CC  =
TRACE_AS  =
TRACE_LD  =
TRACE_AR  =
Q=
else
TRACE_CC  = @echo "	CC    $<"
TRACE_AS  = @echo "	AS    $<"
TRACE_LD  = @echo "Linking $@ ..."
TRACE_AR  = @echo "Archiving $@ ..."
Q=@
endif

# Target thing
TARGET					?= $(PROJECT)
TARGET_ELF	 			?= $(TARGET).elf
TARGET_BIN				?= $(TARGET).bin
TARGET_HEX				?= $(TARGET).hex
TARGET_LIB				?= $(TARGET).a
TARGET_MAP				?= $(TARGET).map
TARGET_DIS				?= $(TARGET).dis
OBJCPFLAGS_ELF_TO_BIN	 = -Obinary
OBJCPFLAGS_ELF_TO_HEX	 = -Oihex
OBJCPFLAGS_HEX_TO_BIN	 = -Obinary -Iihex

# Tools
CC			= arm-none-eabi-gcc
AS			= arm-none-eabi-gcc -x assembler-with-cpp
LD			= arm-none-eabi-ld
AR			= arm-none-eabi-ar
OBJCP		= arm-none-eabi-objcopy
OBJDUMP		= arm-none-eabi-objdump
OBJSIZE		= arm-none-eabi-size
STRIP		= arm-none-eabi-strip

# Optimization: 0, 1, 2, 3, s, ultrasize
ifeq ($(CONFIG_OPTIMIZATION_LEVEL), 4)
CC_OPTIMIZE = -Os -fdata-sections -ffunction-sections
LD_OPTIMIZE = -Wl,--gc-sections
else ifeq ($(CONFIG_OPTIMIZATION_LEVEL), 3)
CC_OPTIMIZE = -Os
LD_OPTIMIZE =
else
CC_OPTIMIZE = -O$(CONFIG_OPTIMIZATION_LEVEL)
LD_OPTIMIZE =
endif

# LD ext files
ifneq ($(LDEXTFILE), )
include $(LDEXTFILE)
endif

# Warnings
WARNINGS 	= -Wall -Wformat=0 -Wstrict-aliasing=0

# Flags (-N: prevent link error in FLASH version: "not enough room for program headers")
CCFLAGS		= $(CPU) -ggdb3 -std=gnu99 -fno-delete-null-pointer-checks $(WARNINGS) $(CC_OPTIMIZE) $(INSTRUMENT_FUNCTIONS) $(DEFINES_D) $(INCLUDES_I)
CPPFLAGS	= $(CCFLAGS)
ASFLAGS		= $(CPU) $(DEFINES_D) $(INCLUDES_I)
LDFLAGS		= -nostartfiles -static $(CCFLAGS) $(LD_OPTIMIZE) $(LDDIR_L) -T$(LDSCRIPT) -Wl,-Map=$(TARGET_MAP),--cref -Wl,--print-memory-usage -N
DEPFLAGS	= -MT $@ -MMD -MP -MF $*.d

# Librarys
LDLIBS		= --specs=nano.specs -lc -lnosys -lm $(LIBRARIES) # -u _printf_float

# Files
FILES_C_OBJ = $(patsubst %,$(BUILDDIR)/%,$(filter %.o, $(SOURCES_R:%.c=%.o)))
FILES_S_OBJ = $(patsubst %,$(BUILDDIR)/%,$(filter %.o, $(SOURCES_R:%.S=%.o)))
FILES_C_DEPEND = $(patsubst %,$(BUILDDIR)/%,$(filter %.d, $(SOURCES_R:%.c=%.d)))
FILES_S_DEPEND = $(patsubst %,$(BUILDDIR)/%,$(filter %.d, $(SOURCES_R:%.S=%.d)))

# Target
ifeq ($(CONFIG_LIB_GENERATE), y)
TARGET_OUT = $(TARGET_LIB)
else
TARGET_OUT = $(TARGET_HEX) $(TARGET_BIN)
endif

# PHONY
.PHONY: all

# Target
all: $(TARGET_OUT)

# bin
$(TARGET_BIN) : $(TARGET_HEX)
	$(Q)$(OBJCP) $(OBJCPFLAGS_HEX_TO_BIN) $< $@
	@cp -f $(TARGET_BIN) a.bin
	@cp -f $(TARGET_ELF) a.axf
	@echo "\nEach object size:" >> $(TARGET_MAP)
	@find $(BUILDDIR) -name '*.o' | xargs $(OBJSIZE) -B -d >> $(TARGET_MAP)
	$(POSTBUILDPROG)
	@echo "Build done"

# hex
$(TARGET_HEX) : $(TARGET_ELF)
	$(Q)$(OBJCP) $(OBJCPFLAGS_ELF_TO_HEX) $< $@

# elf
$(TARGET_ELF) : $(FILES_C_OBJ) $(FILES_S_OBJ)
	$(TRACE_LD)
	$(Q)$(CC) $+ $(LDFLAGS) $(LDLIBS) -o $@
	@echo "--------------------------------------------------"
	$(Q)$(OBJSIZE) -B -d $@
	$(Q)$(OBJDUMP) -d $@ > $(TARGET_DIS)

# library
$(TARGET_LIB) : $(FILES_C_OBJ) $(FILES_S_OBJ)
	$(TRACE_AR)
	$(Q)$(AR) cr $@ $^
	$(POSTBUILDPROG)
	@echo "Build done"

# c -> o
$(BUILDDIR)/%.o : $(ROOT_DIR)%.c
$(BUILDDIR)/%.o : $(ROOT_DIR)%.c $(BUILDDIR)/%.d
	$(TRACE_CC)
	@mkdir -p $(@D) >/dev/null
	$(Q)$(CC) -MT $@ -MMD -MP -MF $(BUILDDIR)/$*.Td $(CCFLAGS) $(CPPFLAGS) -c $< -o $@
	@mv -f $(BUILDDIR)/$*.Td $(BUILDDIR)/$*.d

# s -> o
$(BUILDDIR)/%.o : $(ROOT_DIR)%.S
$(BUILDDIR)/%.o : $(ROOT_DIR)%.S $(BUILDDIR)/%.d
	$(TRACE_AS)
	@mkdir -p $(@D) >/dev/null
	$(Q)$(AS) -MT $@ -MMD -MP -MF $(BUILDDIR)/$*.Td $(ASFLAGS) -c $< -o $@
	@mv -f $(BUILDDIR)/$*.Td $(BUILDDIR)/$*.d

# d
$(BUILDDIR)/%.d: ;
.PRECIOUS: $(BUILDDIR)/%.d

# Include dependent (*.d)
-include $(FILES_C_DEPEND)
-include $(FILES_S_DEPEND)

# PHONY
.PHONY: list clean

# some info
list:
	@echo srcs:--------------------------------------------------------------------------------
	@for f in $(SOURCES); do echo $$f; done
	@echo incs:--------------------------------------------------------------------------------
	@for f in $(INCLUDES); do echo $$f; done
	@echo defs:--------------------------------------------------------------------------------
	@for f in $(DEFINES); do echo $$f; done

# Clean
clean:
	$(Q)rm -rf $(BUILDDIR) $(TARGET_ELF) $(TARGET_BIN) $(TARGET_HEX) $(TARGET_MAP) $(TARGET_DIS) a.bin a.axf

