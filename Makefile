SRC_DIR := src
BUILD_DIR := build
CC := gcc

# ---- modes ----
CFLAGS_BASE := -Wall -Wextra -Wpedantic
CFLAGS_DEBUG := -O0 -g -fsanitize=address -fno-omit-frame-pointer

# default = debug (learning-safe)
CFLAGS := $(CFLAGS_BASE) $(CFLAGS_DEBUG)

# Default target: make <filename-without-.c>
%: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "Building $<"
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $<
	@echo "Running $@"
	@./$(BUILD_DIR)/$@

# Create build directory if missing
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean build artifacts
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
