default:
	gcc -g -I/usr/include/libxml2 -I/usr/include/lua5.1 src/core/*.c src/standalone/*.c -lxml2 -ldstring -llua5.1 `pkg-config --cflags --libs glib-2.0` -o trogdor
