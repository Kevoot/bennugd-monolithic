#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

include $(DEVKITPPC)/wii_rules

# Directories.
OBJ_DIR			:= obj
LIB_DIR			:= lib
BIN_DIR			:= bin
SRC_DIR			:= src
INCLUDE_DIR		:= include

# Tools.
PIPE_TO_SED := 2>&1 | sed "s/:\([0-9]*\):/\(\1\) :/"

# Library source files.
SRCS	:= $(SRC_DIR)/chipmunk.c $(SRC_DIR)/cpArbiter.c $(SRC_DIR)/cpArray.c \
	$(SRC_DIR)/cpBB.c $(SRC_DIR)/cpBody.c $(SRC_DIR)/cpCollision.c \
	$(SRC_DIR)/cpHashSet.c $(SRC_DIR)/cpPolyShape.c $(SRC_DIR)/cpShape.c \
	$(SRC_DIR)/cpSpace.c $(SRC_DIR)/cpSpaceComponent.c $(SRC_DIR)/cpSpaceHash.c \
	$(SRC_DIR)/cpSpaceQuery.c $(SRC_DIR)/cpSpaceStep.c $(SRC_DIR)/cpVect.c \
	$(SRC_DIR)/constraints/cpConstraint.c $(SRC_DIR)/constraints/cpDampedRotarySpring.c \
	$(SRC_DIR)/constraints/cpDampedSpring.c $(SRC_DIR)/constraints/cpGearJoint.c \
	$(SRC_DIR)/constraints/cpGrooveJoint.c $(SRC_DIR)/constraints/cpPinJoint.c \
	$(SRC_DIR)/constraints/cpPivotJoint.c $(SRC_DIR)/constraints/cpRatchetJoint.c \
	$(SRC_DIR)/constraints/cpRotaryLimitJoint.c $(SRC_DIR)/constraints/cpSimpleMotor.c \
	$(SRC_DIR)/constraints/cpSlideJoint.c

# Library object files.
OBJS	:= $(subst $(SRC_DIR),$(OBJ_DIR),$(SRCS:.c=.o))

# What's a full build?
all: $(LIB_DIR)/libchipmunk.a

# How to delete the intermediate files.
clean:
	@echo Cleaning $(LIB_DIR)/libchipmunk.a $(OBJ_DIR)
	@rm -f $(LIB_DIR)/libchipmunk.a $(OBJS) $(TEST_OBJS)
	
install:
	@mkdir -p $(INSTALL_HEADER_DIR) $(INSTALL_LIB_DIR)
	@cp -frv $(LIB_DIR)/*.a $(INSTALL_LIB_DIR)
	@cp -frv $(INCLUDE_DIR)/*.h $(INSTALL_HEADER_DIR)

# How to build a library.
$(LIB_DIR)/libchipmunk.a: $(OBJS)
	@echo Archiving $@
	@-mkdir -p $(dir $@)
	powerpc-eabi-ar crs $@ $(OBJS)
	@echo ----

# Compilation flags.
COMMON_FLAGS	:= -std=gnu99 -O3 -mrvl -ffast-math $(MACHDEP)
INCLUDES		:= -Iinclude/chipmunk -Iinclude/chipmunk/constraints -I$(DEVKITPRO)/libogc/include
# -include sys/wait.h 
# -include sys/syslimits.h

DEFINES			:= -DGEKKO

CFLAGS			:= $(COMMON_FLAGS) $(INCLUDES) $(DEFINES)

# How to compile C file.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo Compiling $<
	@-mkdir -p $(dir $@)
	powerpc-eabi-gcc $(CFLAGS) -c $< -o $@ $(PIPE_TO_SED)

