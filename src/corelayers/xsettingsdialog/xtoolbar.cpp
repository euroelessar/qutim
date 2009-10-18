#include "xtoolbar.h"
#include <QAction>

XToolBar::XToolBar ( QWidget* parent ) : QToolBar ( parent )
{
	connect(this,SIGNAL(actionTriggered(QAction*)),SLOT(onActionTriggered(QAction*)));
}


void XToolBar::onActionTriggered ( QAction* action )
{
	foreach (QAction *act, actions())
	{
		act->setChecked(false);
	}
	action->setChecked(true);
}
