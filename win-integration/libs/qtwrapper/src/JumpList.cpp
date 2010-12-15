#include "JumpList.h"
#include "../../apilayer/src/ApiJumpList.h"

unsigned JumpList::maxSlotsCount = 0;

JumpList::JumpList(QObject *parent)
	: QObject(parent)
{

}

void JumpList::add(JumpListTask &task)
{
	wchar_t title[128], descr[256], path[512];
	title[task.title()  .toWCharArray(title)] = '\0';
	path [task.appPath().toWCharArray(path)]  = '\0';
	descr[task.tooltip().toWCharArray(descr)] = '\0';
	Task t;
	t.title       = title;
	t.description = descr;
	t.apppath     = path;
	t.icon = 0;
	t.args = 0;
	unsigned maxSlots;
	BeginList(&maxSlots, 0, 0);
	maxSlotsCount = maxSlots;
	AddTasks(1, &t);
	CommitList();
}

void JumpList::add(JumpListTasks &)
{

}

void JumpList::clear()
{
	ClearWholeList();
}

void JumpList::setAppId(const QString &id)
{
	wchar_t buff[512];
	buff[id.toWCharArray(buff)] = '\0';
	SetAppID(buff);
}
