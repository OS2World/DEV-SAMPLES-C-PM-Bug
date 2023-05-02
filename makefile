# nmake makefile
#
# Tools used:
#  Compile::Resource Compiler
#  Compile::GNU C
#  Make: nmake
all : bug.exe

bug.exe : main.obj bug.res bug.def
	gcc -Zomf main.obj bug.res bug.def -o bug.exe
	wrc bug.res

main.obj : main.c
	gcc -Wall -Zomf -c -O2 main.c -o main.obj

bug.res : bug.rc bug.ico
	wrc -r bug.rc

clean :
	rm -rf *exe *res *obj
