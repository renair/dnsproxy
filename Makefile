OBJECT_FILES = main.o configurator.o tree.o

.PHONY: clean

dnsserver: $(OBJECT_FILES)
	gcc -o $@ $(OBJECT_FILES)

main.o: main.c
	gcc -o $@ -c main.c

configurator.o: configurator.c configurator.h
	gcc -o $@ -c configurator.c

tree.o: tree.c tree.h
	gcc -o $@ -c tree.c

clean:
	rm *.o
