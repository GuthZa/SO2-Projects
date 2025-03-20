#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include <math.h>
#define BLOCK_SIZE 10000
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
    BOOL cont;
    HANDLE* hEvent;
    DWORD* pBlock;
} TDATA;

DWORD WINAPI SumPairs(LPVOID lpParam) {
    TDATA* pdt = (TDATA*)lpParam;
    DWORD res, i, block, lowerBound, upperBound;

    while (pdt->cont)
    {
        ResetEvent(pdt->hEvent);
        block = *pdt->pBlock;
        (*pdt->pBlock)++;
        SetEvent(pdt->hEvent);
        lowerBound = (block * BLOCK_SIZE) + 1;
        upperBound = (block + 1) * BLOCK_SIZE;
        _tprintf_s(TEXT("%d ... %d\n"), lowerBound, upperBound);
        for (i = lowerBound; i <= upperBound; i++) {
            if (i % 2 == 0) {
                ResetEvent(pdt->hEvent);
                (*pdt->result)++;
                SetEvent(pdt->hEvent);
            }
        }
        Sleep(3000);
        _tprintf_s(_T("\n"));
    }

    ExitThread(0);
}

int _tmain(int argc, TCHAR** argv) {
#ifdef UNICODE
    (void)_setmode(_fileno(stdin), _O_WTEXT);
    (void)_setmode(_fileno(stdout), _O_WTEXT);
    (void)_setmode(_fileno(stderr), _O_WTEXT);
#endif 
    HANDLE hThreads[MAX_THREADS], hEvent;
    TDATA tData[MAX_THREADS];
    //Do I really want to create as many data as threads?
    //Shouldn't this be shared?
    TCHAR str[TAM];
    DWORD numThreads = 0, block = 0;
    volatile DWORD result = 0;
    __int64 clockticks;
    double duration;
    
    hEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
    if (hEvent == NULL)
    {
        _tprintf_s(_T("Error creating the event"));
        return 1;
    }

    initClock();
    _tprintf_s(TEXT("\nNumber of threads -> "));
    _tscanf_s(TEXT("%d"), &numThreads);

    if (numThreads > MAX_THREADS) 
        numThreads = MAX_THREADS; 

    //Setting up information and creation of threads
    for (DWORD i = 0; i < numThreads; i++)
    {
        tData[i].result = &result;
        tData[i].hEvent = &hEvent;
        tData[i].pBlock = &block;
        tData[i].cont = TRUE;

        hThreads[i] = CreateThread(NULL, 0, SumPairs, &tData[i], 0, NULL);

        if (hThreads[i] == NULL) {
            _tprintf(_T("Error creating the threads %d\n"), i);
            CloseHandle(hEvent);
            return 1;
        }
    }

    clockticks = startClock();
    do
    {
        _tscanf_s(_T("%s"), str, TAM);
        str[TAM - 1] = '\0';
    } while (_tcscmp(str, _T("fim")) != 0);

    //Signals all threads to stop
    for (DWORD i = 0; i < numThreads; i++)
    {
        tData[i].cont = FALSE;
    }

    // Stops executions until all threads have finished
    // Waits for threads to complete
    WaitForMultipleObjects(numThreads, hThreads, TRUE, INFINITE);

    duration = stopClock(clockticks);

    _tprintf(TEXT("\nSeconds = %f\n"), duration);
    _tprintf(TEXT("\nSum = %d\n"), result);

    // Close thread handles
    for (int i = 0; i < numThreads; i++)
        CloseHandle(hThreads[i]);
    CloseHandle(hEvent);

    return 0;
}