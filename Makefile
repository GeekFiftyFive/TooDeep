ARGS = -std=c99 -Wall -O2

LIBS = -lSDL2 -lSDL2_image -lm -lcJSON

tooDeep: obj/main.o obj/renderer.o obj/eventLoop.o obj/linkedList.o obj/testHelper.o obj/linkedListTests.o obj/fileIO.o obj/jsonParser.o
			cc $(ARGS) $(LIBS) -o tooDeep obj/main.o obj/renderer.o obj/eventLoop.o obj/linkedList.o obj/testHelper.o obj/linkedListTests.o obj/fileIO.o obj/jsonParser.o

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

obj/linkedList.o: src/DataStructures/LinkedList/linkedList.c src/DataStructures/LinkedList/linkedList.h
			cc -c src/DataStructures/LinkedList/linkedList.c -o obj/linkedList.o

obj/fileIO.o: src/IO/fileIO.c src/IO/fileIO.h
			cc -c src/IO/fileIO.c -o obj/fileIO.o

obj/jsonParser.o: src/JSON/jsonParser.c src/JSON/jsonParser.h
			cc -c src/JSON/jsonParser.c -o obj/jsonParser.o

clean:
	rm -rf obj/*