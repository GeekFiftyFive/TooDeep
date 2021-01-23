LUA_INC = `pkg-config --cflags lua5.3`

LUA_LIB = `pkg-config --libs lua5.3`

ARGS = -std=c99 -Wall -O2 -g -D_DEFAULT_SOURCE=__STRICT_ANSI__ $(LUA_INC)

LIBS = -lSDL2 -lSDL2_image -lm -lcjson $(LUA_LIB)

tooDeep: obj/main.o\
		 obj/renderer.o\
		 obj/eventLoop.o\
		 obj/linkedList.o\
		 obj/hashMap.o\
		 obj/testHelper.o\
		 obj/linkedListTests.o\
		 obj/hashMapTests.o\
		 obj/resourceLoaderTests.o\
		 obj/fileIO.o\
		 obj/resourceLoader.o\
		 obj/jsonParser.o\
		 obj/jsonTests.o\
		 obj/logger.o\
		 obj/gameLoader.o\
		 obj/scene.o\
		 obj/entity.o\
		 obj/tuple.o\
		 obj/tupleTests.o\
		 obj/stringUtilsTests.o\
		 obj/stringUtils.o\
		 obj/luaFunctions.o\
		 obj/keyboardEvents.o\
		 obj/physics.o\
		 obj/boxCollision.o\
		 obj/camera.o\
		 obj/animation.o\
		 obj/entityState.o\
		 obj/luaCallback.o
			cc $(ARGS) -o tooDeep obj/main.o\
				obj/renderer.o\
				obj/eventLoop.o\
				obj/hashMap.o\
				obj/linkedList.o\
				obj/testHelper.o\
				obj/linkedListTests.o\
				obj/hashMapTests.o\
				obj/resourceLoaderTests.o\
				obj/fileIO.o\
				obj/resourceLoader.o\
				obj/jsonParser.o\
				obj/jsonTests.o\
				obj/logger.o\
				obj/gameLoader.o\
				obj/scene.o\
				obj/entity.o\
				obj/tuple.o\
				obj/tupleTests.o\
				obj/stringUtilsTests.o\
				obj/stringUtils.o\
				obj/luaFunctions.o\
				obj/keyboardEvents.o\
				obj/physics.o\
				obj/boxCollision.o\
				obj/camera.o\
				obj/animation.o\
				obj/entityState.o\
				obj/luaCallback.o\
				$(LIBS)

obj/main.o: src/main.c
			cc $(ARGS) -c src/main.c -o obj/main.o

obj/renderer.o: src/Renderer/renderer.c src/Renderer/renderer.h
			cc $(ARGS) -c src/Renderer/renderer.c -o obj/renderer.o

obj/eventLoop.o: src/Events/eventLoop.c src/Events/eventLoop.h
			cc $(ARGS) -c src/Events/eventLoop.c -o obj/eventLoop.o

obj/testHelper.o: src/Tests/testHelper.c src/Tests/testHelper.h
			cc $(ARGS) -c src/Tests/testHelper.c -o obj/testHelper.o

obj/linkedListTests.o: src/Tests/linkedListTests.c src/Tests/linkedListTests.h
			cc $(ARGS) -c src/Tests/linkedListTests.c -o obj/linkedListTests.o

obj/jsonTests.o: src/Tests/jsonTests.c src/Tests/jsonTests.h
			cc $(ARGS) -c src/Tests/jsonTests.c -o obj/jsonTests.o

obj/hashMapTests.o: src/Tests/hashMapTests.c src/Tests/hashMapTests.h
			cc $(ARGS) -c src/Tests/hashMapTests.c -o obj/hashMapTests.o

obj/resourceLoaderTests.o: src/Tests/resourceLoaderTests.c src/Tests/resourceLoaderTests.h
			cc $(ARGS) -c src/Tests/resourceLoaderTests.c -o obj/resourceLoaderTests.o

