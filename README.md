# TOYC - Toy Compiler

## Description

TOYC (Toy Compiler) is a custom assembler developed in C. It's designed to translate TOY assembly language instructions into TOY machine code, supporting unique syntax rules and different instruction formats.

## Features

- Translates TOY assembly instructions into TOY machine code.
- Labels support.
- Offsets support.
- Values support.
- Comments are also available.

## Syntax Specifics

- Instructions do not require commas between registers, labels, and addresses.
- The `stor` instruction has the format `stor dest addr` (e.g., `stor r1 0xff`).
- The `halt` instruction requires three register placeholders (e.g., `halt r0 r0 r0`).

## Getting Started

### Prerequisites

- C compiler (like gcc or clang)
- Make (for building the project using the Makefile)

### Building the Project

Use the provided Makefile for easy compilation:

- To compile, run:
  ```bash
  make all
- To clean up object files and executable, run:
  ```bash
  make fclean
- To recompile, run:
  ```bash
  make re

## Usage

- To run the assembler, use:
   ```bash
  ./toyc inputfile.toy outputfile.bin
  
- Example toy sources are located inside of the `toy_progs_examples` directory.
