#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <stdio.h>
#include <fcntl.h> 
#include <math.h>

#define TAM 1000
#define MAX_THREADS 20

typedef struct {
	DWORD start, end;
	DWORD* pValue;
	BOOLEAN cont;
} TDATA;

DWORD WINAPI somaPar(LPVOID data) {
	TDATA* ptd = (TDATA*)data;

	_tprintf(_T("\nSou a thread %d e tenho limite de %d a %d\n"), GetCurrentThreadId(), ptd->start, ptd->end);

	for (int i = ptd->start; i <= ptd->end; i++) {
		if (i % 3 == 0 && i != 0) {
			ptd->pValue++;
		}
	}

	return 0;
}

DWORD WINAPI multiplos(LPVOID data) {
	TDATA* ptd = (TDATA*)data;

	_tprintf(_T("Thread: %d - %d\n"), ptd->start, ptd->end);

	ExitThread(0);
}

// funcionalidade relacionada com temporiza��o
static double PerfCounterFreq;   // n ticks por seg.

void initClock() {
	LARGE_INTEGER aux;
	if (!QueryPerformanceFrequency(&aux))
		_tprintf(TEXT("\nSorry - No can do em QueryPerfFreq\n"));
	PerfCounterFreq = (double)(aux.QuadPart);   // / 1000.0;
	_tprintf(TEXT("\nTicks por sec.%f\n"), PerfCounterFreq);
}

__int64 startClock() {
	LARGE_INTEGER aux;
	QueryPerformanceCounter(&aux);
	return aux.QuadPart;
}

double stopClock(__int64 from) {
	LARGE_INTEGER aux;
	QueryPerformanceCounter(&aux);
	return (double)(aux.QuadPart - from) / PerfCounterFreq;
}

int _tmain(int argc, TCHAR** argv) {

	// matriz de handles das threads
	HANDLE hThreads[MAX_THREADS];

	// matriz de dados para as threads
	TDATA tData[MAX_THREADS];
	DWORD counter;

	// n�mero efectivo de threads
	int numThreads;

	// limite superior
	unsigned int superiorLimit;

	// vari�veis para cron�metro
	__int64 clockticks;
	double duration;

	unsigned int range;
	unsigned int inter;

#ifdef UNICODE
	
	(void) _setmode(_fileno(stdout), _O_WTEXT); 
	(void) _setmode(_fileno(stdin), _O_WTEXT);
	(void) _setmode(_fileno(stderr), _O_WTEXT);
#endif 

	initClock();
	_tprintf_s(TEXT("\nLimite sup. -> "));
	_tscanf_s(TEXT("%u"), &superiorLimit);
	_tprintf_s(TEXT("\nNum threads -> "));
	_tscanf_s(TEXT("%u"), &numThreads);
	if (numThreads > MAX_THREADS)
		numThreads = MAX_THREADS;

	// FAZER: prepara e cria threads
	//        manda as threads come�ar
	for (DWORD i = 0; i < numThreads; i++)
	{
		//valores de base p alterar
		tData[i].start = (superiorLimit/numThreads) * i + 1;
		tData[i].end = (superiorLimit / numThreads) * (i + 1);
		tData[i].cont = TRUE;
		tData[i].pValue = &counter;
		hThreads[i] = CreateThread(NULL, 0, multiplos, (LPVOID)&tData[i], 0, NULL);
	}

	clockticks = startClock();

	// FAZER: aguarda / controla as threads 
	//        manda as threads parar

	duration = stopClock(clockticks);
	_tprintf(TEXT("\nSegundos=%f\n"), duration);

	// FAZER: apresenta resultados

	// c�d. ref. para aguardar por uma tecla � caso fa�a falta
	// _tprintf(TEXT("\nCarregue numa tecla"));
	// _gettch();

	return 0;
}
// Este c�digo � apenas uma ajuda para o exerc�cio. Se quiser, mude-o.
