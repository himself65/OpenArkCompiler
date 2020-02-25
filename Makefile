#
# Makefile for OpenArkCompiler
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

.PHONY: mapleall
mapleall:
	$(call build_gn, ${GN_OPTIONS}, maple)

.PHONY: install
install: mapleall
	$(shell cp -rf $(MAPLE_ROOT)/src/bin/java2jar $(MAPLE_ROOT)/output/bin/)
	$(shell cp -rf $(MAPLE_ROOT)/src/bin/jbc2mpl $(MAPLE_ROOT)/output/bin/)
	$(shell cp -rf $(MAPLE_ROOT)/src/bin/mplcg $(MAPLE_ROOT)/output/bin/)

.PHONY: clean
clean:
	@rm -rf output/

define build_gn
    mkdir -p ${INSTALL_DIR}; \
    $(GN) gen ${INSTALL_DIR} --args='$(1)' --export-compile-commands; \
    cd ${INSTALL_DIR}; \
    $(NINJA) -v $(2) | tee ${MAPLE_ROOT}/build/logs/$(shell date +"%Y-%m-%d-%H-%M-%S").log;
endef
