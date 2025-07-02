# FT_NM

nm (name mangling) is a tool that can be used to list symbols from object files.

## Getting started

You need to compile the project with `make`.

### Usage

```
usage: ft_nm <option(s)> file
  Options are:
  -a, --debug-syms         Show all symbols, even debugger only
  -g, --extern-only        Show only external symbols
  -u, --undefined-only     Show only undefined symbols
  -D, --dynamic            Display dynamic symbols instead of normal symbols
  -r, --reverse-sort       Sort in reverse order
  -p, --no-sort            Show symbols in order encountered
  -h, --help               Display this help
  -d, --debug              Display debug information
  -v, --version            Display the version
```

### Example

```bash
$> ./ft_nm targets/target
0000000000000000 T _start
000000000000000f d fmt
0000000000000000 d msg
                 U printf
```