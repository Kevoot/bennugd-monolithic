#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
# EXEFS_SRC is the optional input directory containing data copied into exefs, if anything this normally should only contain "main.npdm".
# ROMFS is the directory containing data to be added to RomFS, relative to the Makefile (Optional)

# NO_ICON: if set to anything, do not use icon.
# NO_NACP: if set to anything, no .nacp file is generated.
# APP_TITLE is the name of the app stored in the .nacp file (Optional)
# APP_AUTHOR is the author of the app stored in the .nacp file (Optional)
# APP_VERSION is the version of the app stored in the .nacp file (Optional)
# APP_TITLEID is the titleID of the app stored in the .nacp file (Optional)
# ICON is the filename of the icon (.jpg), relative to the project folder.
#   If not set, it attempts to use one of the following (in this order):
#     - <Project name>.jpg
#     - icon.jpg
#     - <libnx folder>/default_icon.jpg
#---------------------------------------------------------------------------------
TARGET		:=	LaiNes
BUILD		:=	build
SOURCES		:=	src \
				../../3rdparty/libc_additions/ \
				../common/ \
				$(SRC_OBJS) \
				$(EXT_LIBS)

INCLUDES	:=	include ../include ../../modules/mod_say/ ../../modules/mod_string/ \
                ../../modules/mod_math/ ../../modules/mod_time/ ../../modules/mod_file/ \
                ../../modules/mod_sound/ ../../modules/libsdlhandler/ ../../modules/mod_joy/ \
                ../../modules/libjoy/ ../../modules/mod_proc/ ../../modules/mod_sort/ \
                ../../modules/mod_timers/ ../../modules/mod_regex/ ../../modules/libgrbase/ \
                ../../modules/libblit/ ../../modules/libvideo/ ../../modules/librender/ \
                ../../modules/mod_video/ ../../modules/libmouse/ ../../modules/mod_mouse/ \
                ../../modules/mod_map/ ../../modules/libfont/  ../../modules/mod_dir/ \
                ../../modules/libtext/ ../../modules/mod_text/ ../../modules/mod_rand/ \
                ../../modules/mod_grproc/ ../../modules/libscroll/ ../../modules/mod_scroll/ \
                ../../modules/libkey/ ../../modules/mod_key/ ../../modules/libdraw/ \
                ../../modules/mod_draw/ ../../modules/mod_screen/ ../../modules/mod_path/ \
                ../../modules/mod_effects/ ../../modules/mod_blendop/ ../../modules/mod_m7/ \
				../../modules/libwm/ ../../modules/mod_wm/ ../../modules/mod_sys/ \
				../../modules/mod_mem/ ../../modules/mod_flic/ ../../modules/mod_mathi/ \
				../../modules/mod_debug/ ../../modules/mod_iconv/ ../../modules/mod_wpad/ \
				../../modules/mod_image/ ../../modules/mod_sqlite3/ ../../modules/mod_chipmunk/ \
				../../3rdparty/libc_additions ../common/include/ \
				$(INC_OBJS) \
				$(EXT_LIBS)
		
APP_TITLE	:= BennuGDSwitch
APP_AUTHOR	:= Kevoot
APP_VERSION	:= 1.0
APP_TITLEID	:= 2481

ICON		:=

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH		:=	-march=armv8-a -mtune=cortex-a57 -mtp=hard -fPIE
DEFINES 	+=  -DWITH_GPL_CODE -D__MONOLITHIC__ -DNO_MODMATHI -DNO_MODIMAGE -DNO_MODCHIPMUNK -DNO_MODMULTI -DNO_MODFMODEX -DNO_MODCURL -DVERSION='"1.0.0"' -DHAVE_DLFCN=0
CFLAGS		=	-g -O2 -Wall -D__BGDC__ -ffunction-sections -fpermissive -ffast-math -funsafe-math-optimizations `sdl2-config --cflags` `freetype-config --cflags` \
				$(ARCH) $(DEFINES) $(INCLUDE) -D__SWITCH__
CXXFLAGS	=	$(CFLAGS) -fno-rtti -fexceptions -std=gnu++11 #-save-temps -Xassembler -aln=$@.lst $(CFLAGS)
ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-g $(ARCH) -Wl,-Map,$(notdir $*.map)

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS := -lz -lm -lSDL2_mixer -lmpg123 -lSDL2_ttf -lSDL2_gfx -lSDL2_image -lpng -ljpeg `sdl2-config --libs` `freetype-config --libs` -specs=$(DEVKITPRO)/libnx/switch.specs -lnx
#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(CURDIR) $(PORTLIBS) $(LIBNX)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR	:=	$(CURDIR)/$(BUILD)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))
	
#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
			$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

export BUILD_EXEFS_SRC := $(TOPDIR)/$(EXEFS_SRC)

ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.jpg)
	ifneq (,$(findstring $(TARGET).jpg,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).jpg
	else
		ifneq (,$(findstring icon.jpg,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.jpg
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif

ifeq ($(strip $(NO_ICON)),)
	export NROFLAGS += --icon=$(APP_ICON)
endif

ifeq ($(strip $(NO_NACP)),)
	export NROFLAGS += --nacp=$(CURDIR)/$(TARGET).nacp
endif

ifneq ($(APP_TITLEID),)
	export NACPFLAGS += --titleid=$(APP_TITLEID)
endif

ifneq ($(ROMFS),)
	export NROFLAGS += --romfsdir=$(CURDIR)/$(ROMFS)
endif

.PHONY: $(BUILD) clean all

#---------------------------------------------------------------------------------
all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).pfs0 $(TARGET).nso $(TARGET).nro $(TARGET).nacp $(TARGET).elf


#---------------------------------------------------------------------------------
else
.PHONY:	all

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all	:	$(OUTPUT).pfs0 $(OUTPUT).nro

$(OUTPUT).pfs0	:	$(OUTPUT).nso

$(OUTPUT).nso	:	$(OUTPUT).elf

ifeq ($(strip $(NO_NACP)),)
$(OUTPUT).nro	:	$(OUTPUT).elf $(OUTPUT).nacp
else
$(OUTPUT).nro	:	$(OUTPUT).elf
endif

$(OUTPUT).elf	:	$(OFILES)
#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.lua.o	:	%.lua
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.ttf.o	:	%.ttf
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.png.o	:	%.png
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------