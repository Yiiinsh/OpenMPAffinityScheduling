CC = gcc-7
CFLAGS = -fopenmp -std=c99

EXE = loops2 

SRC = loops2.c
OBJ = $(SRC:.c=.o)

all : $(EXE)

$(EXE) : $(OBJ)
	   $(CC) $(CFLAGS) -o $@ $(OBJ)

$(OBJ) : $(SRC)
	$(CC) $(CFLAGS) -c $<


.PHONY: clean
clean :
	rm -f $(OBJ) $(EXE)
