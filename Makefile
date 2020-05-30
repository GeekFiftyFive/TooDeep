ARGS = -std=c99 -Wall -O2

LIBS = -lSDL2 -lSDL2_image

tooDeep: obj/main.o obj/renderer.o obj/eventLoop.o obj/testHelper.o obj/linkedListTests.o
			cc $(ARGS) $(LIBS) -o tooDeep obj/main.o obj/renderer.o obj/eventLoop.o obj/testHelper.o obj/linkedListTests.o

obj/main.o: src/main.c
			cc -c src/main.c -o obj/main.o

obj/renderer.o: src/Renderer/renderer.c src/Renderer/renderer.h
			cc -c src/Renderer/renderer.c -o obj/renderer.o

obj/eventLoop.o: src/Events/eventLoop.c src/Events/eventLoop.h
			cc -c src/Events/eventLoop.c -o obj/eventLoop.o

obj/testHelper.o: src/Tests/testHelper.c src/Tests/testHelper.h
			cc -c src/Tests/testHelper.c -o obj/testHelper.o

obj/linkedListTests.o: src/Tests/linkedListTests.c src/Tests/linkedListTests.h
			cc -c src/Tests/linkedListTests.c -o obj/linkedListTests.o

clean:
	rm -rf obj/*