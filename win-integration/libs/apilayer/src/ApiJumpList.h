#include <Windows.h>
#include "apiglobal.h"

#ifdef _MSC_VER
#	pragma pack(push, 4)
#endif
struct MINGWALIGN Task {
	wchar_t *title;
	wchar_t *description;
	wchar_t *args;
	wchar_t *icon;
	wchar_t *apppath;
};
#ifdef _MSC_VER
#	pragma pack(pop)
#endif

EXPORT void     SetAppID(const wchar_t*);
EXPORT void     AddTasks(unsigned count, Task *arr);
EXPORT void     ClearTasksList();
EXPORT void     ClearCustomDestinationsList();
EXPORT void	    ClearWholeList();
EXPORT bool     BeginList(unsigned *maxSlots, unsigned *count, Task *removedFromCategories); // count in= arrsize, count out= filled
EXPORT void     CommitList();
