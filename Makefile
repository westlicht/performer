# This Makefile is used to setup the toolchain and application builds
# The application itself is configured and built using CMake

# Parts of this Makefile are based on https://github.com/cleanflight/cleanflight


# Directories
ROOT		:= $(CURDIR)/$(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
TOOLS_DIR 	?= $(ROOT)/tools
DL_DIR 		:= $(ROOT)/downloads

$(TOOLS_DIR):
	mkdir -p $@

$(DL_DIR):
	mkdir -p $@

# Determine host OS
UNAME := $(shell uname)
ARCH := $(shell uname -m)

# Linux
ifeq ($(UNAME), Linux)
  OSFAMILY := linux
  LINUX := 1
endif

# Mac OSX
ifeq ($(UNAME), Darwin)
  OSFAMILY := macosx
  MACOSX := 1
endif

# Report error if no OS was determined
ifndef OSFAMILY
  $(info uname reports $(UNAME))
  $(info uname -m reports $(ARCH))
  $(error failed to detect operating system)
endif

# Tools

.PHONY: tools_install
tools_install: arm_sdk_install openocd_install

.PHONY: tools_clean
tools_clean: arm_sdk_clean openocd_clean

# ARM toolchain

.PHONY: arm_sdk_install

# Source: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
ARM_SDK_URL_BASE := https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1

ifdef LINUX
  ARM_SDK_URL := $(ARM_SDK_URL_BASE)-$(ARCH)-arm-none-eabi.tar.xz
endif

ifdef MACOSX
  ARM_SDK_URL := $(ARM_SDK_URL_BASE)-darwin-$(ARCH)-arm-none-eabi.tar.xz
endif

ARM_SDK_FILE := $(notdir $(ARM_SDK_URL))
ARM_SDK_DIR := $(TOOLS_DIR)/gcc-arm-none-eabi
GCC_REQUIRED_VERSION := 6.3.1
ARM_SDK_INSTALL_MARKER := $(ARM_SDK_DIR)/bin/arm-none-eabi-gcc-$(GCC_REQUIRED_VERSION)

arm_sdk_install: | $(TOOLS_DIR)
arm_sdk_install: arm_sdk_download $(ARM_SDK_INSTALL_MARKER)

$(ARM_SDK_INSTALL_MARKER):
	$(V1) mkdir -p $(ARM_SDK_DIR)
	$(V1) tar -C $(ARM_SDK_DIR) --strip-components=1 -xaf "$(DL_DIR)/$(ARM_SDK_FILE)"

.PHONY: arm_sdk_download
arm_sdk_download: | $(DL_DIR)
arm_sdk_download: $(DL_DIR)/$(ARM_SDK_FILE)
$(DL_DIR)/$(ARM_SDK_FILE):
	$(V1) curl -L -k -o "$(DL_DIR)/$(ARM_SDK_FILE)" -z "$(DL_DIR)/$(ARM_SDK_FILE)" "$(ARM_SDK_URL)"

.PHONY: arm_sdk_clean
arm_sdk_clean:
	$(V1) [ ! -d "$(ARM_SDK_DIR)" ] || $(RM) -r $(ARM_SDK_DIR)
	$(V1) [ ! -f "$(DL_DIR)/$(ARM_SDK_FILE)" ] || $(RM) $(DL_DIR)/$(ARM_SDK_FILE)

# OpenOCD

.PHONY: openocd_install

OPENOCD_URL := https://downloads.sourceforge.net/project/openocd/openocd/0.10.0/openocd-0.10.0.tar.bz2
OPENOCD_FILE := $(notdir $(OPENOCD_URL))
OPENOCD_DIR := $(TOOLS_DIR)/openocd
OPENOCD_BUILD_DIR := $(TOOLS_DIR)/openocd-build
OPENOCD_INSTALL_MARKER := $(OPENOCD_DIR)/bin/openocd
OPENOCD_OPTIONS := --enable-maintainer-mode --prefix="$(OPENOCD_DIR)" --enable-buspirate --enable-stlink --enable-ftdi

openocd_install: | $(TOOLS_DIR)
openocd_install: openocd_download $(OPENOCD_INSTALL_MARKER)

$(OPENOCD_INSTALL_MARKER):
	$(V1) mkdir -p $(OPENOCD_BUILD_DIR)
	$(V1) tar -C $(OPENOCD_BUILD_DIR) --strip-components=1 -xjf "$(DL_DIR)/$(OPENOCD_FILE)"
	# build and install
	$(V1) mkdir -p $(OPENOCD_DIR)
	$(V1) ( \
	  cd $(OPENOCD_BUILD_DIR) ; \
	  ./bootstrap ; \
	  ./configure  $(OPENOCD_OPTIONS) ; \
	  $(MAKE) -j ; \
	  $(MAKE) install ; \
	)
	$(V1) $(RM) -r $(OPENOCD_BUILD_DIR)


.PHONY: openocd_download
openocd_download: | $(DL_DIR)
openocd_download: $(DL_DIR)/$(OPENOCD_FILE)
$(DL_DIR)/$(OPENOCD_FILE):
	$(V1) curl -L -k -o "$(DL_DIR)/$(OPENOCD_FILE)" -z "$(DL_DIR)/$(OPENOCD_FILE)" "$(OPENOCD_URL)"

.PHONY: openocd_clean
openocd_clean:
	$(V1) [ ! -d "$(OPENOCD_DIR)" ] || $(RM) -r $(OPENOCD_DIR)
	$(V1) [ ! -f "$(DL_DIR)/$(OPENOCD_FILE)" ] || $(RM) $(DL_DIR)/$(OPENOCD_FILE)

# Setup CMake projects

.PHONY: setup_stm32
setup_stm32: arm_sdk_install
	$(eval export PATH=$(ARM_SDK_DIR)/bin:$(OPENOCD_DIR)/bin:$(PATH))
	(mkdir -p $(ROOT)/build/stm32/debug && cd $(ROOT)/build/stm32/debug && cmake -DCMAKE_TOOLCHAIN_FILE=./cmake/arm.cmake -DCMAKE_BUILD_TYPE=Debug -DPLATFORM=stm32 ../../..)
	(mkdir -p $(ROOT)/build/stm32/release && cd $(ROOT)/build/stm32/release && cmake -DCMAKE_TOOLCHAIN_FILE=./cmake/arm.cmake -DCMAKE_BUILD_TYPE=Release -DPLATFORM=stm32 ../../..)

.PHONY: setup_sim
setup_sim:
	(mkdir -p build/sim/debug && cd build/sim/debug && cmake -DCMAKE_BUILD_TYPE=Debug -DPLATFORM=sim ../../..)
	(mkdir -p build/sim/release && cd build/sim/release && cmake -DCMAKE_BUILD_TYPE=Release -DPLATFORM=sim ../../..)

.PHONY: setup_www
setup_www:
	(mkdir -p build/sim/www && cd build/sim/www && cmake -DCMAKE_TOOLCHAIN_FILE="${EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" -DCMAKE_BUILD_TYPE=Release -DPLATFORM=sim ../../..)

# Deployment

.PHONY: deploy
deploy:
	./scripts/deploy

.PHONY: deploy-simulator
deploy-simulator:
	./scripts/deploy-simulator
