# hshell

> French version: [README.fr.md](README.fr.md)

A minimal C++ Linux shell built as a personal project to figure out how process execution, redirection, and pipes work at the system level.

## What it does

- Interactive prompt displaying current working directory (`hshell ~/Projects >`)
- Process execution using `fork` / `exec`
- Built-in commands: `cd`, `pwd`, `history`, `exit`
- Single pipe support (`cmd1 | cmd2`)
- File redirection (`cmd > file.txt`)

## Quick start

Requires `g++` (C++17 standard) on Linux.

```bash
make
./hshell
```

Exit with `exit` or Ctrl+D. Run `make clean` to remove compiled binaries.

## Usage example

```
hshell ~/Projects > ls | grep cpp
main.cpp

hshell ~/Projects > echo "hello world" > output.txt

hshell ~/Projects > cat output.txt
"hello world"
```

## Limitations

- Max of one pipe (`|`) or one redirect (`>`) per command (cannot mix both)
- No quote parsing, wildcards (`*`), or environment variable expansion (`$HOME`)
- No background process execution (`&`)
- Command history is kept in memory and resets on exit

## Project structure

```
src/
├── main.cpp    # REPL loop, parsing, built-ins
└── shell.cpp   # process control, pipes, file descriptors
include/
└── shell.h     # declarations
```

~330 lines of C++ total.
