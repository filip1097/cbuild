# Cbuild

Cbuild is an easy to use program wrapper for building C-projects with gcc.

## How to use 

#### Build product code

To build a C-project simply call cbuild:

```
cbuild
```

Cbuild will then search the decending directories for c-files and h-files to use for compilation.
Test code stored in the directory `./test` will be ignored.
Object files and executables will be stored in `./build`.

#### Build test code 

To build the test code of a C-project:

```
cbuild --test
```

By including `--test` the code in `./test` will now be included in compilation.
Compiled files are stored in `./test_build`.


## TODO

Things that are on the list to be done in the project:

* Only compile necessary files.
* Product code files will not be included in `./test_build`.
* Replace all sprintf with snprintf
