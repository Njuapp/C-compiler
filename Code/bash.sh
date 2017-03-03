#!/bin/bash
flex lexical.l
gcc main.c lex.yy.c -lfl -o scanner
./scanner test | less
