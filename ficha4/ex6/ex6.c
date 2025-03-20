#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include <math.h>
#define TAM_BLOCO 10000
#define TAM 200
#define MAX_THREADS 50

// Functionality related to timing
static double PerfCounterFreq;   // ticks per second

void initClock() {
    LARGE_INTEGER aux;
    if (!QueryPerformanceFrequency(&aux))
        _tprintf(TEXT("\nSorry - No can do in QueryPerfFreq\n"));
    PerfCounterFreq = (double)(aux.QuadPart);
    _tprintf(TEXT("\nTicks per sec: %f\n"), PerfCounterFreq);
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

// Data structure to control threads
typedef struct {
    DWORD* result;
    BOOL continua;
    HANDLE* hMutex;
    DWORD* pBloco;
} TDATA;

// Thread function
DWORD WINAPI SumPairs(LPVOID lpParam) {
    TDATA* data = (TDATA*)lpParam;
    DWORD res, i, bloco, inf, sup;
    while (data->continua)
    {
        WaitForSingleObject(data->hMutex, INFINITE);
        bloco = *data->pBloco;
        (*data->pBloco)++;
        ReleaseMutex(data->hMutex);
        inf = (bloco * TAM_BLOCO) + 1;
        sup = (bloco + 1) * TAM_BLOCO;
        _tprintf_s(TEXT("%d ... %d\n"), inf, sup);
        for (i = inf; i <= sup; i++) {
            if (i % 2 == 0) {
                WaitForSingleObject(data->hMutex, INFINITE);
                (*data->result)++;
                ReleaseMutex(data->hMutex);
            }
        }
        _tprintf_s(_T("\n"));
    }
    ExitThread(0);
}

// Maximum number of threads
#define MAX_THREADS 20

int _tmain(int argc, TCHAR* argv[]) {
#ifdef UNICODE
    (void)_setmode(_fileno(stdin), _O_WTEXT);
    (void)_setmode(_fileno(stdout), _O_WTEXT);
    (void)_setmode(_fileno(stderr), _O_WTEXT);
#endif 
    HANDLE hThreads[MAX_THREADS], hMutex, hEvent;
    TDATA tdata[MAX_THREADS];
    DWORD numthreads;
    TCHAR str[TAM]; 
    volatile DWORD result = 0; // Initialize result
    __int64 clockticks;
    double duration;

    hMutex = CreateMutex(NULL, FALSE, NULL);
    if (hMutex == NULL) {
        _tprintf_s(_T("Error creating the mutex"));
        return;
    }

    initClock();
    _tprintf_s(TEXT("\nNumber of threads -> "));
    _tscanf_s(TEXT("%d"), &numthreads);

    if (numthreads > MAX_THREADS)
        numthreads = MAX_THREADS;

    // Prepare and create threads
    for (DWORD i = 0; i < numthreads; i++) {
        tdata[i].result = &result;
        tdata[i].continua = TRUE;

        tdata[i].hMutex = &hMutex;

        hThreads[i] = CreateThread(NULL, 0, SumPairs, &tdata[i], 0, NULL);

        if (hThreads[i] == NULL) {
            _tprintf(TEXT("Error creating thread %d\n"), i);
            return 1; // Exit if thread creation fails
        }
    }

    clockticks = startClock();

    do
    {
        _tscanf_s(_T("%s"), str, TAM);
    } while (_tcscmp(str, _T("fim")) != 0);

    for (DWORD i = 0; i < numthreads; i++)
    {
        tdata[i].continua = FALSE;
    }

   // Wait for threads to complete
    WaitForMultipleObjects(numthreads, hThreads, TRUE, INFINITE);

    duration = stopClock(clockticks);

    _tprintf(TEXT("\nSeconds = %f\n"), duration);
    _tprintf(TEXT("\nSum = %d\n"), result);

    // Close thread handles
    for (int i = 0; i < numthreads; i++) {
        CloseHandle(hThreads[i]);
    }
    CloseHandle(hMutex);
    return 0;
}