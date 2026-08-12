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
UI_DIR = ui

# SDL2 flags
SDL_CFLAGS = $(shell pkg-config --cflags sdl2)
SDL_LIBS = $(shell pkg-config --libs sdl2) -lSDL2_gfx

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

# Viewer source files (main.c removed)
VIEWER_SRCS = $(UI_DIR)/viewer.c \
              $(UI_DIR)/graphics.c

VIEWER_TARGET = cartpole_viewer

# ============================================================
# Targets
# ============================================================

.PHONY: all run plot show plot-show clean help viewer viewer-run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ============================================================
# Viewer (SDL2 Graphics)
# ============================================================

viewer: $(VIEWER_TARGET)

$(VIEWER_TARGET): $(VIEWER_SRCS)
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -D_GNU_SOURCE -o $@ $^ $(SDL_LIBS) $(LDFLAGS)

viewer-run: $(VIEWER_TARGET)
	./$(VIEWER_TARGET) $(DATA_DIR)/simulation.csv

# ============================================================
# Run
# ============================================================

run: $(TARGET)
	./$(TARGET) > $(DATA_DIR)/simulation.csv
	@echo "Simulation complete. Data saved to $(DATA_DIR)/simulation.csv"
	@echo "Run 'make viewer-run' to visualize."

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
	rm -rf $(BUILD_DIR) $(TARGET) $(VIEWER_TARGET) $(DATA_DIR)/*.csv $(DATA_DIR)/*.png
	@echo "Cleaned."

# ============================================================
# Help
# ============================================================

help:
	@echo "Vector4 - Makefile Commands:"
	@echo ""
	@echo "  make            - Build the physics simulation"
	@echo "  make run        - Build and run simulation (generates CSV)"
	@echo "  make plot       - Generate plot from CSV"
	@echo "  make show       - Open the PNG image"
	@echo "  make plot-show  - Plot and open in one command"
	@echo ""
	@echo "  make viewer     - Build the SDL2 visualization viewer"
	@echo "  make viewer-run - Build and run viewer with current CSV"
	@echo ""
	@echo "  make clean      - Remove build artifacts"
	@echo "  make help       - Show this help"
