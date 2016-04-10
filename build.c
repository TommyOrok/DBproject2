#include <stdio.h>
#include
#include

#define MIN_REQUIRED 4


/* display usage */
int help() {
   printf("Usage: build [K] [P] [int] ...\n");
   printf("\tK: any int val -> num keys\n");
   printf("\tP: any int -> num probes\n");
   printf("\tint: any int val -> num fanout for level 1\n");
   printf("\t...: string of ints (for level 2, 3, etc.)\n");
   return 1;
}


/* main */
int main(int argc, char **argv)
{
	// input validation
	if (argc < MIN_REQUIRED)
		return help();



	// get nunber of keys and probes
	int k = argv[1];
	int p = argv[2];
	
	// make an array of the fanouts for each level
	int size = (sizeof(argv)/sizeof(int)) - 3;
	int fanout_levels[size];

	int i, j = 0;

	for (i = 3; i < (sizeof(argv)/sizeof(int)); i++) {
		fanout_levels[j] = argv[i];
		j++;
	}


	// MAKE SHIT HAPPEN
	//
}