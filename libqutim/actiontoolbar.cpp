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
	static ActionGenerator *createGenerator(int data, const LocalizedString &text)
	{
		ActionGenerator *action = new ActionGenerator(QIcon(), text, 0);
		action->addProperty("intData", data);
		action->setCheckable(true);
		return action;
	}

	static SizeList init_size_map()
	{
		SizeList list;
		list << createGenerator(16, QT_TRANSLATE_NOOP("ActionToolBar","Small (16x16)"));
		list << createGenerator(22, QT_TRANSLATE_NOOP("ActionToolBar","Medium (22x22)"));
		list << createGenerator(32, QT_TRANSLATE_NOOP("ActionToolBar","Big (32x32)"));
		list << createGenerator(64, QT_TRANSLATE_NOOP("ActionToolBar","Huge (64x64)"));
		return list;
	}
	
	static SizeList init_style_map()
	{
		SizeList list;
		list << createGenerator(Qt::ToolButtonIconOnly,
		                        QT_TRANSLATE_NOOP("ActionToolBar","Only display the icon"));
		list << createGenerator(Qt::ToolButtonTextOnly,
		                        QT_TRANSLATE_NOOP("ActionToolBar","Only display the text"));
		list << createGenerator(Qt::ToolButtonTextBesideIcon,
		                        QT_TRANSLATE_NOOP("ActionToolBar","The text appears beside the icon"));
		list << createGenerator(Qt::ToolButtonTextUnderIcon,
		                        QT_TRANSLATE_NOOP("ActionToolBar","The text appears under the icon"));
		list << createGenerator(Qt::ToolButtonFollowStyle,
		                        QT_TRANSLATE_NOOP("ActionToolBar","Follow the style"));
		return list;
	}
	
	Q_GLOBAL_STATIC_WITH_INITIALIZER(SizeList, sizeMap, *x = init_size_map())
	Q_GLOBAL_STATIC_WITH_INITIALIZER(SizeList, styleMap, *x = init_style_map())
	
	QActionGroup *ActionToolBarPrivate::fillMenu(QMenu *menu, SizeList *map, int current)
	{
		QActionGroup *group = new QActionGroup(menu);
		SizeList::const_iterator it;
		for (it = map->constBegin(); it != map->constEnd(); it++) {
			QAction *act = (*it)->generate<QAction>();
			act->setMenu(menu);
			act->setChecked(act->property("intData") == current);
			menu->addAction(act);
			group->addAction(act);
		}
		group->setExclusive(true);
		return group;
	}
	
	ActionToolBarPrivate::ActionToolBarPrivate() : id("common")
	{
	}
	
	void ActionToolBarPrivate::initContextMenu()
	{
		Q_Q(ActionToolBar);
		contextMenu = new QMenu(tr("ToolBar appearance"));
		QActionGroup *group;
		sizeMenu = new QMenu(tr("Icon size"), contextMenu);
		group = fillMenu(sizeMenu, sizeMap(), q->iconSize().height());
		contextMenu->addMenu(sizeMenu);
		connect(group,SIGNAL(triggered(QAction*)),SLOT(sizeActionTriggered(QAction*)));
		
		styleMenu = new QMenu(tr("Tool button style"), contextMenu);
		group = fillMenu(styleMenu, styleMap(), q->toolButtonStyle());
		contextMenu->addMenu(styleMenu);
		connect(group,SIGNAL(triggered(QAction*)),SLOT(styleActionTriggered(QAction*)));
	}
	
	void ActionToolBarPrivate::sizeActionTriggered(QAction *action)
	{
		Q_Q(ActionToolBar);
		int size = action->property("intData").toInt();
		q->setIconSize(QSize(size,size));
	}
	
	void ActionToolBarPrivate::styleActionTriggered(QAction *action)
	{
		Q_Q(ActionToolBar);
		Qt::ToolButtonStyle style = static_cast<Qt::ToolButtonStyle>(action->property("intData").toInt());		
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
	
	void ActionToolBar::changeEvent(QEvent *e)
	{
	    QWidget::changeEvent(e);
	    switch (e->type()) {
	    case QEvent::LanguageChange:
		{
			Q_D(ActionToolBar);
			d->contextMenu->setTitle(tr("ToolBar appearance"));
			d->sizeMenu->setTitle(tr("Icon size"));
			d->styleMenu->setTitle(tr("Tool button style"));
	        break;
		}
	    default:
	        break;
	    }
	}
		
// 	void ActionToolBar::setId(const QString &id)
// 	{
// 		d_func()->id = id;
// 	}

}
