# Compiler Lab Exam Pack (Flex + Bison/Yacc)

This pack has three independent question folders:
1. `01_while_loop`
2. `02_if_else`
3. `03_simple_assignment`

Each folder contains a lexer, parser with syntax checking + SDT/TAC generation, sample input,
a simple TAC optimizer, a target-code generator, and build instructions.

## Requirements
Install Flex, Bison (or Yacc), and GCC. On Debian/Ubuntu:
```sh
sudo apt install flex bison gcc
```

## Build and run
Enter one question folder and run:
```sh
make
./compiler < sample.in
```
The program writes `tac.txt`; then run:
```sh
./optimizer tac.txt optimized.txt
./backend optimized.txt target.txt
cat tac.txt
cat optimized.txt
cat target.txt
```

The parser prints syntax success/failure. TAC uses simple three-address instructions:
` t1 = a * b`, `ifFalse a < b goto L2`, `goto L1`, and `L1:`.

## Exam notes
- The exact grammar differs by question: the while version accepts `while (...) { ... }`,
  the if/else version accepts `if (...) { ... } else { ... }`, and the assignment version
  accepts assignment statements only.
- Statements end with semicolons.
- The optimizer performs constant folding for simple integer arithmetic and a conservative
  loop-invariant-code-motion pass for the canonical loop form `while (condition) { ... }`.
  It is an educational demonstration, not a production compiler optimizer.
- The backend emits illustrative accumulator-style pseudo assembly, not machine-specific code.
- `make clean` removes generated build files and output files.
