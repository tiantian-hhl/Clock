CC=gcc
CFLAGS=-Wall -g
LDFLAGS=-lm

SRC_DIR=src
BUILD_DIR=build
TARGET=showtime
PROJECT_NAME = showtime
VERSION = 1.0.0

.PHONY: all install clean source

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(SRC_DIR)/main.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

install:
	@echo "Creating target directory..."
	@mkdir -p "$(dir $(PREFIX)/$(TARGET))" || true
	install -m 755 $(BUILD_DIR)/$(TARGET) "$(PREFIX)"

clean:
	rm -rf $(BUILD_DIR)/*

source:
	rm -rf /tmp/$(PROJECT_NAME)-source-tmp
	mkdir -p /tmp/$(PROJECT_NAME)-source-tmp/$(PROJECT_NAME)-$(VERSION)
	cp -r * /tmp/$(PROJECT_NAME)-source-tmp/$(PROJECT_NAME)-$(VERSION)/
	cd /tmp/$(PROJECT_NAME)-source-tmp && \
		tar -czf ../$(PROJECT_NAME)-$(VERSION).tar.gz $(PROJECT_NAME)-$(VERSION)
	mv /tmp/$(PROJECT_NAME)-$(VERSION).tar.gz .

help:
	@echo "Usage:"
	@echo "  make           Build the project"
	@echo "  make install   Install the project"
	@echo "  make clean     Clean build files"
	@echo "  make source    Create a source distribution"
