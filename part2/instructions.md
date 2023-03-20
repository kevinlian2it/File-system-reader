# Part 2
You will build an executable `spfind` that sorts the output from `pfind`. All
your code should be in a file named `spfind.c`.

You will fork two child processes, one for `pfind` (executable provided) and one
for `sort`. For more information on `sort`, check out `man 1 sort`. You will
connect the standard output of `pfind` to the standard input of `sort` using one
pipe, then you will connect a second pipe to the standard output of `sort`,
which your parent process will read from.

Your parent process will read from the read-end of the `sort` pipe until it
reaches end-of-file (`read()` returns 0), print out all the text received, and
report how many lines were returned.

A few caveats: you will not use `popen()`, `fread()`, `fwrite()`, or any other
function that uses FILE streams. You will be working with pipes (which are raw
file descriptors), using `fork()` and `exec*()`, using `dup2()` to duplicate
file descriptors, and making sure you close all unused file descriptors. You may
also use `read()` and `write()`.

You should check the return code of **all system and function calls** you use in
your program. If any of them fail, print a reasonable error message and return
`EXIT_FAILURE`. 

However, if an error occurs when exec-ing `pfind` or `sort`, you should print

`Error: pfind failed.`

or 

`Error: sort failed.`

to standard error.

## Sample Executions

```
$ ./spfind
Usage: ./spfind -d <directory> -p <permissions string> [-h]

$ ./spfind -d ~ -p rwxr-xr-q
Error: Permissions string 'rwxr-xr-q' is invalid.
```

For the following output, we will operate under the following directory tree and
permissions.


    test_dir
    ├── subdir1
    │   ├── file1
    │   └── file2
    └── subdir2
    │   ├── file1
    │   └── file2
    └── danger

    
The files in test_dir have the following permissions:

```
test_dir:
drwxrwxrwx subdir1
drwxr-xr-x subdir2
---------- danger

test_dir/subdir1:
---------- file1
-rw-r--r-- file2

test_dir/subdir2:
-rw-r--r-- file1
-rw-r--r-- file2
```

**Please note that, as with part 1, "~" in the output should expand to the full
path of your home directory.**

```
$ ./spfind -d ~/test_dir -p rw-r--r--
Error: Cannot open directory '~/test_dir/danger'. Permission denied.
~/test_dir/subdir1/file2
~/test_dir/subdir2/file1
~/test_dir/subdir2/file2
Total matches: 3
```

Notice the error message is written to standard error and is not part of the
total matches. However, the names of the files and number of total matches is
written to standard output.
