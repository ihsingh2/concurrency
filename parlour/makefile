FLAGS = -Werror -Wall -g3
COMMON = ../common

main: $(COMMON)/* *
	gcc $(FLAGS) $(COMMON)/*.c *.c -o parlour

sim: $(COMMON)/* *
	gcc -DSIM $(FLAGS) $(COMMON)/*.c *.c -o parlour

priority: $(COMMON)/* *
	gcc -DPRIORITY $(FLAGS) $(COMMON)/*.c *.c -o parlour

prsim: $(COMMON)/* *
	gcc -DPRIORITY -DSIM $(FLAGS) $(COMMON)/*.c *.c -o parlour

clean:
	rm -f parlour
