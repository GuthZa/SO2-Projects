#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include <math.h>
#define TAM 200

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
    DWORD limsup;
    DWORD liminf;
    DWORD* result;
    //CRITICAL_SECTION* cs;
    BOOL continua;
    //HANDLE* hMutex;
    HANDLE* hEvent;

} TDATA;

// Thread function
DWORD WINAPI SumPairs(LPVOID lpParam) {
    TDATA* data = (TDATA*)lpParam;
    DWORD res, i;

    _tprintf_s(TEXT("%d ... %d\n"), data->liminf, data->limsup);
    while (data->continua) {
        for (i = data->liminf; i <= data->limsup; i++) {
            if (i % 3 == 0) {
                // Critical section - mutex
                //EnterCriticalSection(data->cs)
                //res = WaitForSingleObject(data->hMutex, INFINITE);
                res = WaitForSingleObject(data->hEvent, INFINITE);
                (*data->result)++;
                //LeaveCriticalSection(data->cs);
                //ReleaseMutex(data->hMutex);
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
    DWORD limsup;
    TCHAR str[TAM];
    volatile DWORD result = 0; // Initialize result
    __int64 clockticks;
    double duration;

    //CRITICAL_SECTION cs;

    //InitializeCriticalSection(&cs);

    initClock();
    _tprintf_s(TEXT("\nUpper limit -> "));
    _tscanf_s(TEXT("%d"), &limsup);
    _tprintf_s(TEXT("\nNumber of threads -> "));
    _tscanf_s(TEXT("%d"), &numthreads);

    if (numthreads > MAX_THREADS)
        numthreads = MAX_THREADS;

    /*hMutex = CreateMutex(NULL, FALSE, NULL);
    if (hMutex == NULL) {
        _tprintf_s(_T("Error creating the mutex"));
        return;
    }*/
    hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (hEvent == NULL) {
        _tprintf_s(_T("Error creating the event"));
        return;
    }

    // Prepare and create threads
    for (int i = 0; i < numthreads; i++) {
        tdata[i].liminf = 1 + (limsup / numthreads) * i;
        tdata[i].limsup = (limsup / numthreads) * (i + 1);
        if (i == numthreads - 1) { // Ensure the last thread covers the remaining numbers
            tdata[i].limsup = limsup;
        }
        tdata[i].result = &result;
        tdata[i].continua = TRUE;

        tdata[i].hEvent = &hEvent;
        //tdata[i].hMutex = &hMutex;
        //tdata[i].cs = &cs; // Pass the address of the critical section

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
    SetEvent(hEvent);

    // Wait for threads to complete
    WaitForMultipleObjects(numthreads, hThreads, TRUE, INFINITE);

    duration = stopClock(clockticks);

    _tprintf(TEXT("\nSeconds = %f\n"), duration);
    _tprintf(TEXT("\nSum = %d\n"), result);

    // Close thread handles
    for (int i = 0; i < numthreads; i++) {
        CloseHandle(hThreads[i]);
    }
    CloseHandle(hEvent);
    //CloseHandle(hMutex);
    //DeleteCriticalSection(&cs);
    return 0;
}