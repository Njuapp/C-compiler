### File Structure:
```
	/Lab
	    /Code
		Makefile
		lexical.l
		syntax.y
		main.c
		  .
		  .
	    /Test
		test1.cmm
		test2.cmm
		  .
		  .
	    /report.pdf
	    /parser
	    /README
```
### Prerequsite
1. flex
2. bison
3. gcc

### Code/
1. Contains all of files on flex/bison，*.c/*.h files and Makefile；
2. Navigate to Code/ directory and then type 'make'.

### Test/
1. Type 'make test' and our compiler will compile the test1.cmm
2. Your newly added test file should be named in "*.cmm" format.
