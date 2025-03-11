GoLF Compiler 

Building the Compiler

Build Instructions:
Step 1. Pull the Git Repo into a Repository
Step 2. Run make in the directory, this will build an executable golf from all of the code in the directory.
 - You can also run make clean to fully clean up the files in the directory.

Running the Compiler
- This compiler only takes one argument, that being the file name, here is how you should run it.

Examples of running the compiler
./golf functest.txt
./golf /TEST/final/gen.t3
./golf /TEST/final/gen.t25 > temp.c && gcc temp.c && ./a.out

There is additionally a run.output, which shows the results of Generated C Code which has been submitted as well in this directory.
