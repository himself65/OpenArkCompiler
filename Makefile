#
# Makefile for OpenArkCompiler
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

INSTALL_DIR := ${MAPLE_ROOT}/output
BUILD_TYPE := RELEASE
HOST_ARCH := 64
MIR_JAVA := 1
GN := ${MAPLE_ROOT}/tools/gn/gn
NINJA := ${MAPLE_ROOT}/tools/ninja_1.9.0/ninja

GN_OPTIONS := \
  GN_INSTALL_PREFIX="$(MAPLE_ROOT)" \
  GN_BUILD_TYPE="$(BUILD_TYPE)" \
  HOST_ARCH=$(HOST_ARCH) \
  MIR_JAVA=$(MIR_JAVA)

.PHONY: default
default: install

.PHONY: maplegen
maplegen:
	$(call build_gn, ${GN_OPTIONS}, maplegen)

.PHONY: maplegendef
maplegendef: maplegen
	$(call build_gn, ${GN_OPTIONS}, aarch64isa_headers maplegendef)

.PHONY: maple
maple: maplegendef
	$(call build_gn, ${GN_OPTIONS}, maple)

.PHONY: irbuild
irbuild:
	$(call build_gn, ${GN_OPTIONS}, irbuild)

.PHONY: mplfe
mplfe:
	$(call build_gn, ${GN_OPTIONS}, mplfe)

.PHONY: install
install: maple
	$(shell cp -rf $(MAPLE_ROOT)/src/bin/java2jar $(MAPLE_ROOT)/output/bin/)
	$(shell cp -rf $(MAPLE_ROOT)/src/bin/jbc2mpl $(MAPLE_ROOT)/output/bin/)

.PHONY: clean
clean:
	@rm -rf output/

define build_gn
    mkdir -p ${INSTALL_DIR}; \
    $(GN) gen ${INSTALL_DIR} --args='$(1)' --export-compile-commands; \
    cd ${INSTALL_DIR}; \
    $(NINJA) -v $(2);
endef
