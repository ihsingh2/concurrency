## Brief

Simulation of a cafe and an ice cream parlour, using multithreading.

## Methodology

- We create separate threads for each producer and consumer. A cyclic barrier is used to synchronize their (discrete) operations every second.

- We do this in multiple phases to allow the producer to virtually exhaust the entire second and leverage the customer tolerance completely.

- The execution is naturally modelled as a state diagram, represented using switch case here.

## Instructions

### Steps to build

```shell
mkdir build
cd build
cmake ..
make
```

### Steps to run

```shell
./build/cafe < input/cafe/<num>.txt
```

```shell
./build/parlour < input/parlour/<num>.txt
```

### Using macros

Change (append/remove) the following lines in [CMakeLists.txt](CMakeLists.txt).

```cmake
target_compile_definitions(cafe PRIVATE SIM PRIORITY)
```

```cmake
target_compile_definitions(parlour PRIVATE SIM PRIORITY)
```

- ``SIM`` - Simulate a round every second, in a real-time fashion.

- ``PRIORITY`` - Prioritize consumers based on the index in which they appear in the input.
