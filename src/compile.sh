#!/bin/sh
echo "compiling"

# pipeline of execution without pepper (directly from the exisitng api result file)
# compiling (converts the source code to object files (*.o))
gcc -c  bn_functions.c  -I/usr/include/python3.5/
gcc -c  bn_netica.c   -I/home/karen/Netica_API_504/src   -I/usr/include/python3.5/
# linking object files together (linker takes all the object files, together
# with statically-linked libraries and creates an executable program: my_program.exe)
gcc -o  my_program bn_functions.o bn_netica.o  /home/karen/Netica_API_504/lib/NeticaEx.o  -L/home/karen/Netica_API_504/lib  -lrt -lnetica -lpython3.5m -lpthread -lstdc++ -lm
./my_program



# pipeline of execution for pepper
# python motion_module.py
# gcc -c  bn_functions.c  -I/usr/include/python3.5/
# gcc -c  bn_netica.c   -I/home/karen/Netica_API_504/src   -I/usr/include/python3.5/
# gcc -o  my_program bn_functions.o bn_netica.o  /home/karen/Netica_API_504/lib/NeticaEx.o  -L/home/karen/Netica_API_504/lib  -lrt -lnetica -lpython3.5m -lpthread -lstdc++ -lm
# ./my_program
# python speak_module.py
