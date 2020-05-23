tooDeep: obj/main.o 
			cc -o tooDeep obj/main.o

obj/main.o: src/main.c
			cc -c src/main.c -o obj/main.o