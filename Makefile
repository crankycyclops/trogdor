default:
	gcc -g src/*.c -lxml2 -ldstring `pkg-config --cflags --libs glib-2.0` -o trogdor
