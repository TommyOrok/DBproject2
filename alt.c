#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//[5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130]
//use pointers to each level as arrays..
//fanout must be between 2 and 17
//more on posix memalign:  "Confused about the requirement"
//if too many keys or too little keys for given fanout, throw error
//if a tree node isn't filled, pad it with MAXINT
//if a new node will never be started, don't allocate space for that node.

int main(){
	//create array of size 26
	int keys[26] ;
	//int levels[3][18] ;
	int *levels[3];
	int l;
	for (l = 0; l < sizeof(levels)/sizeof(levels[0]); l++){ 
		levels[l] = (int *) malloc(sizeof(int)*18) ;
	}
	int indexes[3] = {0, 0 , 0} ;
	int makenew[3] = {0, 0, 0} ;
	int nodeS[3] = {2, 2, 2} ;


	//create array of keys, this should actually be replaced with Uzo's code
	int i = 0;
	for (i; i < sizeof(keys)/sizeof(keys[0]); i++)
		keys[i] = 5*(i+1) ;


	//now iterate through keys
	int currlevel = 0;
	int k = 0;
	for (k; k < sizeof(keys)/sizeof(keys[0]); k++){
		//define current key
		int currkey = keys[k] ;

		//check if trying to start a new node
		if (indexes[currlevel]%nodeS[currlevel]==0 && indexes[currlevel]!=0){
			//check if we are allowed to make new node
			if (makenew[currlevel] || currlevel == (sizeof(levels) - 1)){ //allowed
				makenew[currlevel] = 0 ;
				levels[currlevel][indexes[currlevel]] = currkey ;
				indexes[currlevel]++;
				currlevel = 0;
			}
			else{ //not ALLOWED
				makenew[currlevel] = 1 ; //allow next time
				currlevel++;  
				k--;
				continue;
			}
		}
		else{
			levels[currlevel][indexes[currlevel]] = currkey ;
			indexes[currlevel]++;
			currlevel = 0;
		}

	}


	printf("[");
	int m = 0;
	for (m; m < 18; m++)
		printf("%d,", levels[2][m]) ;
	printf("]\n");   

	printf("[");
	int n = 0;
	for (n; n < 18; n++)
		printf("%d,", levels[1][n]) ;
	printf("]\n");   

	printf("[");
	int o = 0;
	for (o; o < 18; o++)
		printf("%d,", levels[0][o]) ;
	printf("]\n");   



}