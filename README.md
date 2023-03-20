# Homework 5

In this homework, you will write two command line utilities that will print the files that match a given permission string.
In Part 1, you will build a simple library to recurse the file system and print out the matching files. In Part 2, you will use fork, exec, pipe, and sort to sort this output.

Instructions for each part can be found under 

    part1/instructions.md
    part2/instructions.md

All your changes must be done under the `src` directory for each part.

Your code should be organized as follows. (Ignoring all markdown files)

    part1
        \_ src
            \_ pfind.c
            \_ Makefile
    part2
        \_ src
            \_ spfind.c
            \_ Makefile

All files and directories must be named exactly as above case-sensitive. You should not commit any extraneous files, such as object files or executables. 
*Note: it is ok to track the pfind executable provided for part 2.*

All rules about Makefiles and compilation are listed on HW2.

If you are allocating heap memory, you should follow all rules listed on HW4 and HW3. 

All submissions must include at least five git commits with **meaningful** messages.

## Submission

To submit:

    git commit -am "hw5 completed"
    git tag -a "handin" -m "hw5 submission"
    git push origin main
    git push origin handin

After submitting, you should re-test with your submitted version. Details on how to do that are [here](https://github.com/cs3157-borowski/guides/blob/main/submission.md).
