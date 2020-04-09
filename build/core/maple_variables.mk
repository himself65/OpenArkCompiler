#
# Copyright (c) [2020] Huawei Technologies Co.,Ltd.All rights reserved.
#
# OpenArkCompiler is licensed under the Mulan PSL v1.
# You can use this software according to the terms and conditions of the Mulan PSL v1.
# You may obtain a copy of Mulan PSL v1 at:
#
#     http://license.coscl.org.cn/MulanPSL
#
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
# FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v1 for more details.
#
OPT := O0
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
