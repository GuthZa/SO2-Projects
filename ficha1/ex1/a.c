#include <stdio.h>

#define TAM 40

int main(int argc, char** argv) {
	char str[TAM] = "Ol� Mundo!";

	printf("%s\nsizeof: %d\nstrlen: %d", str, (int)sizeof(str), strlen(str));
}