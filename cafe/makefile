FLAGS = -Werror -Wall -g3
COMMON = ../common

main: $(COMMON)/* *
	gcc $(FLAGS) $(COMMON)/*.c *.c -o cafe

sim: ../common/* *
	gcc -DSIM $(FLAGS) $(COMMON)/*.c *.c -o cafe

clean:
	rm -f cafe
