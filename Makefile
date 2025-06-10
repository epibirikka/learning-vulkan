CC=gcc
SRC=./
INCLUDE=./
BUILD=./.build

LIST_FLAGS=-I ./
LIST_FLAGS += -Wall
LIST_FLAGS += -mconsole
LIST_FLAGS += $(shell pkg-config vulkan --libs --cflags)
LIST_FLAGS += -g

LIST_OBJECTS=
LIST_OBJECTS += $(BUILD)/main.o
LIST_OBJECTS += $(BUILD)/window.o
LIST_OBJECTS += $(BUILD)/update.o
LIST_OBJECTS += $(BUILD)/graphics.o
LIST_OBJECTS += $(BUILD)/validation.o
LIST_OBJECTS += $(BUILD)/debugger.o
LIST_OBJECTS += $(BUILD)/extension.o
LIST_OBJECTS += $(BUILD)/device.o

$(BUILD)/main: $(LIST_OBJECTS)
	$(CC) -o $@ $^ $(LIST_FLAGS) -s

$(BUILD)/main.o: $(SRC)/main.c
	$(CC) -c -o $@ $^ $(LIST_FLAGS)

$(BUILD)/graphics.o: $(SRC)/graphics.c
	$(CC) -c -o $@ $^ $(LIST_FLAGS)

$(BUILD)/window.o: $(SRC)/window.c
	$(CC) -c -o $@ $^ $(LIST_FLAGS)

$(BUILD)/update.o: $(SRC)/update.c
	$(CC) -c -o $@ $^ $(LIST_FLAGS)

$(BUILD)/validation.o: $(SRC)/validation.c $(SRC)/validation.h
	$(CC) -c -o $@ $< $(LIST_FLAGS)

$(BUILD)/debugger.o: $(SRC)/debugger.c
	$(CC) -c -o $@ $^ $(LIST_FLAGS)

$(BUILD)/extension.o: $(SRC)/extension.c
	$(CC) -c -o $@ $^ $(LIST_FLAGS)

$(BUILD)/device.o: $(SRC)/device.c
	$(CC) -c -o $@ $^ $(LIST_FLAGS)

.PHONY: clean
clean:
	rm $(BUILD)/*.o
	rm $(BUILD)/*.exe
