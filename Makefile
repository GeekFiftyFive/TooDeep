ARGS = -std=c99 -Wall -O2

LIBS = -lSDL2 -lSDL2_image

tooDeep: obj/main.o obj/renderer.o obj/eventLoop.o
			cc $(ARGS) $(LIBS) -o tooDeep obj/main.o obj/renderer.o obj/eventLoop.o

obj/main.o: src/main.c
			cc -c src/main.c -o obj/main.o

obj/renderer.o: src/Renderer/renderer.c src/Renderer/renderer.h
			cc -c src/Renderer/renderer.c -o obj/renderer.o

obj/eventLoop.o: src/Events/eventLoop.c src/Events/eventLoop.h
			cc -c src/Events/eventLoop.c -o obj/eventLoop.o

clean:
	rm -rf obj/*