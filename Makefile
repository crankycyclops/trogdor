default:
	gcc src/*.c -lxml2 -ldstring `pkg-config --cflags --libs glib-2.0`
