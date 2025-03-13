#include <stdio.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>

#define TAM 100

int _tmain(int argc, TCHAR** argv) {

#ifdef _UNICODE
	(void) _setmode(_fileno(stdout), _O_WTEXT);
	(void) _setmode(_fileno(stdin), _O_WTEXT);
	(void) _setmode(_fileno(stderr), _O_WTEXT);
#endif

	TCHAR str[TAM];
	DWORD res;

	res = GetModuleFileName(NULL, str, TAM);
	if (res != 0) {
		//Shows the full path
		_tprintf_s(_T("Executável: \"%s\" (%d)"), str, res);
	}

	_tprintf(_T("Executável: \"%s\"\n"), argv[0]);
	return 0;
}