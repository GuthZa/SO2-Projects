#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX 256

int main(int argc, char* argv[]) {
	char result[MAX] = "Ol�! Este programa ainda n�o representa UNICODE\n";
	printf("Frase:%s\nTamanho (strlen):%d\nTamanho (sizeof):%d", result, strlen(result), sizeof(result));
	getchar();
	return 0;
}