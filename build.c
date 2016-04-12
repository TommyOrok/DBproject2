#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

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



// START: from p2random.c
typedef struct {
	size_t index;
	uint32_t num[625];
} rand32_t;

rand32_t *rand32_init(uint32_t x)
{
	rand32_t *s = malloc(sizeof(rand32_t));
	uint32_t *n = s->num;
	size_t i = 1;
	n[0] = x;
	do {
		x = 0x6c078965 * (x ^ (x >> 30));
		n[i] = x;
	} while (++i != 624);
	s->index = i;
	return s;
}

uint32_t rand32_next(rand32_t *s)
{
	uint32_t x, *n = s->num;
	size_t i = s->index;
	if (i == 624) {
		i = 0;
		do {
			x = (n[i] & 0x80000000) + (n[i + 1] & 0x7fffffff);
			n[i] = (n[i + 397] ^ (x >> 1)) ^ (0x9908b0df & -(x & 1));
		} while (++i != 227);
		n[624] = n[0];
		do {
			x = (n[i] & 0x80000000) + (n[i + 1] & 0x7fffffff);
			n[i] = (n[i - 227] ^ (x >> 1)) ^ (0x9908b0df & -(x & 1));
		} while (++i != 624);
		i = 0;
	}
	x = n[i];
	x ^= (x >> 11);
	x ^= (x <<  7) & 0x9d2c5680;
	x ^= (x << 15) & 0xefc60000;
	x ^= (x >> 18);
	s->index = i + 1;
	return x;
}

int int32_cmp(const void *x, const void *y)
{
	int32_t a = * (const int*) x;
	int32_t b = * (const int*) y;
	return a < b ? -1 : a > b ? 1 : 0;
}

int32_t *generate(size_t n, rand32_t *gen)
{
	size_t i;
	int32_t *a = malloc(n * 4);	
	for (i = 0 ; i != n ; ++i)
		a[i] = rand32_next(gen);
	return a;
}

size_t compact(int32_t *a, size_t n)
{
	size_t i, j;
	int32_t x = a[0];
	for (i = j = 1 ; i != n ; ++i) {
		int32_t y = a[i];
		if (x != y) a[j++] = x = y;
	}
	return j;
}

int32_t *generate_sorted_unique(size_t n, rand32_t *gen)
{
	size_t m = n + 10;
	int32_t *a = NULL;
	do {
		free(a);
		m *= 1.1;
		a = generate(m, gen);
		qsort(a, m, 4, int32_cmp);		
	} while (compact(a, m) < n);
	return a;
}
// END: from p2random.c




/* main */
int main(int argc, char **argv)
{
	// input validation
	if (argc < MIN_REQUIRED)
		return help();



	// GET nunber of keys and probes
	int keys = atoi(argv[1]);		printf("Number of Keys: %d\n", keys);
	int probes = atoi(argv[2]);		printf("Number of Probes: %d\n", probes);



	// GENERATE random keys
	rand32_t *gen = rand32_init(time(NULL));
	size_t pos, n = keys;									// pass number of keys from cmd line
	int32_t *key_arr = generate_sorted_unique(n, gen);		printf("Random Keys\n");
	for (pos = 0 ; pos != n ; ++pos) {
		printf("%d\n", key_arr[pos]);
	}


	// GENERATE random probes
	rand32_t *genn = rand32_init(time(NULL));
	pos = 0; n = probes;									// pass number of probes from cmd line
	int32_t *probe_arr = generate(n, genn);					printf("Random Probes\n");
	for (pos = 0 ; pos != n ; ++pos) {
		printf("%d\n", probe_arr[pos]);
	}


	// MAKE an array of the fanouts for each level
	int size = (argc - 3);	// subtracting non-fanout args
	int *fanout_arr = malloc(sizeof(int) * size);
	int i, j = 0;

	for (i = 3; i < argc; i++) {
		// add fanout vals from argv to fanout_arr
		fanout_arr[j] = atoi(argv[i]);	j++;
		printf("Level %d Fanout: %d\n", j, fanout_arr[j-1]);
	}
	


	// GET dimensions (length and width) of 2D array
	//int length_tree = sizeof(fanout_arr) / sizeof(int);
	int length_tree = argc - 3;
	int width_tree = fanout_arr[0];

	int in = 0, foo = 0;
	for (foo = 1; foo < length_tree; foo++ ) {		
		width_tree = width_tree * fanout_arr[in];
	}


	printf("Length of 2D array: %d\n", length_tree);
	printf("Width of 2D array: %d\n", width_tree);


/*
	// initialize 2D array for search tree
	int tree[length_tree][width_tree];
*/


	// FREE allocated memory
	free(key_arr);
	free(gen);
	free(probe_arr);
	free(genn);
	free(fanout_arr);



	return 0;
}