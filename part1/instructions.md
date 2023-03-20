# Part 1

## Overview 

The goal of this assignment is to create an executable `pfind`. The executable
takes the following parameters:

    ./pfind -d <directory> -p <permissions string> [-h]

On success, `pfind` will print the files that match the given permission string
starting in the given directory and return `EXIT_SUCCESS`. On failure, `pfind`
returns `EXIT_FAILURE`.

## Part A - Command Line Flags

You should verify that the flags passed in are consistent with the usage
statement listed in the overview. You should specify the directory to be
searched after the -d flag, and list files matching the specified permissions
following the -p flag. If the -h flag is passed, you should print a usage
message to `stdout` and return `EXIT_SUCCESS`. You may handle errors as you see
fit, but any incorrect usage must print a reasonable descriptive message to
`stderr` and return `EXIT_FAILURE`.

## Part B - Permission Strings

You should verify that the given permission string is in proper format. That is,
there are exactly 9 characters which all must be a dash (-) or one of the
characters rwx, in the proper position.

Some examples of valid permissions strings:

    rwxrwxrwx
    ---------
    rw-r--r--
    rwx------

Some examples of invalid permissions strings:

    abcdefghi
    xrwxrwxrw
    ---rrr---
    -rwxrwxrwxrwxrwxrwxrwxrwxrwx

If an invalid permissions string is passed, you should print a reasonable
message to `stderr` and return `EXIT_FAILURE`.

## Part C - Recursing the Directory Tree

After validating the inputs, you should implement the recursion, printing out
each file that matches the given permission string (the full path must be
printed, not just the filename). Use `lstat`, not `stat`, as you recurse through
the file system.

## Sample Executions

To assist you, we will be providing you with some sample executions. However,
please note that the examples shown are not comprehensive and therefore other
edge or error cases may be tested. **Additionally, "~" in the output should expand
to the full path of your home directory.**

For these examples, the directory tree will have the following format:


    test_dir
    ├── subdir1
    │   ├── file1
    │   └── file2
    └── subdir2
        ├── file1
        └── file2

    
The files in test_dir have the following permissions:

```
test_dir:
drwxrwxrwx subdir1
drwxr-xr-x subdir2

test_dir/subdir1:
---------- file1
-rw-r--r-- file2

test_dir/subdir2:
-rw-r--r-- file1
-rw-r--r-- file2
```
<!-- just so that it renders separately -->

    $ ./pfind -h
    Usage: ./pfind -d <directory> -p <permissions string> [-h]
    $ ./pfind -v -h
    Error: Unknown option '-v' received.
    $ ./pfind -d ~/test_dir
    Error: Required argument -p <permissions string> not found.
    $ ./pfind -p rwxrwxrwx
    Error: Required argument -d <directory> not found.
    $ ./pfind -d ~/test_dir -p badpermis
    Error: Permissions string 'badpermis' is invalid.
    $ ./pfind -d invalid_dir -p rwxrwxrwx
    Error: Cannot stat 'invalid_dir'. No such file or directory.
    $ ./pfind -d invalid_dir -p badpermis
    Error: Cannot stat 'invalid_dir'. No such file or directory.
    $ ./pfind -d ~/test_dir -p rwxrwxrwx -h
    Usage: pfind -d <directory> -p <permissions string> [-h]
    $ ./pfind -d ~/test_dir -p rw-r--r--
    ~/test_dir/subdir1/file2
    ~/test_dir/subdir2/file2
    ~/test_dir/subdir2/file1
    $ ./pfind -d ~/test_dir -p --x--x--x
    <no output>


Create a new directory danger_dir with permissions `---------` inside your
directory, and try to run `pfind` on it.

    $ mkdir -m 000 ~/danger_dir
    $ ./pfind -d ~/danger_dir -p --x--x--x
    Error: Cannot open directory '~/danger_dir'. Permission denied.
