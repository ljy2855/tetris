#include <stdio.h>
#include <stdlib.h>

typedef struct _RecNode{
	char (*f)[10];
}RecNode;

RecNode * p;


int main(){

	char s[10][10];
	char (*f)[10];
	
	p = (RecNode*)malloc(sizeof(RecNode));


	int i,j;

	for(i=0; i < 10 ; i++){
		for(j = 0 ; j < 10 ; j++){
			s[i][j] = 2;

		}
	}


	p->f = s;
	
	s[1][3] = 1;
	for(i=0; i < 10 ; i++){
		for(j= 0 ; j < 10 ; j++){
			printf("%d",p->f[i][j]);
		}
	printf("\n");
	}
}
