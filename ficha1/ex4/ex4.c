#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <string>
using namespace std;
//Permitir que o mesmo c�digo possa funcionar para ASCII ou UNICODE
#ifdef UNICODE
#define tcout wcout
#define tcin wcin
#define tstring wstring
#else
#define tcout cout
#define tcin cin
#define tstring string
#endif
int _tmain(int argc, LPTSTR argv[]) {
	tstring str = TEXT("Ol�! Este programa � para aceitar UNICODE. Insira \'fim\'
		para sair\n");
		//UNICODE: Por defeito, a consola Windows n�o processe caracteres wide.
		//A maneira mais f�cil para ter esta funcionalidade � chamar _setmode
#ifdef UNICODE
		_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif
	tcout << str;
	do {
		getline(tcin, str);
		//Mai�sculas
		for (unsigned int i = 0; i < str.length(); i++)
			str[i] = _totupper(str[i]);
		tcout << TEXT("Frase: ") << str << TEXT("Tamanho:") << str.length() <<
			endl;
	} while (str != TEXT("FIM"));
	return 0;
}