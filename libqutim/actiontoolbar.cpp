/****************************************************************************
 *  actiontoolbar.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *  Copyright (c) 2010-2011 by Sidorov Aleksey <sauron@citadelspb.com> 
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
#include "config.h"
#include "actiongenerator_p.h"
#include <QShortcut>
#include "debug.h"
#ifdef Q_OS_WIN
# include <qt_windows.h>
#endif

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
	//TODO list sizes from IconBackend
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
		//act->setMenu(menu);
		act->setChecked(act->property("intData") == current);
		menu->addAction(act);
		group->addAction(act);
	}
	group->setExclusive(true);
	return group;
}

ActionToolBarPrivate::ActionToolBarPrivate()
{
	style = -1;
	size = QSize(-1,-1);
}

QMenu *ActionToolBarPrivate::initContextMenu()
{
	Q_Q(ActionToolBar);
	QMenu *contextMenu = new QMenu(q->tr("ToolBar appearance"));
	QActionGroup *group;
	QMenu *sizeMenu = new QMenu(q->tr("Icon size"), contextMenu);
	group = fillMenu(sizeMenu, sizeMap(), q->iconSize().height());
	contextMenu->addMenu(sizeMenu);
	q->connect(group,SIGNAL(triggered(QAction*)),
			   q, SLOT(_q_size_action_triggered(QAction*)));

	QMenu *styleMenu = new QMenu(q->tr("Tool button style"), contextMenu);
	group = fillMenu(styleMenu, styleMap(), q->toolButtonStyle());
	contextMenu->addMenu(styleMenu);
	q->connect(group,SIGNAL(triggered(QAction*)),
			   q, SLOT(_q_style_action_triggered(QAction*)));
	return contextMenu;
}

void ActionToolBarPrivate::_q_size_action_triggered(QAction *action)
{
	Q_Q(ActionToolBar);
	int s = action->property("intData").toInt();
	size = QSize(s,s);
	QToolBar *bar = q;
	bar->setIconSize(size);
	
	Config cfg = Config("appearance").group("toolBars").group(q->objectName());
	cfg.setValue("iconSize",s);
	cfg.sync();
}

void ActionToolBarPrivate::_q_style_action_triggered(QAction *action)
{
	Q_Q(ActionToolBar);
	style = action->property("intData").toInt();
	QToolBar *bar = q;
	bar->setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(style));
	
	Config cfg = Config("appearance").group("toolBars").group(q->objectName());
	cfg.setValue("buttonStyle",style);
	cfg.sync();
}

ActionToolBarPrivate::~ActionToolBarPrivate()
{
}

ActionToolBar::ActionToolBar(const QString &title, QWidget *parent)
	: QToolBar(title, parent), d_ptr(new ActionToolBarPrivate)
{
	Q_D(ActionToolBar);
	d->q_ptr = this;
	d->moveHookEnabled = false;
	setObjectName(QLatin1String("ActionToolBar"));
}

ActionToolBar::ActionToolBar(QWidget *parent)
	: QToolBar(parent), d_ptr(new ActionToolBarPrivate)
{
	Q_D(ActionToolBar);
	d->q_ptr = this;
	d->moveHookEnabled = false;
	setObjectName(QLatin1String("ActionToolBar"));
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
		generator->createImpl(action, this);
		actionsCache()->operator[](generator).insert(this,action);
	}
	//action->setData(d->data);

	d->generators << generator;
	d->actions << action;
	bool hasMenu = !!action->menu();
	QWidget::insertAction(before,action);
	if (hasMenu) {
		QToolButton *button = qobject_cast<QToolButton*>(widgetForAction(action));
		if (button) {
			button->setPopupMode(QToolButton::InstantPopup);
			//HACK a little spike for menu actions
			foreach (QKeySequence key, action->shortcuts()) {
				debug() << "added action:" << key;
				QShortcut *shortCut = new QShortcut(this);
				shortCut->setKey(key);
				connect(shortCut, SIGNAL(activated()), button, SLOT(click()));
			}
			action->setShortcut(QKeySequence());
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
	if (d->moveHookEnabled && event->button() == Qt::LeftButton) {
#ifndef Q_OS_WIN
		d->dragPos = event->globalPos() - QWidget::window()->frameGeometry().topLeft();
#else
		ReleaseCapture();
		SendMessage(this->window()->winId(), WM_SYSCOMMAND, SC_MOVE|HTCAPTION, 0);
		PostMessage(this->window()->winId(),  WM_LBUTTONUP, 0, 0);
#endif
	}
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
	QMenu *menu = d->initContextMenu();
	menu->setAttribute(Qt::WA_DeleteOnClose);
	menu->exec(event->globalPos());
}


void ActionToolBar::setMoveHookEnabled(bool enabled)
{
#if !(defined(Q_WS_X11) || defined(Q_WS_S60)) //use alt modificator instead!
	d_func()->moveHookEnabled = enabled;
#else
	Q_UNUSED(enabled);
#endif
}

void ActionToolBar::changeEvent(QEvent *e)
{
 	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
	{
		break;
	}
	default:
		break;
	}
}

void ActionToolBar::setIconSize(const QSize& iconSize)
{
	Q_D(ActionToolBar);
	if(!d->size.isValid())
		QToolBar::setIconSize(iconSize);
}

void ActionToolBar::setToolButtonStyle(Qt::ToolButtonStyle toolButtonStyle)
{
	Q_D(ActionToolBar);
	if(d->style == -1)
		QToolBar::setToolButtonStyle(toolButtonStyle);
}

void ActionToolBar::showEvent(QShowEvent* event)
{
	Q_D(ActionToolBar);
	QWidget::showEvent (event);
	Config cfg = Config("appearance").group("toolBars").group(objectName());
	int size = cfg.value("iconSize",-1);
	d->size = QSize(size,size);
	d->style = cfg.value("buttonStyle",-1);
	
	if(d->size.isValid())
		QToolBar::setIconSize(d->size);
	if(d->style != -1)
		QToolBar::setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(d->style));
}


// 	void ActionToolBar::setId(const QString &id)
// 	{
// 		d_func()->id = id;
// 	}

}

#include "actiontoolbar.moc"
