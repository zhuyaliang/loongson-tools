CC = gcc  
MAINC =loongson-firmware.c  main.c  tools-window.c loongson-utils.c loongson-fan.c
EXEC = style
CFLAGS = `pkg-config --cflags --libs gtk+-3.0`
main: 
	$(CC)  -g $(MAINC)  -o $(EXEC) $(CFLAGS)
clean:
	rm $(EXEC) -rf
