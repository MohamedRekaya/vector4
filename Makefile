# ============================================================
# Vector4 - Makefile
# ============================================================

CC = gcc
CFLAGS = -O2 -Wall -Wextra -std=c11
LDFLAGS = -lm
TARGET = vector4
SRC_DIR = src
BUILD_DIR = build
DATA_DIR = data
SCRIPT_DIR = scripts

# Detect OS and set image viewer
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	IMAGE_VIEWER = xdg-open
endif
ifeq ($(UNAME_S),Darwin)
	IMAGE_VIEWER = open
endif
ifeq ($(OS),Windows_NT)
	IMAGE_VIEWER = start
endif

# Source files
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/physics_engine.c

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# ============================================================
# Targets
# ============================================================

.PHONY: all run plot show plot-show clean help

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ============================================================
# Run
# ============================================================

run: $(TARGET)
	./$(TARGET) > $(DATA_DIR)/simulation.csv
	@echo "Simulation complete. Data saved to $(DATA_DIR)/simulation.csv"
	@echo "Run 'make plot' to visualize."

# ============================================================
# Plot
# ============================================================

plot:
	@if [ -f $(DATA_DIR)/simulation.csv ]; then \
		python3 $(SCRIPT_DIR)/plot.py $(DATA_DIR)/simulation.csv; \
	else \
		echo "No simulation data found. Run 'make run' first."; \
	fi

# ============================================================
# Show (Open the PNG)
# ============================================================

show:
	@if [ -f $(DATA_DIR)/simulation.png ]; then \
		$(IMAGE_VIEWER) $(DATA_DIR)/simulation.png; \
	else \
		echo "No plot found. Run 'make plot' first."; \
	fi

# ============================================================
# Plot + Show (One Command)
# ============================================================

plot-show: plot show

# ============================================================
# Clean
# ============================================================

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(DATA_DIR)/*.csv $(DATA_DIR)/*.png
	@echo "Cleaned."

# ============================================================
# Help
# ============================================================

help:
	@echo "Vector 4 - Makefile Commands:"
	@echo "  make          - Build the project"
	@echo "  make run      - Build and run simulation (generates CSV)"
	@echo "  make plot     - Generate plot from CSV"
	@echo "  make show     - Open the PNG image"
	@echo "  make plot-show - Plot and open in one command"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make help     - Show this help"
