SRC_DIR := src
BUILD_DIR := build
CC := gcc

# ---- flags ----
CFLAGS_BASE := -Wall -Wextra -Wpedantic
CFLAGS_DEBUG := -O0 -g -fsanitize=address -fno-omit-frame-pointer
CFLAGS_RELEASE := -O3 -DNDEBUG

# ---- default: debug ----
CFLAGS := $(CFLAGS_BASE) $(CFLAGS_DEBUG)
BUILD_SUBDIR := debug

# ---- pattern: debug ----
%: $(SRC_DIR)/%.c | $(BUILD_DIR)/$(BUILD_SUBDIR)
	@echo "[DEBUG] Building $@"
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(BUILD_SUBDIR)/$@ $<
	@echo "[DEBUG] Running $@"
	@./$(BUILD_DIR)/$(BUILD_SUBDIR)/$@

# ---- pattern: release ----
%-release: CFLAGS := $(CFLAGS_BASE) $(CFLAGS_RELEASE)
%-release: BUILD_SUBDIR := release
%-release: $(SRC_DIR)/%.c | $(BUILD_DIR)/release
	@echo "[RELEASE] Building $*"
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/release/$* $<
	@echo "[RELEASE] Running $*"
	@./$(BUILD_DIR)/release/$*

# ---- directories ----
$(BUILD_DIR)/debug:
	mkdir -p $@

$(BUILD_DIR)/release:
	mkdir -p $@

# ---- clean ----
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
