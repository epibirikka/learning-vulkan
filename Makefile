CC=gcc
SRC=./
INCLUDE=./
BUILD=./.build
SHADERS=./shaders

GLSLC=glslc

LIST_FLAGS=-I ./
LIST_FLAGS += -Wall
LIST_FLAGS += -mconsole
LIST_FLAGS += $(shell pkg-config vulkan --libs --cflags)
LIST_FLAGS += -g

LIST_OBJECTS=
LIST_OBJECTS += $(BUILD)/main.o
LIST_OBJECTS += $(BUILD)/window.o
LIST_OBJECTS += $(BUILD)/update.o
LIST_OBJECTS += $(BUILD)/app.o
LIST_OBJECTS += $(BUILD)/graphics.o
LIST_OBJECTS += $(BUILD)/validation.o
LIST_OBJECTS += $(BUILD)/debugger.o
LIST_OBJECTS += $(BUILD)/extension.o
LIST_OBJECTS += $(BUILD)/device.o
LIST_OBJECTS += $(BUILD)/logical.o
LIST_OBJECTS += $(BUILD)/surface.o
LIST_OBJECTS += $(BUILD)/swapchain.o
LIST_OBJECTS += $(BUILD)/shader.o
LIST_OBJECTS += $(BUILD)/pipeline.o
LIST_OBJECTS += $(BUILD)/command.o
LIST_OBJECTS += $(BUILD)/framebuffer.o
LIST_OBJECTS += $(BUILD)/order.o

LIST_COMPILED_SHADERS =
LIST_COMPILED_SHADERS += $(BUILD)/triangle.vert.spv
LIST_COMPILED_SHADERS += $(BUILD)/triangle.frag.spv

$(BUILD)/main: $(LIST_OBJECTS) $(LIST_COMPILED_SHADERS)
	$(CC) -o $@ $(LIST_OBJECTS) $(LIST_FLAGS) -s

$(BUILD)/triangle.vert.spv: $(SHADERS)/triangle.vert
	$(GLSLC) -o $@ $^

$(BUILD)/triangle.frag.spv: $(SHADERS)/triangle.frag
	$(GLSLC) -o $@ $^

$(BUILD)/main.o: $(SRC)/main.c
	$(CC) -c -o $@ $^ $(LIST_FLAGS)

$(BUILD)/app.o: $(SRC)/app.c
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

$(BUILD)/logical.o: $(SRC)/logical.c
	$(CC) -c -o $@ $^ $(LIST_FLAGS)

$(BUILD)/surface.o: $(SRC)/surface.c
	$(CC) -c -o $@ $^ $(LIST_FLAGS)

$(BUILD)/swapchain.o: $(SRC)/swapchain.c
	$(CC) -c -o $@ $^ $(LIST_FLAGS)

$(BUILD)/shader.o: $(SRC)/shader.c
	$(CC) -c -o $@ $^ $(LIST_FLAGS)

$(BUILD)/pipeline.o: $(SRC)/pipeline.c
	$(CC) -c -o $@ $^ $(LIST_FLAGS)

$(BUILD)/command.o: $(SRC)/command.c
	$(CC) -c -o $@ $^ $(LIST_FLAGS)

$(BUILD)/framebuffer.o: $(SRC)/framebuffer.c
	$(CC) -c -o $@ $^ $(LIST_FLAGS)

$(BUILD)/order.o: $(SRC)/order.c
	$(CC) -c -o $@ $^ $(LIST_FLAGS)

.PHONY: clean
clean:
	rm $(BUILD)/*.o
	rm $(BUILD)/*.exe
	rm $(BUILD)/*.spv

.PHONY: run
run:
	cd $(BUILD) && ./main
