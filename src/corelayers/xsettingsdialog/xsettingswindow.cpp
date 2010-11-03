/****************************************************************************
 *  xsettingswindow.cpp
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
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

#include "xsettingswindow.h"
#include <QListWidget>
#include <QStackedWidget>
#include <qutim/actiontoolbar.h>
#include <QSplitter>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <qutim/icon.h>
#include <qutim/settingswidget.h>
#include <qutim/config.h>
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>
#include <QCloseEvent>

namespace Core
{

struct XSettingsWindowPrivate
{
	ActionToolBar *toolBar;
	QActionGroup *group;
	QStackedWidget *stackedWidget;
	QListWidget *listWidget;
	QDialogButtonBox *buttonBox;
	QSplitter *splitter;
	QObject *controller;
	QMap<Settings::Type,SettingsItem*> items;
	QList<SettingsWidget*> modifiedWidgets;
	QMap<Settings::Type,QAction*> actionMap;
	QAction *currentAction;
};
	
XSettingsWindow::XSettingsWindow(const qutim_sdk_0_3::SettingsItemList& settings, QObject* controller) :
	p(new XSettingsWindowPrivate)
{
	setAttribute(Qt::WA_DeleteOnClose);
	p->controller = controller;
	//setup ui
	QWidget *w = new QWidget(this);
	QVBoxLayout *l = new QVBoxLayout(w);
	Config cfg;
	cfg.beginGroup("xsettings/window");
	QByteArray data = cfg.value("geometry", QByteArray());
	if (data.isEmpty() || !restoreGeometry(data)) {
		QSize desktopSize = QApplication::desktop()->availableGeometry(QCursor::pos()).size();
		resize(desktopSize.width() / 2, desktopSize.height() * 2 / 3);
		centerizeWidget(this);
	}
	//init widgets
	p->splitter = new QSplitter(Qt::Horizontal,w);
	p->listWidget = new QListWidget(w);

	p->stackedWidget = new QStackedWidget(w);
	//default widget
	QWidget *empty = new QWidget(this);

	p->stackedWidget->addWidget(empty);
	p->splitter->addWidget(p->listWidget);
	p->splitter->addWidget(p->stackedWidget);
	data = cfg.value("splitterState", QByteArray());
	if (data.isEmpty() || !p->splitter->restoreState(data))
		p->splitter->setSizes(QList<int>() << 80  << 250);
	l->addWidget(p->splitter);

	p->buttonBox = new QDialogButtonBox(QDialogButtonBox::Save|QDialogButtonBox::Cancel,Qt::Horizontal,w);
	l->addWidget(p->buttonBox);
	p->buttonBox->hide();
	//init actiontoolbar
#ifdef Q_WS_MAC
	l->setContentsMargins(3,6,3,3);
	p->listWidget->setIconSize(QSize(22,22));
#endif
	setCentralWidget(w);
	p->toolBar = new ActionToolBar(w);
	p->toolBar->setIconSize(QSize(32,32));
	p->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	addToolBar(Qt::TopToolBarArea,p->toolBar);
	setUnifiedTitleAndToolBarOnMac(true);
	p->toolBar->setMovable(false);
	p->group = new QActionGroup(w);
	p->group->setExclusive(true);
	//connections
	connect(p->group,SIGNAL(triggered(QAction*)),SLOT(onGroupActionTriggered(QAction*)));
	connect(p->listWidget,
			SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
			SLOT(onCurrentItemChanged(QListWidgetItem*))
		   );
	connect(p->buttonBox,SIGNAL(accepted()),SLOT(save()));
	connect(p->buttonBox,SIGNAL(rejected()),SLOT(cancel()));
	loadSettings(settings);
}

	
void XSettingsWindow::update(const qutim_sdk_0_3::SettingsItemList& settings)
{
	foreach (SettingsItem *item, (p->items.values().toSet() -= settings.toSet()))
		item->clearWidget();
	p->items.clear();
	loadSettings(settings);
}

XSettingsWindow::~XSettingsWindow()
{
	Config cfg;
	cfg.beginGroup("xsettings/window");
	cfg.setValue("geometry", saveGeometry());
	cfg.setValue("splitterState", p->splitter->saveState());
}

void XSettingsWindow::loadSettings(const qutim_sdk_0_3::SettingsItemList& settings)
{
	foreach (SettingsItem *item,settings) {
		//QListWidgetItem *list_item = new QListWidgetItem(item->icon(),item->text(),p->listWidget);
		p->items.insertMulti(item->type(),item);
	}
	ensureActions();
	if (p->group->actions().count())
		p->group->actions().first()->trigger();
}

QAction* XSettingsWindow::get(Settings::Type type)
{
	QAction *action = p->actionMap.value(type);
	if (!action) {
		action = new QAction(this);
		action->setText(Settings::getTypeTitle(type));
		action->setIcon(Settings::getTypeIcon(type));
		action->setCheckable(true);
		p->actionMap.insert(type,action);
		p->group->addAction(action);
	}
	return action;
}


void XSettingsWindow::ensureActions()
{
	qDeleteAll(p->actionMap);
	p->actionMap.clear();
	foreach (Settings::Type type,p->items.keys()) {
		QAction *a = get(type);
		//small spike
		if (!p->toolBar->actions().contains(a)) {
			p->toolBar->addAction(a);
			if (type == Settings::General)
				p->toolBar->addSeparator();
		}
	}
	p->toolBar->setVisible(p->actionMap.count() > 1);
}

void XSettingsWindow::onGroupActionTriggered(QAction *a )
{
	if (p->currentAction == a)
		return;
	p->currentAction = a;
	//remove old settings widgets
	QWidget *c = p->stackedWidget->widget(0);
	for (int index = 0;index!=p->stackedWidget->count();index++) {
		SettingsWidget *w = static_cast<SettingsWidget*>(p->stackedWidget->widget(index));
		if (w != c && !p->modifiedWidgets.contains(w)) {
			w->deleteLater();
		}
	}
	p->listWidget->clear();
	SettingsItemList list = p->items.values(p->actionMap.key(a));
	foreach (SettingsItem *item,list) {
		QListWidgetItem *list_item = new QListWidgetItem(item->icon(),item->text(),p->listWidget);
		list_item->setData(Qt::UserRole,reinterpret_cast<qptrdiff>(item));
	}
	if (p->listWidget->count() > 1)
		p->listWidget->show();
	else
		p->listWidget->hide();
	int currentRow = 0; //TODO save current row
	p->listWidget->setCurrentRow(currentRow);
	onCurrentItemChanged(p->listWidget->currentItem()); //if current row = 0	
}

void XSettingsWindow::onCurrentItemChanged(QListWidgetItem *item)
{
	//FIXME don't use reinterpret_cast
	if (!item)
		return;
	qptrdiff ptr = item->data(Qt::UserRole).value<qptrdiff>();
	if (!ptr)
		return;
	SettingsItem *settingsItem = reinterpret_cast<SettingsItem*>(ptr);

	SettingsWidget *w = settingsItem->widget();	
	if (p->stackedWidget->indexOf(w) == -1) {
		p->stackedWidget->addWidget(w);
		w->setController(p->controller);
		w->load();
		connect(w,SIGNAL(modifiedChanged(bool)),SLOT(onModifiedChanged(bool)));
	}
	p->stackedWidget->setCurrentWidget(w);
	setWindowTitle(tr("qutIM settings - %1").arg(settingsItem->text()));
}

void XSettingsWindow::onModifiedChanged(bool haveChanges)
{
	SettingsWidget *w = qobject_cast<SettingsWidget*>(sender());
	Q_ASSERT(w);
	if (haveChanges)
		p->modifiedWidgets.append(w);
	p->buttonBox->show();
}

void XSettingsWindow::closeEvent(QCloseEvent* ev)
{
	if (p->modifiedWidgets.count()) {
		int ret = QMessageBox::question(this,
										tr("Apply Settings - System Settings"),
										tr("The settings of the current module have changed. \n Do you want to apply the changes or discard them?"),
										QMessageBox::Apply,
										QMessageBox::Discard,
										QMessageBox::Cancel);
		switch (ret) {
			case QMessageBox::Apply:
				save();
				break;
			case QMessageBox::Discard:
				break;
			case QMessageBox::Cancel:
				cancel();
				ev->ignore();
				break;
			default:
				break;
		}
	}
}

void XSettingsWindow::save()
{
	QWidget *c = p->stackedWidget->currentWidget();
	while (p->modifiedWidgets.count()) {
		SettingsWidget *widget = p->modifiedWidgets.takeFirst();
		widget->save();
		if (widget != c)
			widget->deleteLater();
	}
	p->buttonBox->close();
}

void XSettingsWindow::cancel()
{
	QWidget *c = p->stackedWidget->currentWidget();	
	while (p->modifiedWidgets.count()) {
		SettingsWidget *widget = p->modifiedWidgets.takeFirst();
		widget->save();
		if (widget != c)
			widget->deleteLater();
	}	
	p->buttonBox->close();
}


}
