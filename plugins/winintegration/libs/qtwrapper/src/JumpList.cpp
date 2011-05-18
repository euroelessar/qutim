#include "JumpList.h"

JumpList::JumpList(QObject *parent) :
    QObject(parent)
{
}

JumpList::~JumpList()
{

}

void JumpList::clearList()
{
	//qDeleteAll(m_items.values());
	qDeleteAll(m_tasks);
	m_items.clear();
	m_tasks.clear();
	sync();
}

void JumpList::sync()
{

}
