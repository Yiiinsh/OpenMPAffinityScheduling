CC = icc
CFLAGS = -O3 -qopenmp -std=c99 

EXE = loops2 

SRC = loops2.c workqueue.c
INC = workqueue.h
OBJ = $(SRC:.c=.o)

all : $(EXE)

$(EXE) : $(OBJ)
	   $(CC) $(CFLAGS) -o $@ $(OBJ)

$(OBJ) : %o : %c $(INC)
	$(CC) $(CFLAGS) -c $< 


.PHONY: clean
clean :
	rm -f $(OBJ) $(EXE)
