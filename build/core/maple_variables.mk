OPT := O2
TARGETS := $(APP)
LIB_CORE_PATH := $(MAPLE_ROOT)/libjava-core
LIB_CORE_JAR := $(LIB_CORE_PATH)/java-core.jar
LIB_CORE_MPLT := $(LIB_CORE_PATH)/java-core.mplt
APP_CLASS := $(foreach APP, $(TARGETS), $(APP).class)
APP_JAR := $(foreach APP, $(TARGETS), $(APP).jar)
APP_MPL := $(foreach APP, $(TARGETS), $(APP).mpl)
APP_MPLT:=$(foreach APP, $(TARGETS), $(APP).mplt)
APP_VTABLEIMPL_MPL := $(foreach APP, $(TARGETS), $(APP).VtableImpl.mpl)
APP_S := $(foreach APP, $(TARGETS), $(APP).VtableImpl.s)

JAVA2JAR := $(MAPLE_ROOT)/output/bin/java2jar
JBC2MPL_BIN := $(MAPLE_ROOT)/output/bin/jbc2mpl
MAPLE_BIN := $(MAPLE_ROOT)/output/bin/maple
MPLCG_BIN := $(MAPLE_ROOT)/output/bin/mplcg
