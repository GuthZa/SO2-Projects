#include <stdio.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>

#define TAM 100

static int _tmain(int argc, TCHAR** argv) {

#ifdef _UNICODE
	(void) _setmode(_fileno(stdout), _O_WTEXT);
	(void) _setmode(_fileno(stdin), _O_WTEXT);
	(void) _setmode(_fileno(stderr), _O_WTEXT);
#endif

	TCHAR str[TAM];
	DWORD res = 0;
	BOOL processReturn;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	// These are equal
	// memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);

	do
	{
		_tprintf_s(_T("Programa a executar:"));
		_fgetts(str, TAM, stdin);

		str[_tcslen(str) - 1] = '\0';

		_tprintf_s(_T("Vou executar o programa %s\n"), str);
		processReturn = CreateProcess(NULL, str, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &res);

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		_tprintf_s(_T("Eu PID=%d, criei o filho %d... terminou com %d\n"), GetCurrentProcessId(), pi.dwProcessId, res);

	} while (_tcscmp(_T("fim"), str) != 0);

	//Ex5 a)
	/*
	res = GetModuleFileName(NULL, str, TAM);
	if (res != 0) {
		//Shows the full path
		_tprintf_s(_T("Executável: \"%s\" (%d)"), str, res);
	}

	_tprintf(_T("Executável: \"%s\"\n"), argv[0]);
	*/
	return 0;
}