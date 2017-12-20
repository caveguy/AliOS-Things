TOOLCHAIN_PATH ?=
TOOLCHAIN_PREFIX := arm
CC      := $(TOOLCHAIN_PATH)iccarm
CXX     := $(TOOLCHAIN_PATH)iccarm
AS      := $(TOOLCHAIN_PATH)iasmarm
AR      := $(TOOLCHAIN_PATH)iarchive
LD      := $(TOOLCHAIN_PATH)ilinkarm
CPP     := $(TOOLCHAIN_PATH)iccarm

OPTIONS_IN_FILE_OPTION_PREFIX_DIRECT  = $
OPTIONS_IN_FILE_OPTION_PREFIX         = $$
OPTIONS_IN_FILE_OPTION                = (file < 
OPTIONS_IN_FILE_OPTION_SUFFIX         = )
OBJCOPYFLAGS := --bin --output=

ADD_COMPILER_SPECIFIC_STANDARD_CFLAGS   = 
ADD_COMPILER_SPECIFIC_STANDARD_CXXFLAGS = 
ADD_COMPILER_SPECIFIC_STANDARD_ADMFLAGS = $(1)
COMPILER_SPECIFIC_OPTIMIZED_CFLAGS     := -Os
COMPILER_SPECIFIC_UNOPTIMIZED_CFLAGS   := -O0
COMPILER_SPECIFIC_PEDANTIC_CFLAGS      := $(COMPILER_SPECIFIC_STANDARD_CFLAGS)
COMPILER_SPECIFIC_ARFLAGS_CREATE       := --create
COMPILER_SPECIFIC_ARFLAGS_ADD          := --create
COMPILER_SPECIFIC_ARFLAGS_VERBOSE      := -V

#debug: no optimize and log enable
COMPILER_SPECIFIC_DEBUG_CFLAGS         := -DDEBUG -Dgdb $(COMPILER_SPECIFIC_UNOPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_DEBUG_CXXFLAGS       := -DDEBUG -Dgdb $(COMPILER_SPECIFIC_UNOPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_DEBUG_ASFLAGS        := 
COMPILER_SPECIFIC_DEBUG_LDFLAGS        :=

#release_log: optimize but log enable
COMPILER_SPECIFIC_RELEASE_LOG_CFLAGS   := -Dgdb $(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_RELEASE_LOG_CXXFLAGS := -Dgdb $(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_RELEASE_LOG_ASFLAGS  :=
COMPILER_SPECIFIC_RELEASE_LOG_LDFLAGS  := 

#release: optimize and log disable
COMPILER_SPECIFIC_RELEASE_CFLAGS       := -DNDEBUG $(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_RELEASE_CXXFLAGS     := -DNDEBUG $(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_RELEASE_ASFLAGS      :=
COMPILER_SPECIFIC_RELEASE_LDFLAGS      := 

COMPILER_SPECIFIC_DEPS_FLAG                :=
COMPILER_SPECIFIC_COMP_ONLY_FLAG           := -c -e --dlib_config=full
COMPILER_SPECIFIC_LINK_MAP                  = --map $(1)
COMPILER_SPECIFIC_LINK_FILES                = $(1)
COMPILER_SPECIFIC_LINK_SCRIPT_DEFINE_OPTION = 
COMPILER_SPECIFIC_LINK_SCRIPT               =

LINKER                             := $(LD)
LINK_SCRIPT_SUFFIX                 := .icf
TOOLCHAIN_NAME := iar

ENDIAN_CFLAGS_LITTLE   := --littleend
ENDIAN_CXXFLAGS_LITTLE := --littleend
ENDIAN_ASMFLAGS_LITTLE :=
ENDIAN_LDFLAGS_LITTLE  := --littleend
CLIB_LDFLAGS_NANO      := 
CLIB_LDFLAGS_NANO_FLOAT:= 

# Chip specific flags for compiler
CPU_CFLAGS     :=
CPU_CXXFLAGS   := 
CPU_ASMFLAGS   := 
CPU_LDFLAGS    :=

# $(1) is map file, $(2) is CSV output file
#COMPILER_SPECIFIC_MAPFILE_TO_CSV = $(PYTHON) $(MAPFILE_PARSER) $(1) > $(2)

#TODO
#MAPFILE_PARSER            :=$(MAKEFILES_PATH)/scripts/map_parse_armcc.py

# $(1) is map file, $(2) is CSV output file
COMPILER_SPECIFIC_MAPFILE_DISPLAY_SUMMARY = $(PYTHON) $(MAPFILE_PARSER) $(1)

OBJDUMP := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)ielfdumparm$(EXECUTABLE_SUFFIX)"
OBJCOPY := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)ielftool$(EXECUTABLE_SUFFIX)"

#no need to strip in arm fromelf
#STRIP   := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)fromelf$(EXECUTABLE_SUFFIX)"

LINK_OUTPUT_SUFFIX  :=.iarElf
BIN_OUTPUT_SUFFIX   :=.bin
HEX_OUTPUT_SUFFIX   :=.hex

