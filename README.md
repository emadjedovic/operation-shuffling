**Operation Shuffling** is a technique used to obfuscate algorithms by randomly shuffling the sequence of operations, while maintaining the algorithm's correctness. This method can be employed to prevent reverse engineering and tampering in cybersecurity, intellectual property protection, and software development.

## Problem Statement

Given a series of operations in an algorithm, we aim to shuffle the operations such that the algorithm still functions the same way. Each operation has input and output variables. For example:

```plaintext
a, b = Operation(c, d, e)
d, f = Operation(b, h)
y, g = Operation(u, v)
b = Operation(d, r)
```

The third operation `y, g = Operation(u, v)` can be performed one or two operations earlier.

We also handle loops where a series of operations is repeated a certain number of times. In such cases, we cannot allow an operation outside the loop to go inside, or an operation inside to go outside. For example:

```plaintext
a, b = Operation(c, d, e)
d, f = Operation(b, h)
FOR_BEGIN(5)
    s = Operation(d, k)
    d = Operation(s, t)
FOR_END
y, g = Operation(u, v)
b = Operation(d, g)
```

Example of obfuscation for a simple algorithm that computes the sum of two numbers, squares them in a loop, and calculates a weighted average afterward:

```plaintext
a, b = 6, 8        # Initialize values
sum_ab = a + b     # Sum of the two numbers
FOR_BEGIN(3)       # Loop to calculate squares and update weighted sum
    square_a = a**2
    square_b = b**2
    weighted_sum = square_a + 2 * square_b
FOR_END
average = (sum_ab + weighted_sum) / 2  # Weighted average
```

Obfuscated algorithm (shuffled):

```plaintext
a, b = 6, 8        # Initialize values
FOR_BEGIN(3)       # Loop to calculate squares and update weighted sum
    square_b = b**2
    square_a = a**2
    weighted_sum = square_a + 2 * square_b
FOR_END
sum_ab = a + b     # Sum of the two numbers, moved outside loop for obfuscation
average = (sum_ab + weighted_sum) / 2  # Weighted average
```

In the obfuscated version, operations are shuffled while preserving the dependencies between them, ensuring the algorithm works the same.

### Key Issues:
- `square_a` and `square_b` must be computed before `weighted_sum`, as `weighted_sum` depends on both variables.
- `average` depends on both `sum_ab` and `weighted_sum`, so it must remain after their computations.
- Operations within the loop (`square_a`, `square_b`, `weighted_sum`) cannot move outside the loop.
- Operations outside the loop (such as `sum_ab`) cannot move inside.

## Approach

This problem is modeled as a **dependency graph** problem:
- **Nodes**: Each operation is a node.
- **Edges**: Directed edges represent dependencies—if operation `A` outputs a variable that operation `B` uses as input, an edge is created from `A` to `B`.
- **Loops**: Loops are treated as subgraphs, and operations inside a loop are handled as a unit, ensuring they stay within the loop.

The solution begins by parsing the operations to identify the inputs and outputs for each operation, and then creating nodes based on this information. Next, a dependency graph is constructed where each operation's dependencies are represented as directed edges. After building the graph, a topological sort is performed to generate a valid execution order that respects the dependencies. Once the graph is sorted, the operations are randomly shuffled, but within the constraints of the dependency graph to ensure the algorithm still functions correctly. Lastly, loops are handled separately by treating them as isolated subgraphs, and the operations within the loop are shuffled independently from the operations outside the loop.

## Time Complexity

- Constructing the graph involves iterating through all operations and checking dependencies, which is done in **O(n + m)**:
    - `n_outer`: Number of operations outside loops.
    - `m_outer`: Number of dependencies outside loops.
    - `n_loop`: Number of operations inside a loop.
    - `m_loop`: Number of dependencies inside a loop.
    - `k`: Number of loops.

- For the topological sort:
    - **Outer DAG**: **O(n_outer + m_outer)**
    - **Loop Subgraphs**: Each loop contributes **O(n_loop + m_loop)**, summed over all loops.

- After obtaining a topological sort, shuffling the operations can be done in **O(n)** using a random permutation algorithm such as Fisher-Yates.

Even with nested loops, the overall complexity remains **O(n + m)**, where `n` is the total number of operations and `m` is the total number of dependencies. However, the dependency graph includes additional nested subgraphs, and operations within each loop are handled independently.
