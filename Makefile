SOURCES_DIR 		:= src
OUTPUT_DIR 			:= bin
BUILD_TYPE 			?= debug

CELL_MK_DIR 		?= 	$(SCE_PS3_ROOT)/samples/mk
include 			$(CELL_MK_DIR)/sdk.makedef.mk

PPU_SRCS 			:= $(shell find $(SOURCES_DIR) -name "*.c")
PPU_PRX_LDFLAGS 	+= $(PRX_LDFLAGS_EXTRA)
PPU_PRX_TARGET 		=  $(OUTPUT_DIR)/$(BUILD_TYPE)/$(notdir $(CURDIR)).prx
PPU_PRX_LDLIBS 		+= -lc -lfs_stub

ifeq ($(BUILD_TYPE),release)
PPU_CFLAGS 			= -O2
PRX_LDFLAGS_EXTRA	= -s --stripe-unused-data
else
PPU_CFLAGS 			= -O0
PRX_LDFLAGS_EXTRA	= 
endif

PPU_CFLAGS 			+= -std=c99 -ffunction-sections -fdata-sections -fno-builtin-printf -nodefaultlibs -Wno-shadow -Wno-unused-parameter

CLEANFILES 			= $(PRX_DIR)/$(PPU_SPRX_TARGET)

include 			$(CELL_MK_DIR)/sdk.target.mk

rebuild:
	$(MAKE) --no-print-directory clean
	$(MAKE) --no-print-directory all

clean:
	@echo "# removing $(OUTPUT_DIR)/$(BUILD_TYPE) if it is empty.";
	@if [ -d $(OUTPUT_DIR)/$(BUILD_TYPE) ]; then \
		rm -r $(OUTPUT_DIR)/$(BUILD_TYPE); \
	fi

	@rmdir $(OUTPUT_DIR) ||:

release:
	$(MAKE) rebuild BUILD_TYPE=release


		


