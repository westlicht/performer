# This Makefile is used to setup the toolchain and application builds
# The application itself is configured and built using CMake

# Parts of this Makefile are based on https://github.com/cleanflight/cleanflight


# Directories
ROOT 		:= $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
ROOTABS		:= $(CURDIR)/$(ROOT)
ifndef TOOLS_DIR
TOOLS_DIR 	:= $(ROOT)/tools
endif
DL_DIR 		:= $(ROOT)/downloads
ARM_SDK_DIR ?= $(TOOLS_DIR)/gcc-arm-none-eabi-6-2017-q2-update

$(TOOLS_DIR):
	mkdir -p $@

$(DL_DIR):
	mkdir -p $@

# Determine host OS
UNAME := $(shell uname)

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


# ARM toolchain

.PHONY: arm_sdk_install

# Source: https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads
ARM_SDK_URL_BASE := https://developer.arm.com/-/media/Files/downloads/gnu-rm/6-2017q2/gcc-arm-none-eabi-6-2017-q2-update

ifdef LINUX
  ARM_SDK_URL := $(ARM_SDK_URL_BASE)-linux.tar.bz2
endif

ifdef MACOSX
  ARM_SDK_URL := $(ARM_SDK_URL_BASE)-mac.tar.bz2
endif

ARM_SDK_FILE := $(notdir $(ARM_SDK_URL))
GCC_REQUIRED_VERSION ?= 6.3.1
SDK_INSTALL_MARKER := $(ARM_SDK_DIR)/bin/arm-none-eabi-gcc-$(GCC_REQUIRED_VERSION)

arm_sdk_install: | $(TOOLS_DIR)
arm_sdk_install: arm_sdk_download $(SDK_INSTALL_MARKER)

$(SDK_INSTALL_MARKER):
	$(V1) tar -C $(TOOLS_DIR) -xjf "$(DL_DIR)/$(ARM_SDK_FILE)"

.PHONY: arm_sdk_download
arm_sdk_download: | $(DL_DIR)
arm_sdk_download: $(DL_DIR)/$(ARM_SDK_FILE)
$(DL_DIR)/$(ARM_SDK_FILE):
	$(V1) curl -L -k -o "$(DL_DIR)/$(ARM_SDK_FILE)" -z "$(DL_DIR)/$(ARM_SDK_FILE)" "$(ARM_SDK_URL)"

.PHONY: arm_sdk_clean
arm_sdk_clean:
	$(V1) [ ! -d "$(ARM_SDK_DIR)" ] || $(RM) -r $(ARM_SDK_DIR)
	$(V1) [ ! -d "$(DL_DIR)" ] || $(RM) -r $(DL_DIR)

# OpenOCD

OPENOCD_DIR       := $(TOOLS_DIR)/openocd
OPENOCD_BUILD_DIR := $(DL_DIR)/openocd-build

.PHONY: openocd_install

openocd_install: | $(DL_DIR) $(TOOLS_DIR)
openocd_install: OPENOCD_URL     := git://git.code.sf.net/p/openocd/code
openocd_install: OPENOCD_TAG     := v0.10.0
openocd_install: OPENOCD_OPTIONS := --enable-maintainer-mode --prefix="$(ROOTABS)/$(OPENOCD_DIR)" --enable-buspirate --enable-stlink --enable-ftdi

ifeq ($(UNAME), Darwin)
openocd_install: OPENOCD_OPTIONS := $(OPENOCD_OPTIONS) --disable-option-checking
endif

openocd_install: openocd_clean
	# download the source
	$(V0) @echo " DOWNLOAD     $(OPENOCD_URL) @ $(OPENOCD_TAG)"
	$(V1) [ ! -d "$(OPENOCD_BUILD_DIR)" ] || $(RM) -rf "$(OPENOCD_BUILD_DIR)"
	$(V1) mkdir -p "$(OPENOCD_BUILD_DIR)"
	$(V1) git clone --no-checkout $(OPENOCD_URL) "$(OPENOCD_BUILD_DIR)"
	$(V1) ( \
	  cd $(OPENOCD_BUILD_DIR) ; \
	  git checkout -q tags/$(OPENOCD_TAG) ; \
	)

	# build and install
	$(V0) @echo " BUILD        $(OPENOCD_DIR)"
	$(V1) mkdir -p "$(OPENOCD_DIR)"
	$(V1) ( \
	  cd $(OPENOCD_BUILD_DIR) ; \
	  ./bootstrap ; \
	  ./configure  $(OPENOCD_OPTIONS) ; \
	  $(MAKE) ; \
	  $(MAKE) install ; \
	)

	# delete the extracted source when we're done
	$(V1) [ ! -d "$(OPENOCD_BUILD_DIR)" ] || $(RM) -rf "$(OPENOCD_BUILD_DIR)"

.PHONY: openocd_clean
openocd_clean:
	$(V0) @echo " CLEAN        $(OPENOCD_DIR)"
	$(V1) [ ! -d "$(OPENOCD_DIR)" ] || $(RM) -r "$(OPENOCD_DIR)"

# Setup CMake projects

.PHONY: setup_stm32
setup_stm32: arm_sdk_install
	$(eval export PATH=$(ROOTABS)/$(ARM_SDK_DIR)/bin:$(ROOTABS)/$(OPENOCD_DIR)/bin:$(PATH))
	(mkdir -p $(ROOT)/build/stm32/debug && cd $(ROOT)/build/stm32/debug && cmake -DCMAKE_TOOLCHAIN_FILE=./cmake/arm.cmake -DCMAKE_BUILD_TYPE=Debug -DPLATFORM=stm32 ../../..)
	(mkdir -p $(ROOT)/build/stm32/release && cd $(ROOT)/build/stm32/release && cmake -DCMAKE_TOOLCHAIN_FILE=./cmake/arm.cmake -DCMAKE_BUILD_TYPE=Release -DPLATFORM=stm32 ../../..)

.PHONY: setup_sim
setup_sim:
	(mkdir -p build/sim/debug && cd build/sim/debug && cmake -DCMAKE_BUILD_TYPE=Debug -DPLATFORM=sim ../../..)
	(mkdir -p build/sim/release && cd build/sim/release && cmake -DCMAKE_BUILD_TYPE=Release -DPLATFORM=sim ../../..)

.PHONY: setup_www
setup_www:
	(mkdir -p build/sim/www && cd build/sim/www && cmake -DCMAKE_TOOLCHAIN_FILE="${EMSCRIPTEN}/cmake/Modules/Platform/Emscripten.cmake" -DCMAKE_BUILD_TYPE=Release -DPLATFORM=sim ../../..)
