CFLAGS = `pkg-config --cflags glfw3` -g -Wall --std=gnu99
LFLAGS = `pkg-config --static --libs glfw3`

serial_render: src/serial_render.c
	cc $(CFLAGS) -o serial_render src/serial_render.c $(LFLAGS)
