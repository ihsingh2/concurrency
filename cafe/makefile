FLAGS = -Werror -Wall -g3
COMMON = ../common

main: $(COMMON)/* *
	gcc $(FLAGS) $(COMMON)/*.c *.c -o cafe

sim: $(COMMON)/* *
	gcc -DSIM $(FLAGS) $(COMMON)/*.c *.c -o cafe

priority: $(COMMON)/* *
	gcc -DPRIORITY $(FLAGS) $(COMMON)/*.c *.c -o cafe

prsim: $(COMMON)/* *
	gcc -DPRIORITY -DSIM $(FLAGS) $(COMMON)/*.c *.c -o cafe

clean:
	rm -f cafe
