# the SOURCE definiton lets you move your makefile to another position
CONFIG 				= CONSOLE

# set directories to your wanted values
SRC_DIR				= ../../../../source/App/TAppEncoder
INC_DIR				= ../../../../source/Lib
LIB_DIR				= ../../../../lib
BIN_DIR				= ../../../../bin

SRC_DIR1		=
SRC_DIR2		=
SRC_DIR3		=
SRC_DIR4		=

USER_INC_DIRS	= -I$(SRC_DIR)
USER_LIB_DIRS	=

# intermediate directory for object files
OBJ_DIR				= ./objects

# set executable name
PRJ_NAME			= TAppEncoder

# defines to set
DEFS				= -DMSYS_LINUX -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DMSYS_UNIX_LARGEFILE

# set objects
OBJS          		= 	\
					$(OBJ_DIR)/encmain.o \
					$(OBJ_DIR)/TAppEncCfg.o \
					$(OBJ_DIR)/TAppEncTop.o \

# set libs to link with
LIBS				= -ldl

DEBUG_LIBS			=
RELEASE_LIBS		=

STAT_LIBS			= -lpthread
DYN_LIBS			=


DYN_DEBUG_LIBS		= -lTLibEncoderd -lTLibCommond -lTLibVideoIOd -lTAppCommond -lTLibRendererd
DYN_DEBUG_PREREQS		= $(LIB_DIR)/libTLibEncoderd.a $(LIB_DIR)/libTLibCommond.a $(LIB_DIR)/libTLibVideoIOd.a $(LIB_DIR)/libTAppCommond.a $(LIB_DIR)/libTLibRendererd.a
STAT_DEBUG_LIBS		= -lTLibEncoderStaticd -lTLibCommonStaticd -lTLibVideoIOStaticd -lTAppCommonStaticd -lTLibRendererStaticd
STAT_DEBUG_PREREQS		= $(LIB_DIR)/libTLibEncoderStaticd.a $(LIB_DIR)/libTLibCommonStaticd.a $(LIB_DIR)/libTLibVideoIOStaticd.a $(LIB_DIR)/libTAppCommonStaticd.a $(LIB_DIR)/libTLibRendererStaticd.a

DYN_RELEASE_LIBS	= -lTLibEncoder -lTLibCommon -lTLibVideoIO -lTAppCommon -lTLibRenderer
DYN_RELEASE_PREREQS	= $(LIB_DIR)/libTLibEncoder.a $(LIB_DIR)/libTLibCommon.a $(LIB_DIR)/libTLibVideoIO.a $(LIB_DIR)/libTAppCommon.a $(LIB_DIR)/libTLibRenderer.a
STAT_RELEASE_LIBS	= -lTLibEncoderStatic -lTLibCommonStatic -lTLibVideoIOStatic -lTAppCommonStatic -lTLibRendererStatic
STAT_RELEASE_PREREQS	= $(LIB_DIR)/libTLibEncoderStatic.a $(LIB_DIR)/libTLibCommonStatic.a $(LIB_DIR)/libTLibVideoIOStatic.a $(LIB_DIR)/libTAppCommonStatic.a $(LIB_DIR)/libTLibRendererStatic.a


# name of the base makefile
MAKE_FILE_NAME		= ../../common/makefile.base

# include the base makefile
include $(MAKE_FILE_NAME)
