#include "xtoolbar.h"
#include <QAction>
#include <libqutim/configbase.h>
#include "xsettingslayerimpl.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QDebug>

XToolBar::XToolBar ( QWidget* parent ) : QToolBar ( parent )
{
	//load config
	ConfigGroup toolbar_group = Config("appearence/xsettings").group("toolbar");
	uint icon_size = toolbar_group.value<uint>("iconSize",32);
	Qt::ToolButtonStyle tool_button_style = static_cast<Qt::ToolButtonStyle>(toolbar_group.value<int>("toolButtonStyle",Qt::ToolButtonTextUnderIcon));
	setIconSize(QSize(icon_size,icon_size));
	setToolButtonStyle(tool_button_style);
	//init context menu
	m_context_menu = new QMenu(tr("XBar appearence"));
	QAction *smallSize = new QAction(tr("Small (16x16)"),m_context_menu);
	smallSize->setCheckable(true);

	QAction *normalSize = new QAction(tr("Normal (32x32)"),m_context_menu);
	normalSize->setCheckable(true);

	QAction *bigSize = new QAction(tr("Big (48x48)"),m_context_menu);
	bigSize->setCheckable(true);

	QAction *otherSize = new QAction(tr("Other"),m_context_menu);
	otherSize->setCheckable(true);

	QActionGroup *sizeGroup =  new QActionGroup(this);
	sizeGroup->addAction(smallSize);
	sizeGroup->addAction(normalSize);
	sizeGroup->addAction(bigSize);
	sizeGroup->addAction(otherSize);

	QMenu *sizeMenu = new QMenu(tr("Icon size") ,m_context_menu);

	sizeMenu->addAction(smallSize);
	sizeMenu->addAction(normalSize);
	sizeMenu->addAction(bigSize);
	sizeMenu->addAction(otherSize);

	QAction *animated = new QAction(tr("Animated"),m_context_menu);
	ConfigGroup general_group = Config("appearence/xsettings").group("general");
	animated->setCheckable(true);
	animated->setChecked(general_group.value<bool>("animated",true));

	m_context_menu->addMenu(sizeMenu);
	m_context_menu->addAction(animated);


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


void XToolBar::contextMenuEvent(QContextMenuEvent* e)
{
	m_context_menu->exec(e->globalPos());
    QWidget::contextMenuEvent(e);
}
