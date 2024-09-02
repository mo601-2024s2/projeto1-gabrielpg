all: main.elf

elf_parser.o: elf_parser.c
	gcc elf_parser.c -c -o elf_parser.o

elf_loader.o: elf_loader.c
	gcc elf_loader.c -c -o elf_loader.o

log_generator.o: log_generator.c
	gcc log_generator.c -c -o log_generator.o

my_types.o: my_types.c
	gcc my_types.c -c -o my_types.o

simulator.o: simulator.c
	gcc simulator.c -c -o simulator.o

main.o: main.c
	gcc main.c -c -o main.o

main.elf: main.o elf_parser.o elf_loader.o log_generator.o my_types.o simulator.o
	gcc main.o elf_parser.o elf_loader.o log_generator.o my_types.o simulator.o -o main.elf

clean:
	rm -rf test_bin test_log
	rm *.elf *.o
