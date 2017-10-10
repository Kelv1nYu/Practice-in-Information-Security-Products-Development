#include<stdio.h>

union lg
{
	short int i;
	char c;
}lg;

int main(){
	union lg example;
	example.i=0x1234;
	if(lg.c==0x12){
	printf("big-endian");}
	else{
	printf("little-endian");}
}
