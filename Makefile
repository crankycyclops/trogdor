default:
	gcc -g -I/usr/include/lua5.1 src/*.c -lxml2 -ldstring -llua5.1 `pkg-config --cflags --libs glib-2.0` -o trogdor
