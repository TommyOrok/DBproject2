#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//[5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130]
//fanout must be between 2 and 17
//if too many keys or too little keys for given fanout, throw error
//if a tree node isn't filled, pad it with MAXINT
//if a new node will never be started, don't allocate space for that node.

//elts array tells us maximum fill for given level

#define MAXINT 2147483647
#define LOW 0
#define HI 1

int hiLo[2];

void binarysearch(int *lvlptr, int first, int last, int search){
	int middle = (first + last)/2 ;

	while (first <= last) {
        if (lvlptr[middle] < search){
      		first = middle + 1;
      		if (hiLo[LOW] < lvlptr[middle])
      			hiLo[LOW] = lvlptr[middle] ;
      	}    
      	else if (lvlptr[middle] == search) {
	        //printf("%d found at location %d.\n", search, middle+1);
	        if (hiLo[HI] > lvlptr[middle])
	        	hiLo[HI] = lvlptr[middle] ;
	        break;
      	}
      	else{
      		last = middle - 1;
      		if (hiLo[HI] > lvlptr[middle])
      			hiLo[HI] = lvlptr[middle] ;
      	}
 
      	middle = (first + last)/2;
   	}
   	//if (first > last)
      	//printf("bound created\n");
}


int main(int argc, char **argv){
//info from command line: build 26 2 3 3 3
	int numLevels = argc - 3;
	int keyn = atoi(argv[1]) ;
	int ** levels; // = (int **) malloc(sizeof(int *)*numLevels) ;
	posix_memalign( (void **) &levels, 16, sizeof(int *)*numLevels) ;

	int *keys;// = (int *) malloc(sizeof(int)*keyn);
	posix_memalign( (void **) &keys, 16, sizeof(int)*keyn) ;

	int *indexes; // = (int *) malloc(sizeof(int)*numLevels) ;  //all zero at first
	posix_memalign( (void **) &indexes, 16, sizeof(int)*numLevels) ;

	int *makenew ; //= (int *) malloc(sizeof(int)*numLevels);  //all zero at first
	posix_memalign( (void **) &makenew, 16, sizeof(int)*numLevels) ;

	int *nodeS ; //= (int *) malloc(sizeof(int)*numLevels);  //set to each Fout - 1
	posix_memalign( (void **) &nodeS, 16, sizeof(int)*numLevels) ;

	int *eltsL ;
	posix_memalign( (void **) &eltsL, 16, sizeof(int *)*numLevels) ;

	int l;
	int lastFan = 1;  //nodesizey * fanoutx * numbernodesx
	for (l = 0; l < numLevels; l++){
		nodeS[l] = atoi(argv[l + 3]) - 1 ;  //node size for this level
		eltsL[l] = nodeS[l]*lastFan*(l==0 ? 1 : atoi(argv[l + 2])/nodeS[l]) ;
		lastFan = lastFan*atoi(argv[l + 3]) ;  //node# is now old # times current fanout
		//printf("level: %d, # of elts: %d\n", l, eltsL[l]) ;
		//levels[l] = (int *) malloc(sizeof(int)*18) ; //worry about this 18
		//posix_memalign( (void **) &(levels[l]), 16, sizeof(int)*eltsL[l]) ;  //STILL HANDLE THIS 18
		indexes[l] = 0 ;
		makenew[l] = 0 ;
	}


	//now reverse eltsL
	int e;
	for (l = 0, e = numLevels - 1; l < e; l++, e--){
		int temp = eltsL[l];
		eltsL[l] = eltsL[e] ;
		eltsL[e] = temp;

	}
	

	//allocate levels sub arrays
	for(l = 0; l < numLevels; l++)
		posix_memalign( (void **) &(levels[l]), 16, sizeof(int)*eltsL[l]) ;


	//create array of keys, this should actually be replaced with Uzo's code
	int i = 0;
	for (i; i < keyn- 2; i++)
		keys[i] = 5*(i+1) ;
	/////////////////////////////////////////////////////////////////////////


	//now iterate through keys
	int currlevel = 0;
	int k = 0;
	for (k; k < keyn; k++){
		//define current key
		int currkey = keys[k] ;

		//check if trying to start a new node
		if (indexes[currlevel]%nodeS[currlevel]==0 && indexes[currlevel]!=0){
			//check if we are allowed to make new node
			if (makenew[currlevel] || currlevel == (numLevels - 1)){ //allowed
				//printf("ALLOWED to NN, level: %d, index: %d, elt: %d\n", currlevel, indexes[currlevel], currkey);
				makenew[currlevel] = 0 ;
				levels[currlevel][indexes[currlevel]] = currkey ;
				indexes[currlevel]++;
				//printf("index now: %d\n", indexes[currlevel]) ;
				currlevel = 0;
			}
			else{ //not ALLOWED
				//printf("DISALLOWED NN @ level: %d, move one higer\n", currlevel);
				makenew[currlevel] = 1 ; //allow next time
				currlevel++;  
				k--;
				continue;
			}
		}
		else{
			//printf("WRITTING to level: %d, index: %d, elt: %d\n", currlevel, indexes[currlevel], currkey) ;
			levels[currlevel][indexes[currlevel]] = currkey ;
			indexes[currlevel]++;
			currlevel = 0;
			//printf("index inc, level now 0\n");
		}

	}


	printf("[");
	int m = 0;
	for (m=0; m < eltsL[2]; m++)
		printf("%d,", levels[2][m]) ;
	printf("]\n");   

	printf("[");
	int n = 0;
	for (n=0; n < eltsL[1]; n++)
		printf("%d,", levels[1][n]) ;
	printf("]\n");   

	printf("[");
	int o = 0;
	for (o=0; o < eltsL[0]; o++)
		printf("%d,", levels[0][o]) ;
	printf("]\n");   

	
	hiLo[LOW] = -2147483645; 
	hiLo[HI] = 2147483647;
	for (l = 0; l < numLevels; l++){
		binarysearch(levels[l], 0, eltsL[l] - 1, 118) ;
	}

	printf("bound:  [%d, %d]\n", hiLo[LOW], hiLo[HI]) ;

}
 

