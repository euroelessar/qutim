/*#include "ApiJumpList.h"
#include <atlbase.h>
#include <ShObjIdl.h>
#include <propvarutil.h>
#include <Propkey.h>
#include <wchar.h>
#include <xstring>

#include <atlbase.h>
#include <comdef.h>

using std::wstring;

static wstring appID = L"";
static ICustomDestinationList *gjlist = 0;
/*
void SetAppID(const wchar_t *id)
{
	//	SegFault(); // Don't reset AppID in runtime, zplease!
	appID = id;
	SetCurrentProcessExplicitAppUserModelID(id);
}

void AddTasks(unsigned count, Task *arr)
{
	if(!gjlist)
		return;
	IObjectCollection *tasks; // Array of tasklinks.
	if(S_OK != CoCreateInstance(CLSID_EnumerableObjectCollection, 0, CLSCTX_INPROC_SERVER, IID_IObjectCollection, (void**)&tasks))
		return;

	for(unsigned i = 0; i < count; i++){ // Filling in.
		IShellLinkW *link;
		CoCreateInstance(CLSID_ShellLink, 0, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void**)&link);

		link->SetArguments(arr[i].args);
		link->SetPath(arr[i].apppath);
		link->SetDescription(arr[i].description);

		PROPVARIANT pv;
		IPropertyStore *prop;
		link->QueryInterface(IID_IPropertyStore, (void**)&prop);
		InitPropVariantFromString(arr[i].title, &pv);
		prop->SetValue(PKEY_Title, pv); // Setting title for new link. %)
		PropVariantClear(&pv);
		prop->Commit();

		tasks->AddObject(link);

		prop->Release();
		link->Release();
	}

	IObjectArray *tasksArray;
	tasks->QueryInterface(IID_IObjectArray, (void**)&tasksArray);
	gjlist->AddUserTasks(tasksArray);
	tasksArray->Release();
}

bool BeginList(unsigned *maxSlots, unsigned *count, Task *removedFromCategories)
{
	if(gjlist)
		return false;

	if(S_OK != CoCreateInstance(CLSID_DestinationList, 0, CLSCTX_INPROC_SERVER, IID_ICustomDestinationList, (void**)&gjlist))
		return false;
	gjlist->SetAppID(appID.c_str());

	unsigned removedArrSize = 0, dummy;
	IObjectArray *arr;

	if(maxSlots)
		gjlist->BeginList(maxSlots, IID_IObjectArray, (void**)&arr);
	else
		gjlist->BeginList(&dummy, IID_IObjectArray, (void**)&arr);
	
	if(count){
		removedArrSize = *count;
		arr->GetCount(count);
	}

	if(count)
		if(removedArrSize < *count && removedFromCategories)
			return false;
		else
			for(unsigned i = 0; i < *count; i++){
				//removedFromCategories[i].title = '1';
				// TODO: заполнять
			}

	return true;
}

void CommitList()
{
	if(gjlist){
		gjlist->CommitList();
		gjlist->Release();
		gjlist = 0;
	}
}

void ClearWholeList()
{
	bool alreadyCreated = BeginList(0, 0, 0);
	long l = gjlist->DeleteList(appID.c_str());
	if(alreadyCreated)
		CommitList();
}
*/
