# OpenMP Affinity Scheduling
## Introduction
OpenMP affinity scheduling implementation by hand.

## Project Structure

* loops2.c : main entry of the program for testing.
* workqueue.h/.c : workqueue definition & implementation

## Build
To build this project, open terminal and type:
    
    make

Program with name loops2 will appear under current working directory.

## Usage
To run this program, type: 

    ./loops2

## Demo
Assumed that user has already build the program.

Firstly, set OMP\_NUM\_THREADS to expected value :

    export OMP_NUM_THREADS=8

Then, run the program:

    ./loops2

Execution results will be printed to the screen:

    Loop 1 check: Sum of a is -343021.474766
    Total time for 1000 reps of loop 1 = 0.340465
    Loop 2 check: Sum of c is -25242644.603147
    Total time for 1000 reps of loop 2 = 1.647152

## Author
B119172