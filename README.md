# FT_NM

My nm projet for the 42 School cursus. The project nm (name mangling) is a tool that can be used to list symbols from object files.

## Getting started

You need to compile the project with `make`.

### Usage

```
usage: ft_nm <option(s)> file
  Options are:
  -a, --debug-syms         Show all symbols, even debugger only
  -d, --debug              Display debug information
  -D, --dynamic            Display dynamic symbols instead of normal symbols
  -g, --extern-only        Show only external symbols
  -h, --help               Display this help
  -n, --numeric-sort       Sort symbols by address
  -p, --no-sort            Show symbols in order encountered
  -r, --reverse-sort       Sort in reverse order
  -t, --radix=<radix>      Radix (o/d/x) for printing symbol Values
  -U, --defined-only       Show only defined symbols
  -u, --undefined-only     Show only undefined symbols
  -v, --version            Display the version
  -W, --no-weak            Show only non-weak symbols
```

### Example

```bash
$> ./ft_nm targets/target
0000000000000000 T _start
000000000000000f d fmt
0000000000000000 d msg
                 U printf
```