# Compiler for CoCo Project

A compiler for the course project of CS F363: Compiler Construction at BITS Pilani, Pilani campus.

We follow the [the language specification doc](./Language%20Specifications.pdf) as provided. This repository contains the lexical and syntax analyzers for the language.

## Team members

- Saaransh Jain
- Vishnu Hari
- Aman Patel
- Parth Sudan

## Building

Compatible with **GCC version 11.4.0 and Ubuntu 22.04**.

```bash
git clone https://github.com/SaaranshJain/coco-project.git
cd coco-project
make compiler
```

## Execution

Run the command

```bash
./compiler <input file> <output file>
```

This opens an interactive compiler session, where you can choose to -

1. Remove comments from the program
2. Print the tokens from the input file
3. Generate the parse tree for the program

Any errors encountered in the process will be printed to `STDERR`.
