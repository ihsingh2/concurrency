## Cafe Sim

Source: https://github.com/karthikv1392/cs3301_osn/blob/master/mini-projects/mp3.md

You have been given the task to simulate the operation of a small cafe. The cafe owner wants to ensure that customers can get their coffee efficiently. In your report, you will provide insights based on the simulation results.

#### During the simulation:
- There are `K` coffee types.
- Each coffee type `c` takes some time `t_c` to prepare.
- The cafe has `B` baristas.

- There are `N` customers waiting to get their coffee.
- Each customer `i` arrives at some time `t_arr_i`, and orders only one coffee `x`.
- Each customer `i` arrives at some tolerance time `tol_i`, after which their patience runs out and they will instantly leave the cafe (bad).

#### Assumptions:
- Simulation begins from 0 seconds.
- The cafe has unlimited ingredients.
- If a customer arrives at time `t`, they place the order at time `t`, and the coffee starts getting prepared at time `t+1`.
- If a customer arrives at time `t`, and they have tolerance time `tol` => they collect their order only if it gets done on or before `t + tol`.
- Once an order is completed, the customer picks it up and leaves instantaneously.
- If a customer was already waiting, once a barista finishes their previous order, say at time `t`, they can start making the order of the waiting customer at time `t+1`.
- The cafe has infinite seating capacity. 

Your simulation should utilize multi-threading concepts and avoid potential issues like deadlocks and busy waiting. Implement the problem using semaphores and mutex locks to ensure thread safety. Make sure to colour code your output using the specified colours. 

#### Input format (all space separated):
The first line contains  `B K N`
The next `K lines` contain `c` `t_c`
The next `N lines` contain `i` `x` `t_arr_i` `tol_i`

#### Output format:
- When a customer `c` arrives, print [white colour] `Customer c arrives at t second(s)`
- When a customer `c` places their order, print [yellow colour] `Customer c orders an espresso`

- When a barista `b` begins preparing the order of customer `c`, print [cyan colour] `Barista b begins preparing the order of customer c at t second(s)`
- When a barista `b` successfully complete the order of customer `c`, print [blue colour] `Barista b successfully completes the order of customer c`

- If a customer successfully gets their order, print [green colour] `Customer c leaves with their order at t second(s)`
- If a customer's patience runs out, print [red colour] `Customer c leaves without their order at t second(s)`

#### Example:

Input:

```
2 2 3
Espresso 3
Cappuccino 10
1 Cappuccino 0 15
2 Espresso 3 6
3 Espresso 3 5
```

Output:

```
Customer 1 arrives at 0 second(s)
Customer 1 orders a Cappuccino
Barista 1 begins preparing the order of customer 1 at 1 second(s)
Customer 2 arrives at 3 second(s)
Customer 2 orders an Espresso
Customer 3 arrives at 3 second(s)
Customer 3 orders an Espresso
Barista 2 begins preparing the order of customer 2 at 4 second(s)
Barista 2 completes the order of customer 2 at 7 second(s)
Customer 2 leaves with their order at 7 second(s)
Barista 2 begins preparing the order of customer 3 at 8 second(s)
Customer 3 leaves without their order at 9 second(s)
Barista 1 completes the order of customer 1 at 11 second(s)
Barista 2 completes the order of customer 2 at 11 second(s)
Customer 1 leaves with their order at 11 second(s)

1 coffee wasted
```
