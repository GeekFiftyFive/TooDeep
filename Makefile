tooDeep: obj/main.o obj/renderer.o
			cc -o tooDeep obj/main.o obj/renderer.o

obj/main.o: src/main.c
			cc -c src/main.c -o obj/main.o

obj/renderer.o: src/Renderer/renderer.c src/Renderer/renderer.h
			cc -c src/Renderer/renderer.c -o obj/renderer.o

clean:
	rm -rf obj/*