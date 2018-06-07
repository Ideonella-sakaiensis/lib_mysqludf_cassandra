# lib_mysqludf_cassandra dependency Makefile

LIBUV_MODULE_VER      ?=
CPP_DRIVER_MODULE_VER ?=
CJSON_MODULE_VER      ?=
INSTALL_PATH          ?=
LINKER_FLAGS          ?=
INSTALL_OPTIONS=
ifneq ("","$(INSTALL_PATH)")
  INSTALL_OPTIONS=PREFIX=$(INSTALL_PATH)
endif

CCCOLOR  ="\033[34m"
LINKCOLOR="\033[34;1m"
SRCCOLOR ="\033[33m"
BINCOLOR ="\033[37;1m"
MAKECOLOR="\033[32;1m"
ENDCOLOR ="\033[0m"




default:
	@echo "Explicit target required"

.PHONY: default



libuv:
ifeq ("","$(wildcard libuv)")
  ifeq ("","$(LIBUV_MODULE_VER)")
	$(error required LIBUV_MODULE_VER)
  endif
	sh download-libuv.sh $(LIBUV_MODULE_VER)
endif
	@printf '%b %b\n' $(MAKECOLOR)MAKE$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR)
	cd libuv  &&  (sh autogen.sh) && (./configure --prefix=$(INSTALL_PATH))
	@printf '%b %b\n' $(LINKCOLOR)INSTALL$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR)
	cd libuv && $(MAKE) install

.PHONY: libuv


cpp-driver:
ifeq ("","$(wildcard cpp-driver)")
  ifeq ("","$(CPP_DRIVER_MODULE_VER)")
	$(error required CPP_DRIVER_MODULE_VER)
  endif
	sh download-cpp-driver.sh $(CPP_DRIVER_MODULE_VER)
endif
	@printf '%b %b\n' $(MAKECOLOR)MAKE$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR)
  ifneq ("","$(wildcard cpp-driver/build)")
	rm -rf cpp-driver/build
  endif
	mkdir cpp-driver/build
	cd cpp-driver/build \
    && cmake .. -DCMAKE_INSTALL_PREFIX:PATH=$(INSTALL_PATH) \
                -DCMAKE_INSTALL_LIBDIR:STRING=lib \
                -DLIBUV_INCLUDE_DIR:PATH=$(INSTALL_PATH)/include \
                -DLIBUV_LIBRARY:FILEPATH=$(INSTALL_PATH)/lib/libuv.so \
                -DCASS_INSTALL_PKG_CONFIG:BOOL=OFF \
                -DCASS_USE_LIBSSH2:BOOL=OFF \
                -DCMAKE_SHARED_LINKER_FLAGS:STRING=' $(LINKER_FLAGS)' \
    && $(MAKE)
	@printf '%b %b\n' $(LINKCOLOR)INSTALL$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR)
	cd cpp-driver/build && $(MAKE) install

.PHONY: cpp-driver



cJSON:
ifeq ("","$(wildcard cJSON)")
  ifeq ("","$(CJSON_MODULE_VER)")
	$(error required CJSON_MODULE_VER)
  endif
	sh download-cJSON.sh $(CJSON_MODULE_VER)
endif
	@printf '%b %b\n' $(MAKECOLOR)MAKE$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR)
	cd cJSON && $(MAKE)
	@printf '%b %b\n' $(LINKCOLOR)INSTALL$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR)
	cd cJSON && $(MAKE) install $(INSTALL_OPTIONS)

.PHONY: cJSON



distclean:
	-rm -rf cpp-driver libuv cJSON

.PHONY: distclean
