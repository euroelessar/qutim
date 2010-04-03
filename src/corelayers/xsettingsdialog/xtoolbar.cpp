/****************************************************************************
 *  xtoolbar.cpp
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "xtoolbar.h"
#include <QAction>
#include <libqutim/configbase.h>
#include "xsettingslayerimpl.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QDebug>

XToolBar::XToolBar ( QWidget* parent ) : ActionToolBar ( parent )
{
	//load config
	ConfigGroup toolbar_group = Config("appearance").group("xsettings/toolbar");
	uint icon_size = toolbar_group.value<uint>("iconSize",32);
	Qt::ToolButtonStyle tool_button_style = static_cast<Qt::ToolButtonStyle>(toolbar_group.value<int>("toolButtonStyle",Qt::ToolButtonTextUnderIcon));
	setIconSize(QSize(icon_size,icon_size));
	setToolButtonStyle(tool_button_style);
	//init context menu
	m_context_menu = new QMenu(tr("XBar appearance"));
	//init size menu
	QAction *smallSize = new QAction(tr("Small (16x16)"),m_context_menu);
	smallSize->setCheckable(true);
	smallSize->setProperty("iconSize",16);

	QAction *normalSize = new QAction(tr("Normal (32x32)"),m_context_menu);
	normalSize->setCheckable(true);
	normalSize->setProperty("iconSize",32);

	QAction *bigSize = new QAction(tr("Big (48x48)"),m_context_menu);
	bigSize->setCheckable(true);
	bigSize->setProperty("iconSize",48);

	QAction *otherSize = new QAction(tr("Other"),m_context_menu);
	otherSize->setCheckable(true);
	otherSize->setProperty("iconSize",icon_size);

	int currentSizePos = getSizePos(icon_size);
	QActionGroup *sizeGroup =  new QActionGroup(this);
	sizeGroup->addAction(smallSize);
	sizeGroup->addAction(normalSize);
	sizeGroup->addAction(bigSize);
	sizeGroup->addAction(otherSize);
	sizeGroup->actions().at(currentSizePos)->setChecked(true);
	connect(sizeGroup,SIGNAL(triggered(QAction*)),SLOT(onSizeActionTriggered(QAction*)));

	QMenu *sizeMenu = new QMenu(tr("Icon size") ,m_context_menu);

	sizeMenu->addAction(smallSize);
	sizeMenu->addAction(normalSize);
	sizeMenu->addAction(bigSize);
	sizeMenu->addAction(otherSize);

	//init toolbutton style menu
	QAction *toolButtonIconOnly = new QAction(tr("Only display the icon"),m_context_menu);
	toolButtonIconOnly->setCheckable(true);
	toolButtonIconOnly->setProperty("toolButtonStyle",Qt::ToolButtonIconOnly);

	QAction *toolButtonTextOnly = new QAction(tr("Only display the text"),m_context_menu);
	toolButtonTextOnly->setCheckable(true);
	toolButtonTextOnly->setProperty("toolButtonStyle",Qt::ToolButtonTextOnly);

	QAction *toolButtonTextBesideIcon = new QAction(tr("The text appears beside the icon"),m_context_menu);
	toolButtonTextBesideIcon->setCheckable(true);
	toolButtonTextBesideIcon->setProperty("toolButtonStyle",Qt::ToolButtonTextBesideIcon);

	QAction *toolButtonTextUnderIcon = new QAction(tr("The text appears under the icon"),m_context_menu);
	toolButtonTextUnderIcon->setCheckable(true);
	toolButtonTextUnderIcon->setProperty("toolButtonStyle",Qt::ToolButtonTextUnderIcon);

	QAction *toolButtonFollowStyle = new QAction(tr("Follow the style"),m_context_menu);
	toolButtonFollowStyle->setCheckable(true);
	toolButtonFollowStyle->setProperty("toolButtonStyle",Qt::ToolButtonFollowStyle);

	QActionGroup *toolButtonStyleGroup = new QActionGroup(this);
	toolButtonStyleGroup->addAction(toolButtonTextOnly);
	toolButtonStyleGroup->addAction(toolButtonIconOnly);
	toolButtonStyleGroup->addAction(toolButtonTextBesideIcon);
	toolButtonStyleGroup->addAction(toolButtonTextUnderIcon);
	toolButtonStyleGroup->addAction(toolButtonFollowStyle);
	toolButtonStyleGroup->actions().at(tool_button_style)->setChecked(true);

	QMenu *toolButtonStyleMenu = new QMenu(tr("Tool button style"),m_context_menu);
	toolButtonStyleMenu->addAction(toolButtonTextOnly);
	toolButtonStyleMenu->addAction(toolButtonIconOnly);
	toolButtonStyleMenu->addAction(toolButtonTextBesideIcon);
	toolButtonStyleMenu->addAction(toolButtonTextUnderIcon);
	toolButtonStyleMenu->addAction(toolButtonFollowStyle);
	connect(toolButtonStyleGroup,SIGNAL(triggered(QAction*)),SLOT(onToolButtonStyleActionTriggered(QAction*)));

	m_context_menu->addMenu(sizeMenu);
	m_context_menu->addMenu(toolButtonStyleMenu);

}


void XToolBar::contextMenuEvent(QContextMenuEvent* e)
{
	m_context_menu->exec(e->globalPos());
    QWidget::contextMenuEvent(e);
}

int XToolBar::getSizePos(const int& size)
{
	int sizePos;
	switch (size) {
		case 16:
			sizePos = 0;
			break;
		case 32:
			sizePos = 1;
			break;
		case 48:
			sizePos = 2;
			break;
		default:
			sizePos =3;
			break;
	}
	return sizePos;
}


void XToolBar::onSizeActionTriggered(QAction* action)
{
	int iconSize = action->property("iconSize").toInt();
	setIconSize(QSize(iconSize,iconSize));
	ConfigGroup toolbar_group = Config("appearance").group("xsettings/toolbar");
	toolbar_group.setValue("iconSize",iconSize);
	toolbar_group.sync();
}


void XToolBar::onToolButtonStyleActionTriggered(QAction* action)
{
	Qt::ToolButtonStyle style = static_cast<Qt::ToolButtonStyle>(action->property("toolButtonStyle").toInt());
	setToolButtonStyle(style);
	ConfigGroup toolbar_group = Config("appearance").group("xsettings/toolbar");
	toolbar_group.setValue("toolButtonStyle",style);
	toolbar_group.sync();	
}
