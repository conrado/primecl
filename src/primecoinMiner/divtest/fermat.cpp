#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <gmpxx.h>


#define __global
#define restrict

#include "cldefs.h"
void printbn(uint32_t id, mpzcl_t *mpz);
void printbn32(uint32_t id, mpzcl32_t *mpz);

#include "fermat.cl"
#include "fermatunrolled.cl"

using namespace std;

void copy_mpz(mpz_t n,mpzcl_t *ret, uint32_t idx){
	int i;
	int size = n->_mp_size;
//	printf("Size: %d\n", size);
	for(i=0; i < size; i++){
		D_REF(ret->d,i,idx) = n->_mp_d[i];
	}
	ret->size[idx] = size;
}

void printbn(uint32_t id, mpzcl_t *mpz){
	int i;
	int sz = mpz->size[id];
	printf("%2.2d: ", sz);
	for(i=0; i < sz; i++){
		printf("%16.16lX", D_REF(mpz->d,sz-1-i,id));	
	}
	printf("\n\n");
}

void printbn32(uint32_t id, mpzcl32_t *mpz){
	int i;
	int sz = mpz->size[id];
	printf("%2.2d: ", sz);
	for(i=0; i < sz; i++){
		printf("%8.8X", D_REF(mpz->d,sz-1-i,id));	
	}
	printf("\n\n");
}

int main(){
	fermatTemp_t *ftemp = (fermatTemp_t*)malloc(sizeof(fermatTemp_t));
	memset(ftemp,0xff,sizeof(fermatTemp_t));

	uint32_t idx=0;
	char line[512];
	FILE *f = fopen("input.primes","r");
	while(fgets(line,512,f)){
		mpz_t b,e,p,r; 
		mpz_init(p);
		mpz_init(b);
		mpz_init(e);
		mpz_init(r);
		int size=0;
		if(gmp_sscanf(line,"R: %d %Zx", &size, p)!=2)
			break;
		//Just for fun
#if 1
		mpz_set_ui(b,2);
		mpz_sub_ui(e,p,1);
		mpz_powm(r,b,e,p);
		//cout << hex << r << "\n";

#endif
		mpz_t one,inv,mone;
		mpz_init(one);
		mpz_init(inv);
		mpz_init(mone);
		mpz_set_ui(one,1);
		

		copy_mpz(p,&ftemp->mpzM,idx);
		mpz_sub_ui(mone,p,1);
		copy_mpz(mone,&ftemp->mpzE,idx++);		
		int k = norm(idx-1,&ftemp->mpzM) + 1;
		//printf("%d\n",2*k);
		mpz_mul_2exp(one,one,k*2);
		mpz_tdiv_q(inv,one,p);
	//	cout << "M: " << hex << p << "\n";		
	//	cout << "I: " << hex << r << "\n\n";		
	}
	printf("Loaded %d primes\n", idx);

	uint32_t i;
	for(i=2; i < 3; i++){
		powmBN(i,ftemp);
		printf("XbinU\n");
		printbn(i,&ftemp->mpzXbinU);
		printf("XbinV\n");
		printbn(i,&ftemp->mpzXbinV);

		//printbn(i,&ftemp->mpzInv);

		//printbn32(i,&ftemp->mpzH32);
		//printbn32(i,&ftemp->mpzM32);
		//printbn32(i,&ftemp->mpzV32);
		//printbn32(i,&ftemp->mpzBase);
		//printbn32(i,&ftemp->mpzResult);
		//printbn(i,&ftemp->mpzE);
		printf("Result:\n");
		printbn32(i,&ftemp->mpzResult);
		//printbn32(i,&ftemp->mpzBase);
	}

	return 0;
}
