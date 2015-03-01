
CC = gcc
CFLAGS = -Isrc -DVERIFY_BTREE -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast
SRC_DIR = src
SOURCES = $(SRC_DIR)/rbtree.c
OUT = build

ifdef DTRACE
	CFLAGS += -DTRACE
endif

.PHONY: clean

%:
	$(CC) $^ $(CFLAGS) $(SOURCES) $(SRC_DIR)/$*.c -o $(OUT)/$*

clean:
	rm -f build/*

clean-%:
	rm -f build/$*
