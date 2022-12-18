~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SmartCompile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Jordan "Jorb" Dehmel, 2022
jdehmel@outlook.com, github.com/jorbDehmel
github.com/jorbDehmel/smartCompile

Note: Only compatible with ubuntu linux

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Description:

    A simple auto compiler for C++
projects. Scans for .cpp and .hpp files,
compiles them into .o files if needed, and
links them according to whether or not
they have a main function. Compiles a
seperate .out file for each main function
it finds. (Only compiles or links if the
source files have been modified since the
last compiling/linking)

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Installation and usage:

    To install, first change the terminal
to this directory. Then run 
`make install`. This project relies on
the boost libraries, which will be 
automatically installed if they aren't
present.

    To use on a project, run `sc` in the
project's folder (after installation).

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Terminal arguments:

Arg | Long arg    | Description
------------------------------------------
-u  | --use <CC>  | changes compiler to CC
    | -org        | organizes files
-h  | --help      | shows help
    | -<argname>  | adds an argument
-c  | --clean     | removes .o files
-p  | --purge     | removes .o and .out
-jc | --justclean | cleans (doesn't run)
-jp | --justpurge | purges (doesn't run)

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Autotagging:

    Insert "// SC_ARGS <tags go here>" as 
the beginning of a .cpp file to activate
autotagging.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~