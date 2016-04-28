#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <xmmintrin.h> //SSE 
#include <emmintrin.h> //SSE2 
#include <pmmintrin.h> //SSE3 
#include <tmmintrin.h> //SSSE3 
#include <smmintrin.h> //SSE4.1 
#include <nmmintrin.h> //SSE4.2 
#include <ammintrin.h> //SSE4A
#include <x86intrin.h>


#include "p2random.h"
#include "tree.h"


void print128_numstr(__m128i var, char *stri, int num)
{
        if (num == 32){
                int32_t *val = (int32_t*) &var;
                printf("%s: %i %i %i %i \n",stri, 
                   val[0], val[1], val[2], val[3]);
        }
        else if(num == 16){
                int16_t *val = (int16_t*) &var;
                printf("%s: %i %i %i %i %i %i %i %i \n",stri, 
                   val[0], val[1], val[2], val[3], val[4], val[5], val[6], val[7]);
        }
        else if(num == 8){
                int8_t *val = (int8_t*) &var;
                printf("%s: %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i \n",stri, 
                   val[0], val[1], val[2], val[3], val[4], val[5], val[6], val[7], 
                   val[8], val[9], val[10], val[11], val[12], val[13], val[14], val[15]);
        }
}

int main(int argc, char* argv[]) {
        // parsing arguments
        assert(argc > 3);
        size_t num_keys = strtoull(argv[1], NULL, 0);
        size_t num_probes = strtoull(argv[2], NULL, 0);
        size_t num_levels = (size_t) argc - 3;
        size_t* fanout = malloc(sizeof(size_t) * num_levels);
        assert(fanout != NULL);
        for (size_t i = 0; i < num_levels; ++i) {
                fanout[i] = strtoull(argv[i + 3], NULL, 0);
                assert(fanout[i] >= 2 && fanout[i] <= 17);
        }

        int special = 0;
        int fir, mid, las;
        for (fir = 3, mid = 4, las=5; argc > 5 && las < argc; fir++, mid++, las++){
                if (atoi(argv[fir])==9 && atoi(argv[mid])==5 && atoi(argv[las])==9){
                        special = 1;
                        break;
                }
        }

        // building the tree index
        rand32_t* gen = rand32_init((uint32_t) time(NULL));
        assert(gen != NULL);
        int32_t* delimiter = generate_sorted_unique(num_keys, gen);
        assert(delimiter != NULL);
        Tree* tree = build_index(num_levels, fanout, num_keys, delimiter);
        free(delimiter);
        free(fanout);
        if (tree == NULL) {
                free(gen);
                exit(EXIT_FAILURE);
        }

        // generate probes
        int32_t* probe = generate2(num_probes, gen, num_keys);  //ADD: num_keys (3rd arg) and "generate2"
        assert(probe != NULL);
        free(gen);
        uint32_t* result = malloc(sizeof(uint32_t) * num_probes);
        assert(result != NULL);

        printf("PROBES: \n");
        for (int ind = 0; ind < num_probes; ind++){
                printf("probe: %d\n", probe[ind]) ;
        }


        //show tree
        printf("LEVEL 0\n[");
        for (int ind = 0; ind < tree->node_capacity[0] ; ind++){
                printf("%d,",tree->key_array[0][ind]) ;
        }
        printf("]\n\n") ;

        printf("LEVEL 1\n[");
        int eltsin1 = tree->node_capacity[1]*(tree->node_capacity[0]+1) + 1 ;
        for (int ind = 0; ind < tree->node_capacity[1]*(tree->node_capacity[0]+1) ; ind++){
                printf("%d,",tree->key_array[1][ind]) ;
        }
        printf("]\n\n") ;

        printf("LEVEL 2\n[");
        for (int ind = 0; ind < tree->node_capacity[2]*eltsin1 ; ind++){
                printf("%d,",tree->key_array[2][ind]) ;
        }
        printf("]\n\n") ;

        /*if (argc > 6){
                printf("LEVEL 3\n[");
                for (int ind = 0; ind < tree->node_capacity[3]*eltsin1 ; ind++){
                        printf("%d,",tree->key_array[3][ind]) ;
                }
                printf("]\n\n") ;
        }*/


        if (special){
                //SPECIAL CASE 9 5 9 
                __m128i lvl_0_A = _mm_load_si128((__m128i *) &tree->key_array[0][0]) ;
                //print128_numstr(lvl_0_A, "lvl_0_A", 32);

                __m128i lvl_0_B = _mm_load_si128((__m128i *) &tree->key_array[0][4]) ;
                //print128_numstr(lvl_0_B, "lvl_0_B", 32);

                int z;
                for (z = 0; z < num_probes; z += 4){
                        //load the probes into registers, each register has same probe in 4 lanes
                        __m128i k = _mm_load_si128((__m128i*) &probe[z]);
                        register __m128i k1 = _mm_shuffle_epi32(k, _MM_SHUFFLE(0,0,0,0));
                        register __m128i k2 = _mm_shuffle_epi32(k, _MM_SHUFFLE(1,1,1,1));
                        register __m128i k3 = _mm_shuffle_epi32(k, _MM_SHUFFLE(2,2,2,2));
                        register __m128i k4 = _mm_shuffle_epi32(k, _MM_SHUFFLE(3,3,3,3));

                        //print the probes of 
                        /*print128_numstr(k, "k");
                        print128_numstr(k1, "k1");
                        print128_numstr(k2, "k2");
                        print128_numstr(k3, "k3");
                        print128_numstr(k4, "k4");  */

                        //load the levels into registers
                        //access level 0 (9 5 9 -> 8 4 8)
                        //move outside loop...
                        /*__m128i lvl_0_A = _mm_load_si128((__m128i *) &tree->key_array[0][0]) ;
                        print128_numstr(lvl_0_A, "lvl_0_A", 32);

                        __m128i lvl_0_B = _mm_load_si128((__m128i *) &tree->key_array[0][4]) ;
                        print128_numstr(lvl_0_B, "lvl_0_B", 32); */

                        __m128i cmp_0_A1 = _mm_cmpgt_epi32(lvl_0_A, k1);
                        __m128i cmp_0_A2 = _mm_cmpgt_epi32(lvl_0_A, k2);
                        __m128i cmp_0_A3 = _mm_cmpgt_epi32(lvl_0_A, k3);
                        __m128i cmp_0_A4 = _mm_cmpgt_epi32(lvl_0_A, k4);
                        //print128_numstr(cmp_0_A1, "cmp_0_A", 32);

                        __m128i cmp_0_B1 = _mm_cmpgt_epi32(lvl_0_B, k1);
                        __m128i cmp_0_B2 = _mm_cmpgt_epi32(lvl_0_B, k2);
                        __m128i cmp_0_B3 = _mm_cmpgt_epi32(lvl_0_B, k3);
                        __m128i cmp_0_B4 = _mm_cmpgt_epi32(lvl_0_B, k4);
                        //print128_numstr(cmp_0_B1, "cmp_0_B", 32);

                        __m128i cmp_01 = _mm_packs_epi32(cmp_0_A1, cmp_0_B1);
                        __m128i cmp_02 = _mm_packs_epi32(cmp_0_A2, cmp_0_B2);
                        __m128i cmp_03 = _mm_packs_epi32(cmp_0_A3, cmp_0_B3);
                        __m128i cmp_04 = _mm_packs_epi32(cmp_0_A4, cmp_0_B4);
                        //print128_numstr(cmp_01, "cmp_0", 16);

                        int16_t *bits01 = (int16_t *) &cmp_01 ;
                        int16_t *bits02 = (int16_t *) &cmp_02 ;
                        int16_t *bits03 = (int16_t *) &cmp_03 ;
                        int16_t *bits04 = (int16_t *) &cmp_04 ;
                        int r_01 = bits01[0] + bits01[1] + bits01[2] + bits01[3] +
                                  bits01[4] + bits01[5] + bits01[6] + bits01[7] ;
                        r_01 += 8;
                        //printf("r_0 is: %d\n", r_01) ;

                        int r_02 = bits02[0] + bits02[1] + bits02[2] + bits02[3] +
                                  bits02[4] + bits02[5] + bits02[6] + bits02[7] ;
                        r_02 += 8;

                        int r_03 = bits03[0] + bits03[1] + bits03[2] + bits03[3] +
                                  bits03[4] + bits03[5] + bits03[6] + bits03[7] ;
                        r_03 += 8;

                        int r_04 = bits04[0] + bits04[1] + bits04[2] + bits04[3] +
                                  bits04[4] + bits04[5] + bits04[6] + bits04[7] ;
                        r_04 += 8;
                        //WE NOW HAVE R_0, THE RID FOR THE 0TH LEVEL



                        __m128i lvl_11 = _mm_load_si128((__m128i *) &tree->key_array[1][r_01 << 2 ]) ;
                        __m128i lvl_12 = _mm_load_si128((__m128i *) &tree->key_array[1][r_02 << 2 ]) ;
                        __m128i lvl_13 = _mm_load_si128((__m128i *) &tree->key_array[1][r_03 << 2 ]) ;
                        __m128i lvl_14 = _mm_load_si128((__m128i *) &tree->key_array[1][r_04 << 2 ]) ;
                        //print128_numstr(lvl_11, "lvl_1", 32);

                        __m128i cmp_11 = _mm_cmpgt_epi32(lvl_11, k1);
                        __m128i cmp_12 = _mm_cmpgt_epi32(lvl_12, k2);
                        __m128i cmp_13 = _mm_cmpgt_epi32(lvl_13, k3);
                        __m128i cmp_14= _mm_cmpgt_epi32(lvl_14, k4);
                        //print128_numstr(cmp_11, "cmp_1", 32);

                        int32_t * bits11 = (int32_t *) &cmp_11 ;
                        int r_11 = bits11[0] + bits11[1] + bits11[2] + bits11[3] ;
                        r_11 += 4 ;  //offset relative to position in level 0
                        r_11 = (r_01 << 2) + r_11 ;
                        //printf("r_1 is: %d\n", r_11) ;

                        int32_t * bits12 = (int32_t *) &cmp_12 ;
                        int r_12 = bits12[0] + bits12[1] + bits12[2] + bits12[3] ;
                        r_12 += 4 ;  //offset relative to position in level 0
                        r_12 = (r_02 << 2) + r_12 ;

                        int32_t * bits13 = (int32_t *) &cmp_13 ;
                        int r_13 = bits13[0] + bits13[1] + bits13[2] + bits13[3] ;
                        r_13 += 4 ;  //offset relative to position in level 0
                        r_13 = (r_03 << 2) + r_13 ;

                        int32_t * bits14 = (int32_t *) &cmp_14 ;
                        int r_14 = bits14[0] + bits14[1] + bits14[2] + bits14[3] ;
                        r_14 += 4 ;  //offset relative to position in level 0
                        r_14 = (r_04 << 2) + r_14 ;
                        //WE NOW HAVE R_1, THE RID FOR THE 1ST LEVEL


                        //now for level 2, key 1
                        int ci1 = r_11 + r_11/tree->node_capacity[1] ; //divided by fanout - 1
                        int ci2 = r_12 + r_12/tree->node_capacity[1] ; //divided by fanout - 1
                        int ci3 = r_13 + r_13/tree->node_capacity[1] ; //divided by fanout - 1
                        int ci4 = r_14 + r_14/tree->node_capacity[1] ; //divided by fanout - 1

                        int extra1 = (((int) k1[0] > (int) tree->key_array[2][(ci1 << 3)-1])? 0 : -8) ;
                        int extra2 = (((int) k2[0] > (int) tree->key_array[2][(ci2 << 3)-1])? 0 : -8) ;
                        int extra3 = (((int) k3[0] > (int) tree->key_array[2][(ci3 << 3)-1])? 0 : -8) ;
                        int extra4 = (((int) k4[0] > (int) tree->key_array[2][(ci4 << 3)-1])? 0 : -8) ;
                        //printf("extra: %d\n", extra) ;
                        //printf("k1[0]: %d, ind: %d\n", (int) k1[0], (int) tree->key_array[2][(ci << 3)-1]) ;
                        
                        __m128i lvl_2_A1 = _mm_load_si128((__m128i *) &tree->key_array[2][(ci1 << 3) + extra1]) ;
                        __m128i lvl_2_A2 = _mm_load_si128((__m128i *) &tree->key_array[2][(ci2 << 3) + extra2]) ;
                        __m128i lvl_2_A3 = _mm_load_si128((__m128i *) &tree->key_array[2][(ci3 << 3) + extra3]) ;
                        __m128i lvl_2_A4 = _mm_load_si128((__m128i *) &tree->key_array[2][(ci4 << 3) + extra4]) ;
                        //print128_numstr(lvl_2_A1, "lvl_2_A", 32);

                        __m128i lvl_2_B1 = _mm_load_si128((__m128i *) &tree->key_array[2][(ci1 << 3)+4+extra1]) ;
                        __m128i lvl_2_B2 = _mm_load_si128((__m128i *) &tree->key_array[2][(ci2 << 3)+4+extra2]) ;
                        __m128i lvl_2_B3 = _mm_load_si128((__m128i *) &tree->key_array[2][(ci3 << 3)+4+extra3]) ;
                        __m128i lvl_2_B4 = _mm_load_si128((__m128i *) &tree->key_array[2][(ci4 << 3)+4+extra4]) ;
                        //print128_numstr(lvl_2_B1, "lvl_2_B", 32);

                        __m128i cmp_2_A1 = _mm_cmpgt_epi32(lvl_2_A1, k1);
                        __m128i cmp_2_A2 = _mm_cmpgt_epi32(lvl_2_A2, k2);
                        __m128i cmp_2_A3 = _mm_cmpgt_epi32(lvl_2_A3, k3);
                        __m128i cmp_2_A4 = _mm_cmpgt_epi32(lvl_2_A4, k4);
                        //print128_numstr(cmp_2_A1, "cmp_2_A", 32);

                        __m128i cmp_2_B1 = _mm_cmpgt_epi32(lvl_2_B1, k1);
                        __m128i cmp_2_B2 = _mm_cmpgt_epi32(lvl_2_B2, k2);
                        __m128i cmp_2_B3 = _mm_cmpgt_epi32(lvl_2_B3, k3);
                        __m128i cmp_2_B4 = _mm_cmpgt_epi32(lvl_2_B4, k4);
                        //print128_numstr(cmp_2_B1, "cmp_2_B", 32);

                        __m128i cmp_21 = _mm_packs_epi32(cmp_2_A1, cmp_2_B1);
                        __m128i cmp_22 = _mm_packs_epi32(cmp_2_A2, cmp_2_B2);
                        __m128i cmp_23 = _mm_packs_epi32(cmp_2_A3, cmp_2_B3);
                        __m128i cmp_24 = _mm_packs_epi32(cmp_2_A4, cmp_2_B4);
                        //print128_numstr(cmp_21, "cmp_0", 16);

                        int16_t *bits21 = (int16_t *) &cmp_21 ;
                        int r_21 = bits21[0] + bits21[1] + bits21[2] + bits21[3] +
                                  bits21[4] + bits21[5] + bits21[6] + bits21[7] ;
                        r_21 += 8;
                        r_21 = (ci1 << 3) + r_21 + extra1;
                        //printf("r_2 is: %d\n", r_21) ;

                        int16_t *bits22 = (int16_t *) &cmp_22 ;
                        int r_22 = bits22[0] + bits22[1] + bits22[2] + bits22[3] +
                                  bits22[4] + bits22[5] + bits22[6] + bits22[7] ;
                        r_22 += 8;
                        r_22 = (ci2 << 3) + r_22 + extra2;

                        int16_t *bits23 = (int16_t *) &cmp_23 ;
                        int r_23 = bits23[0] + bits23[1] + bits23[2] + bits23[3] +
                                  bits23[4] + bits23[5] + bits23[6] + bits23[7] ;
                        r_23 += 8;
                        r_23 = (ci3 << 3) + r_23 + extra3;

                        int16_t *bits24 = (int16_t *) &cmp_24 ;
                        int r_24 = bits24[0] + bits24[1] + bits24[2] + bits24[3] +
                                  bits24[4] + bits24[5] + bits24[6] + bits24[7] ;
                        r_24 += 8;
                        r_24 = (ci4 << 3) + r_24 + extra4;

                        result[z] = r_21 + r_11 + r_01 ;
                        result[z+1] = r_22 + r_12 + r_02 ;
                        result[z+2] = r_23 + r_13 + r_03 ;
                        result[z+3] = r_24 + r_14 + r_04 ;

                }
        }
        else{
                int z;
                for (z = 0; z < num_probes; z++){
                        __m128i key = _mm_cvtsi32_si128(probe[z]);
                        key = _mm_shuffle_epi32(key, 0) ;
                        //printf("probe: %d, ", probe[z]) ;
                        //print128_numstr(key, "key", 32) ;

                        //iterate through levels
                        int lev = 0;
                        int lastr = 0 ;
                        int rtotal = 0;
                        for (lev = 0; lev < num_levels; lev++){
                                //int lvlsz = sizeof(tree->key_array[lev])/sizeof(tree->key_array[lev][0]) ;
                                int lvlsz = atoi(argv[3 + lev]) - 1 ;
                                //4 8 16
                                //printf("LEVEL SIZE IS: %d\n", lvlsz) ;
                                if(lvlsz == 4){
                                        int ci = 0, extra = 0 ;
                                        int nodenum  = tree->node_capacity[lev] ;
                                        if (lev > 0){
                                                ci = lastr + lastr/tree->node_capacity[lev-1] ;
                                                if (ci > 0)
                                                        extra = (((int) key[0] > (int) tree->key_array[lev][(ci*nodenum)-1])? 0 : -nodenum) ;
                                        }

                                        //load the level
                                        __m128i lvl = _mm_load_si128((__m128i *) &tree->key_array[lev][ci*nodenum + extra]) ;

                                        //compare the key with the level
                                        __m128i cmp = _mm_cmpgt_epi32(lvl, key);

                                        //get range id
                                        int32_t * bits = (int32_t *) &cmp ;
                                        int r = bits[0] + bits[1] + bits[2] + bits[3] ;
                                        r += nodenum ;  //offset relative to position in level 0
                                        r = (ci*nodenum) + r + extra;

                                        lastr = r;
                                        rtotal += r;

                                        //printf("level%d rid: %d\n", lev, r);
                                }
                                else if(lvlsz == 8){
                                        //printf("1\n");
                                        int ci = 0, extra = 0 ;
                                        int nodenum  = tree->node_capacity[lev] ;
                                        //printf("2\n");
                                        if (lev > 0){
                                                //printf("3\n");
                                                ci = lastr + lastr/tree->node_capacity[lev-1] ;
                                                //printf("%d\n", ci);
                                                if (ci > 0){
                                                        //printf("ci: %d\n", ci);
                                                        extra = (((int) key[0] > (int) tree->key_array[lev][(ci*nodenum)-1])? 0 : -nodenum) ;
                                                }
                                        }

                                        //load the level
                                        __m128i lvl = _mm_load_si128((__m128i *) &tree->key_array[lev][ci*nodenum + extra]) ;
                                        //printf("6\n");
                                        __m128i lvl2 = _mm_load_si128((__m128i *) &tree->key_array[lev][ci*nodenum + extra + 4]) ;
                                        //printf("7\n");

                                        //compare the key with the level
                                        __m128i cmp = _mm_cmpgt_epi32(lvl, key);
                                        //printf("8\n");
                                        __m128i cmp2 = _mm_cmpgt_epi32(lvl2, key);
                                        //printf("9\n");

                                        __m128i cmp12 =  _mm_packs_epi32(cmp, cmp2) ;
                                        //printf("10\n");


                                        //get range id
                                        int16_t * bits = (int16_t *) &cmp12 ;
                                        //printf("11\n");
                                        int r = bits[0] + bits[1] + bits[2] + bits[3] +
                                                bits[4] + bits[5] + bits[6] + bits[7];
                                        //printf("12\n");
                                        r += nodenum ;  //offset relative to position in level 0
                                        r = (ci*nodenum) + r + extra;

                                        lastr = r;
                                        rtotal += r;

                                        //printf("level%d rid: %d\n", lev, r);
                                }
                                else if(lvlsz == 16){
                                        //printf("######################IMPORTANT########################\n");
                                        int ci = 0, extra = 0 ;
                                        int nodenum  = tree->node_capacity[lev] ;
                                        //printf("nodenum: %d\n", nodenum);
                                        if (lev > 0){
                                                ci = lastr + lastr/tree->node_capacity[lev-1] ;
                                                //printf("ci:%d \n", ci) ;
                                                if (ci > 0)
                                                        extra = (((int) key[0] > (int) tree->key_array[lev][(ci*nodenum)-1])? 0 : -nodenum) ;
                                                //printf("extra: %d\n", extra) ;
                                        }

                                        //load the level
                                        __m128i lvl = _mm_load_si128((__m128i *) &tree->key_array[lev][ci*nodenum + extra]) ;
                                        //print128_numstr(lvl, "lvl", 32) ;
                                        __m128i lvl2 = _mm_load_si128((__m128i *) &tree->key_array[lev][ci*nodenum + extra + 4]) ;
                                        //print128_numstr(lvl2, "lvl2", 32) ;
                                        __m128i lvl3 = _mm_load_si128((__m128i *) &tree->key_array[lev][ci*nodenum + extra + 8] ) ;
                                        //print128_numstr(lvl3, "lvl3", 32) ;
                                        __m128i lvl4 = _mm_load_si128((__m128i *) &tree->key_array[lev][ci*nodenum + extra + 12] ) ;
                                        //print128_numstr(lvl4, "lvl4", 32) ;
                                        //printf("*******\n");

                                        //compare the key with the level
                                        __m128i cmp = _mm_cmpgt_epi32(lvl, key);
                                        //print128_numstr(cmp, "cmp", 32) ;
                                        __m128i cmp2 = _mm_cmpgt_epi32(lvl2, key);
                                        //print128_numstr(cmp2, "cmp2", 32) ;
                                        __m128i cmp3 = _mm_cmpgt_epi32(lvl3, key);
                                        //print128_numstr(cmp3, "cmp3", 32) ;
                                        __m128i cmp4 = _mm_cmpgt_epi32(lvl4, key);
                                        //print128_numstr(cmp4, "cmp4", 32) ;

                                        __m128i cmp12 = _mm_packs_epi32(cmp, cmp2);
                                        //print128_numstr(cmp12, "cmp12", 16) ;
                                        __m128i cmp34 = _mm_packs_epi32(cmp3, cmp4);
                                        //print128_numstr(cmp34, "cmp34", 16) ;

                                        __m128i cmp1234 = _mm_packs_epi16(cmp12, cmp34);
                                        //print128_numstr(cmp1234, "cmp1234", 8) ;

                                        //get range id
                                        int8_t * bits = (int8_t *) &cmp1234 ;
                                        int r = ((int) bits[0]) + ((int) bits[1]) + ((int)bits[2]) + ((int) bits[3]) +
                                                ((int) bits[4]) + ((int) bits[5]) + ((int)bits[6]) + ((int) bits[7]) + 
                                                ((int) bits[8]) + ((int) bits[9]) + ((int)bits[10]) + ((int) bits[11]) +
                                                ((int) bits[12]) + ((int) bits[13]) + ((int)bits[14]) + ((int) bits[15]); 

                                        
                                        //printf("these added makes r: %d\n",r) ;
                                        r += nodenum ;  //offset relative to position in level 0
                                        //printf("plus nodenum, r: %d\n", r) ;
                                        r = (ci*nodenum) + r + extra;
                                        //printf("plus extra and ci*nodenum, r: %d\n", r) ;

                                        lastr = r;
                                        rtotal += r;

                                        //printf("level%d rid: %d\n", lev, r);
                                        //printf("#############################################\n");
                                }
                        }
                        result[z] = rtotal ;
                        printf("probe: %d, rangeid: %d \n", probe[z], result[z]) ;
                }

        }


        /*// perform index probing (Phase 2)
        for (size_t i = 0; i < num_probes; ++i) {
                result[i] = probe_index(tree, probe[i]);
        }

        // output results
        for (size_t i = 0; i < num_probes; ++i) {
                fprintf(stdout, "%d %u\n", probe[i], result[i]);
        }*/

        // cleanup and exit
        free(result);
        free(probe);
        cleanup_index(tree);
        return EXIT_SUCCESS;
}