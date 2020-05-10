#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
	char **s;
	char c[10];
	int i;
	int j;
	for(i=0;i<10;i++)
		c[i] =65;
	s= (char**)malloc(sizeof(char*)*22);
	for(i=0;i<22;i++)
	{
		s[i] = malloc(sizeof(char)*10);
		for(j=0 ;j <10 ; j++)
			s[i][j] = 65;
	}
	s[0][2] = 66;
	strncpy(s[1],s[0],10);
	printf("s[i] = %s \nc = %s \ncmp : %d\n",s[0],c,(int)strncmp(s[0],c,10));
	
	for(i=0;i < 22; i++)
		printf("%10s\n",s[i]);


	for(i= 21 ; i >= 0 ; i--){
		if(strncmp(s[i],c,10)==0){
			
				
			
			
		}
	}

}