obj/linkedList.o: src/DataStructures/LinkedList/linkedList.c src/DataStructures/LinkedList/linkedList.h
			cc $(ARGS) -c src/DataStructures/LinkedList/linkedList.c -o obj/linkedList.o

obj/hashMap.o: src/DataStructures/HashMap/hashMap.c src/DataStructures/HashMap/hashMap.h
			cc $(ARGS) -c src/DataStructures/HashMap/hashMap.c -o obj/hashMap.o

obj/fileIO.o: src/IO/fileIO.c src/IO/fileIO.h
			cc $(ARGS) -c src/IO/fileIO.c -o obj/fileIO.o

obj/resourceLoader.o: src/IO/resourceLoader.c src/IO/resourceLoader.h
			cc $(ARGS) -c src/IO/resourceLoader.c -o obj/resourceLoader.o

obj/logger.o: src/IO/logger.c src/IO/logger.h
			cc $(ARGS) -c src/IO/logger.c -o obj/logger.o

obj/jsonParser.o: src/JSON/jsonParser.c src/JSON/jsonParser.h
			cc $(ARGS) -c src/JSON/jsonParser.c -o obj/jsonParser.o

obj/gameLoader.o: src/IO/gameLoader.c src/IO/gameLoader.h
			cc $(ARGS) -c src/IO/gameLoader.c -o obj/gameLoader.o

obj/scene.o: src/State/Scene/scene.c src/State/Scene/scene.h
			cc $(ARGS) -c src/State/Scene/scene.c -o obj/scene.o

obj/entity.o: src/State/Entity/entity.c src/State/Entity/entity.h
			cc $(ARGS) -c src/State/Entity/entity.c -o obj/entity.o

obj/entityState.o: src/State/EntityState/entityState.c src/State/EntityState/entityState.h
			cc $(ARGS) -c src/State/EntityState/entityState.c -o obj/entityState.o

obj/tuple.o: src/DataStructures/Tuple/tuple.c src/DataStructures/Tuple/tuple.h
			cc $(ARGS) -c src/DataStructures/Tuple/tuple.c -o obj/tuple.o

obj/tupleTests.o: src/Tests/tupleTests.c src/Tests/tupleTests.h
			cc $(ARGS) -c src/Tests/tupleTests.c -o obj/tupleTests.o

obj/stringUtilsTests.o: src/Tests/stringUtilsTests.c src/Tests/stringUtilsTests.h
			cc $(ARGS) -c src/Tests/stringUtilsTests.c -o obj/stringUtilsTests.o

obj/stringUtils.o: src/Utils/stringUtils.c src/Utils/stringUtils.h
			cc $(ARGS) -c src/Utils/stringUtils.c -o obj/stringUtils.o

obj/luaFunctions.o: src/Scripting/luaFunctions.c src/Scripting/luaFunctions.h
			cc $(ARGS) -c src/Scripting/luaFunctions.c -o obj/luaFunctions.o

obj/luaCallback.o: src/Scripting/luaCallback.c src/Scripting/luaCallback.h
			cc $(ARGS) -c src/Scripting/luaCallback.c -o obj/luaCallback.o

obj/keyboardEvents.o: src/Events/keyboardEvents.c src/Events/keyboardEvents.h
			cc $(ARGS) -c src/Events/keyboardEvents.c -o obj/keyboardEvents.o

obj/physics.o: src/Physics/physics.c src/Physics/physics.h
			cc $(ARGS) -c src/Physics/physics.c -o obj/physics.o

obj/boxCollision.o: src/Physics/boxCollision.c src/Physics/boxCollision.h
			cc $(ARGS) -c src/Physics/boxCollision.c -o obj/boxCollision.o

obj/camera.o: src/Renderer/camera.c src/Renderer/camera.h
			cc $(ARGS) -c src/Renderer/camera.c -o obj/camera.o

obj/animation.o: src/Renderer/animation.c src/Renderer/animation.h
			cc $(ARGS) -c src/Renderer/animation.c -o obj/animation.o

clean:
	rm -rf obj/* && rm tooDeep