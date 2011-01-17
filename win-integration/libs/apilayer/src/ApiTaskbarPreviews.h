#include "apiglobal.h"
#include <windows.h>

EXPORT void RegisterTab              (HWND tab, HWND owner);
EXPORT void SetTabActive             (HWND tab, HWND owner);
EXPORT void SetNoTabActive           (HWND owner); // SetTabActive(0, owner)
EXPORT void SetTabOrder              (HWND tab, HWND insertBefore);
EXPORT void UnregisterTab            (HWND tab);
EXPORT void SetTabIconicPreview      (HWND tab, HBITMAP bitmap);//
EXPORT void SetTabLivePreview        (HWND tab, HBITMAP bitmap);//
EXPORT void ForceIconicRepresentation(HWND tab);
EXPORT void InvalidateBitmaps        (HWND hwnd);
