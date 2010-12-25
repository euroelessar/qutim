/****************************************************************************
 *  actiontoolbar.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "actiontoolbar.h"
#include <QAction>
#include <QToolButton>
#include <QMouseEvent>
#include "menucontroller_p.h"
#include "actiontoolbar_p.h"

namespace qutim_sdk_0_3
{	
	static SizeMap init_size_map()
	{
		SizeMap map;
		map.insert(16,QT_TRANSLATE_NOOP("ActionToolBar","Small (16x16)"));
		map.insert(22,QT_TRANSLATE_NOOP("ActionToolBar","Medium (22x22)"));
		map.insert(32,QT_TRANSLATE_NOOP("ActionToolBar","Big (32x32)"));
		map.insert(64,QT_TRANSLATE_NOOP("ActionToolBar","Huge (64x64)"));
		return map;
	}
	
	static SizeMap init_style_map()
	{
		SizeMap map;
		map.insert(Qt::ToolButtonIconOnly,QT_TRANSLATE_NOOP("ActionToolBar","Only display the icon"));
		map.insert(Qt::ToolButtonTextOnly,QT_TRANSLATE_NOOP("ActionToolBar","Only display the text"));
		map.insert(Qt::ToolButtonTextBesideIcon,QT_TRANSLATE_NOOP("ActionToolBar","The text appears beside the icon"));
		map.insert(Qt::ToolButtonTextUnderIcon,QT_TRANSLATE_NOOP("ActionToolBar","The text appears under the icon"));
		map.insert(Qt::ToolButtonFollowStyle,QT_TRANSLATE_NOOP("ActionToolBar","Follow the style"));
		return map;
	}
	
	SizeMap *sizeMap()
	{
		static QScopedPointer<SizeMap> list(new SizeMap(init_size_map()));
		return list.data();
	}
	
	SizeMap *styleMap()
	{
		static QScopedPointer<SizeMap> list(new SizeMap(init_style_map()));
		return list.data();		
	}
	
	QActionGroup *ActionToolBarPrivate::fillMenu(QMenu *menu,SizeMap *map, int current)
	{
		QActionGroup *group = new QActionGroup(menu);
		SizeMap::const_iterator it;
		for (it = map->constBegin();it != map->constEnd();it++) {
			QAction *act = new QAction(it.value(),menu);
			act->setCheckable(true);
			act->setData(it.key());
			act->setChecked(it.key() == current);
			menu->addAction(act);
			
			group->addAction(act);
		}
		group->setExclusive(true);
		return group;
	}
	
	ActionToolBarPrivate::ActionToolBarPrivate() :
		contextMenu(new QMenu(QT_TRANSLATE_NOOP("ActionToolBar","ToolBar appearance"))),
		id("common")
	{
	}
	
	void ActionToolBarPrivate::initContextMenu()
	{
		Q_Q(ActionToolBar);
		SizeMap::const_iterator it;
		QActionGroup *group;
		QMenu *size_menu = new QMenu(QT_TRANSLATE_NOOP("ActionToolBar","Icon size"),contextMenu);
		group = fillMenu(size_menu,sizeMap(),q->iconSize().height());
		contextMenu->addMenu(size_menu);
		connect(group,SIGNAL(triggered(QAction*)),SLOT(sizeActionTriggered(QAction*)));
		
		QMenu *style_menu = new QMenu(QT_TRANSLATE_NOOP("ActionToolBar","Tool button style"),contextMenu);
		group = fillMenu(style_menu,styleMap(),q->toolButtonStyle());
		contextMenu->addMenu(style_menu);
		connect(group,SIGNAL(triggered(QAction*)),SLOT(styleActionTriggered(QAction*)));
	}
	
	void ActionToolBarPrivate::sizeActionTriggered(QAction *action)
	{
		Q_Q(ActionToolBar);
		int size = action->data().toInt();
		q->setIconSize(QSize(size,size));
	}
	
	void ActionToolBarPrivate::styleActionTriggered(QAction *action)
	{
		Q_Q(ActionToolBar);
		Qt::ToolButtonStyle style = static_cast<Qt::ToolButtonStyle>(action->data().toInt());		
		q->setToolButtonStyle(style);
	}	
	
	ActionToolBarPrivate::~ActionToolBarPrivate()
	{
		contextMenu->deleteLater();
	}
	
	ActionToolBar::ActionToolBar(const QString &title, QWidget *parent)
			: QToolBar(title, parent), d_ptr(new ActionToolBarPrivate)
	{
		Q_D(ActionToolBar);
		d->q_ptr = this;
		d->moveHookEnabled = false;
		d->initContextMenu();
	}

	ActionToolBar::ActionToolBar(QWidget *parent)
			: QToolBar(parent), d_ptr(new ActionToolBarPrivate)
	{
		Q_D(ActionToolBar);
		d->q_ptr = this;
		d->moveHookEnabled = false;
		d->initContextMenu();
	}

	ActionToolBar::~ActionToolBar()
	{
		Q_D(ActionToolBar);
		qDeleteAll(d->actions);
	}
	
	QAction* ActionToolBar::insertAction(QAction* before, ActionGenerator* generator)
	{
		Q_D(ActionToolBar);
		Q_ASSERT(generator);
		int index = d->generators.indexOf(generator);
		if (index != -1)
			return d->actions.at(index);
		QAction *action = actionsCache()->value(generator).value(this);

		if (!action) {
			action = generator->generate<QAction>();
			Q_ASSERT(action);
			if(!action->parent())
				action->setParent(this);
			actionsCache()->operator[](generator).insert(this,action);
		}
		//action->setData(d->data);

		d->generators << generator;
		d->actions << action;
		bool hasMenu = !!action->menu();
		QWidget::insertAction(before,action);
		if (hasMenu) {
			QList<QToolButton *> buttons = findChildren<QToolButton*>();
			for (int i = buttons.size() - 1; i >= 0; i--) {
				if (buttons.at(i)->defaultAction() == action) {
					buttons.at(i)->setPopupMode(QToolButton::InstantPopup);
					break;
				}
			}
		}
		return action;
	}


	QAction *ActionToolBar::addAction(ActionGenerator *generator)
	{
		Q_ASSERT(generator);
		return insertAction(0,generator);
	}

	void ActionToolBar::setData(const QVariant &var)
	{
		Q_D(ActionToolBar);
		if (d->data == var)
			return;
		for (int i = 0; i < d->actions.size(); i++) {
			if (d->actions.at(i).isNull()) {
				d->actions.removeAt(i);
				d->generators.removeAt(i);
				i--;
			} else
				d->actions.at(i)->setData(var);
		}
	}

	QVariant ActionToolBar::data() const
	{
		return d_func()->data;
	}

	void ActionToolBar::mousePressEvent(QMouseEvent *event)
	{
		Q_D(ActionToolBar);
		if(d->moveHookEnabled && event->button() == Qt::LeftButton)
			d->dragPos = event->globalPos() - QWidget::window()->frameGeometry().topLeft();
	}

	void ActionToolBar::mouseMoveEvent(QMouseEvent *event)
	{
		Q_D(ActionToolBar);
		if(d->moveHookEnabled && event->buttons() & Qt::LeftButton)
			QWidget::window()->move(event->globalPos() - d->dragPos);
	}
	
	void ActionToolBar::contextMenuEvent(QContextMenuEvent* event)
	{
		Q_D(ActionToolBar);
		d->contextMenu->exec(event->globalPos());
	}


	void ActionToolBar::setMoveHookEnabled(bool enabled)
	{
		d_func()->moveHookEnabled = enabled;
	}
		
// 	void ActionToolBar::setId(const QString &id)
// 	{
// 		d_func()->id = id;
// 	}

}
