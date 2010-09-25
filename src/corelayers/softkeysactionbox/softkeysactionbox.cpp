#include "softkeysactionbox.h"
#include <QHBoxLayout>
#include <QToolButton>
#include <QAction>
#include <libqutim/debug.h>

namespace Core
{

SoftkeysActionBoxModule::SoftkeysActionBoxModule()
{
}

void SoftkeysActionBoxModule::addAction(QAction *action)
{
	debug() << "SoftkeysActionBox Added action" << action;

	QWidget *p = this;
	while (p->parentWidget() && !p->isWindow()) {
		p = p->parentWidget();
	}
	p->addAction(action);
}

void SoftkeysActionBoxModule::removeAction(QAction *action)
{
	QWidget *p = this;
	while (p->parentWidget() && !p->isWindow()) {
		p = p->parentWidget();
	}
	p->removeAction(action);
}


}
