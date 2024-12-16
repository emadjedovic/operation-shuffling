**Operation Shuffling** is a technique used to obfuscate algorithms by randomly shuffling the sequence of operations, while maintaining the algorithm's correctness. This method can be employed to prevent reverse engineering and tampering in cybersecurity, intellectual property protection, and software development.

## Problem Statement

Given a series of operations in an algorithm, we aim to shuffle the operations such that the algorithm still functions the same way. Each operation has input and output variables. For example:

```plaintext
a, b = Operation(c, d, e)
d, f = Operation(b, h)
y, g = Operation(u, v)
b = Operation(d, r)
```
can result in:
```plaintext
y, g = Operation(u, v)
a, b = Operation(c, d, e)
d, f = Operation(b, h)
b = Operation(d, r)
```
or:
```plaintext
a, b = Operation(c, d, e)
d, f = Operation(b, h)
b = Operation(d, r)
y, g = Operation(u, v)
```

In the obfuscated version, operations are shuffled while preserving the dependencies between them, ensuring the algorithm works the same.

## Approach

This problem is modeled as a **dependency graph** problem:
- **Nodes**: Each operation is a node.
- **Edges**: Directed edges represent dependencies—if operation `A` outputs a variable that operation `B` uses as input, an edge is created from `A` to `B`.

The solution begins by parsing the operations to identify the inputs and outputs for each operation, and then creating nodes based on this information. Next, a dependency graph is constructed where each operation's dependencies are represented as directed edges. After building the graph, a topological sort is performed to generate a valid execution order that respects the dependencies, with integrated shuffling for operations with zero dependencies.

# Usage

This C++ project allows you to input operations with dependencies and obfuscates the order of operations while maintaining the dependencies. It provides functionality to read inputs either from the console or from a file, and performs a topological sort with shuffling of operations that have no dependencies (zero in-degree).

## Project Structure

The project consists of two main files:

- `OperationsGraph.cpp`: Contains the implementation of the graph data structure and operations logic.
- `main.cpp`: The entry point of the program, which handles input and runs the main logic.
- `test-files/`: A directory containing text files with operations for testing file-based input.

The test-files/ directory is used to store text files that contain operation sequences. These files are used as input when the program is run with the file input option. The file format for the operations is the same as the console input format, with each operation written on a separate line in the following format:

output1,output2 = Operation(input1,input2)

### Main Classes and Functions

- **Operation**: Represents a computational operation with inputs, outputs, and an original expression.
- **OperationsGraph**: A class that manages a graph of operations and handles topological sorting with shuffling of operations that have zero dependencies.

### Key Functions

- `addOperation`: Adds an operation to the graph and updates the dependencies.
- `topSortWithShuffle`: Performs a topological sort while shuffling operations that have no dependencies.
- `parseInput`: Parses a string input to create an `Operation` object.
- `processInput`: Processes input from either the console or a file to build the graph.