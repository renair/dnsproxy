OBJECT_FILES = main.o

dnsserver: $(OBJECT_FILES)
	gcc -o $@ $(OBJECT_FILES)

main.o: main.c
	gcc -o $@ -c main.c
