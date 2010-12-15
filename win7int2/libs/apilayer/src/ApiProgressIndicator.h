#include <Windows.h>
#include "apiglobal.h"
#include "ApiProgressIndicatorStates.h"

EXPORT void SetProgressValEx(HWND hWnd, ULONGLONG Completed, ULONGLONG Total);
EXPORT void SetProgressVal  (HWND hWnd, int Percents);
EXPORT void ClearProgress   (HWND hWnd);
EXPORT void SetProgressState(HWND hWnd, ProgressStates state);
