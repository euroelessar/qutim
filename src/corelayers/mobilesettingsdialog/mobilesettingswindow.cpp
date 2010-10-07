/****************************************************************************
 *  mobilesettingswindow.cpp
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

#include "mobilesettingswindow.h"
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
#include <slidingstackedwidget.h>
#include <qutim/actionbox.h>
#include <qutim/debug.h>

namespace Core
{

struct ActionEntry
{
	//TODO move to libqutim
	ActionEntry(const LocalizedString &t,const QIcon &i)
	{
		text = t;
		icon = i;
	}
	ActionEntry() {}
	LocalizedString text;
	QIcon icon;
};

typedef QMap<Settings::Type,ActionEntry> ActionEntryMap;

static ActionEntryMap init_entry_map()
{
	ActionEntryMap map;
	map.insert(Settings::General,ActionEntry(QT_TRANSLATE_NOOP("Settings","General"),Icon("preferences-system")));
	map.insert(Settings::Protocol,ActionEntry(QT_TRANSLATE_NOOP("Settings","Protocols"),Icon("applications-internet")));
	map.insert(Settings::Appearance,ActionEntry(QT_TRANSLATE_NOOP("Settings","Appearance"),Icon("applications-graphics")));
	map.insert(Settings::Plugin,ActionEntry(QT_TRANSLATE_NOOP("Settings","Plugins"),Icon("applications-other")));
	map.insert(Settings::Special,ActionEntry(QT_TRANSLATE_NOOP("Settings","Special"),QIcon()));
	map.insert(Settings::Invalid,ActionEntry(QT_TRANSLATE_NOOP("Settings","Invalid"),QIcon()));
	return map;
}

Q_GLOBAL_STATIC_WITH_ARGS(ActionEntryMap, entries, (init_entry_map()))

struct MobileSettingsWindowPrivate
{
	SlidingStackedWidget *stackedWidget;
	QListWidget *settingsListWidget;
	QListWidget *categoryListWidget;
	//ActionBox *actionBox;
	QObject *controller;
	ActionBox *actionBox;
	QAction *positiveAct;
	QAction *backAct;
	QMap<Settings::Type,SettingsItem*> items;
	QList<SettingsWidget*> modifiedWidgets;
	QMap<Settings::Type,QListWidgetItem*> categoryMap;
};
	
MobileSettingsWindow::MobileSettingsWindow(const qutim_sdk_0_3::SettingsItemList& settings, QObject* controller) :
	p(new MobileSettingsWindowPrivate)
{
	setAttribute(Qt::WA_DeleteOnClose);
	p->controller = controller;
	//setup ui
	QWidget *w = new QWidget(this);
	QVBoxLayout *l = new QVBoxLayout(w);
	l->setMargin(0);
	l->setSpacing(0);
	p->stackedWidget = new SlidingStackedWidget(w);
	//init widgets
	p->settingsListWidget = new QListWidget(p->categoryListWidget);
	p->categoryListWidget = new QListWidget(w);

	p->stackedWidget->addWidget(p->categoryListWidget);
	p->stackedWidget->addWidget(p->settingsListWidget);

	p->actionBox = new ActionBox(this);
	p->backAct = new QAction(tr("Back"),this);
	p->backAct->setSoftKeyRole(QAction::PositiveSoftKey);
	p->backAct->setVisible(false);
	p->actionBox->addAction(p->backAct);

	l->addWidget(p->stackedWidget);
	l->addWidget(p->actionBox);

	setCentralWidget(w);
	//connections
	connect(p->categoryListWidget,
			SIGNAL(activated(QModelIndex)),
			SLOT(onCategoryActivated(QModelIndex))
			);
	connect(p->settingsListWidget,
			SIGNAL(activated(QModelIndex)),
			SLOT(onCurrentItemActivated(QModelIndex))
		   );
	connect(p->backAct,SIGNAL(triggered()),SLOT(slideUp()));

	loadSettings(settings);

	p->stackedWidget->setCurrentWidget(p->categoryListWidget);
}

	
void MobileSettingsWindow::update(const qutim_sdk_0_3::SettingsItemList& settings)
{
	foreach (SettingsItem *item, (p->items.values().toSet() -= settings.toSet()))
		item->clearWidget();
	p->items.clear();
	loadSettings(settings);
}

MobileSettingsWindow::~MobileSettingsWindow()
{
}

void MobileSettingsWindow::loadSettings(const qutim_sdk_0_3::SettingsItemList& settings)
{
	foreach (SettingsItem *item,settings) {
		p->items.insertMulti(item->type(),item);
	}
	ensureActions();
}

QListWidgetItem* MobileSettingsWindow::get(Settings::Type type)
{
	ActionEntry entry = entries()->value(type);
	QListWidgetItem *item = p->categoryMap.value(type);
	if (!item) {
		item = new QListWidgetItem(entry.icon,entry.text.toString(),p->categoryListWidget);
		p->categoryMap.insert(type,item);
	}
	return item;
}


void MobileSettingsWindow::ensureActions()
{
	qDeleteAll(p->categoryMap);
	p->categoryListWidget->clear();
	foreach (Settings::Type type,p->items.keys()) {
		get(type);
	}
}

void MobileSettingsWindow::onCategoryActivated(const QModelIndex &index)
{
	QListWidgetItem *i = p->categoryListWidget->item(index.row());
	//remove old settings widgets
	for (int index = 0;index!=p->stackedWidget->count();index++) {
		SettingsWidget *w = qobject_cast<SettingsWidget*>(p->stackedWidget->widget(index));
		if (!p->modifiedWidgets.contains(w)) {
			w->deleteLater();
		}
	}
	p->settingsListWidget->clear();
	SettingsItemList list = p->items.values(p->categoryMap.key(i));
	foreach (SettingsItem *item,list) {
		QListWidgetItem *list_item = new QListWidgetItem(item->icon(),item->text(),p->settingsListWidget);
		list_item->setData(Qt::UserRole,reinterpret_cast<qptrdiff>(item));
	}
	int currentRow = 0; //TODO save current row
	p->settingsListWidget->setCurrentRow(currentRow);
	setWindowTitle(tr("qutIM settings - %1").arg(i->text()));
	p->stackedWidget->slideInIdx(p->stackedWidget->indexOf(p->settingsListWidget));
	p->backAct->setVisible(true);
}

void MobileSettingsWindow::onCurrentItemActivated(const QModelIndex &index)
{
	QListWidgetItem *item = p->settingsListWidget->item(index.row());
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
	p->stackedWidget->slideInIdx(p->stackedWidget->indexOf(w));
	setWindowTitle(tr("qutIM settings - %1").arg(settingsItem->text()));
}

void MobileSettingsWindow::onModifiedChanged(bool haveChanges)
{
	SettingsWidget *w = qobject_cast<SettingsWidget*>(sender());
	Q_ASSERT(w);
	if (haveChanges)
		p->modifiedWidgets.append(w);
}

void MobileSettingsWindow::closeEvent(QCloseEvent* ev)
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

void MobileSettingsWindow::save()
{
	QWidget *c = p->stackedWidget->currentWidget();
	while (p->modifiedWidgets.count()) {
		SettingsWidget *widget = p->modifiedWidgets.takeFirst();
		widget->save();
		if (widget != c)
			widget->deleteLater();
	}
}

void MobileSettingsWindow::cancel()
{
	QWidget *c = p->stackedWidget->currentWidget();	
	while (p->modifiedWidgets.count()) {
		SettingsWidget *widget = p->modifiedWidgets.takeFirst();
		widget->save();
		if (widget != c)
			widget->deleteLater();
	}	
}

void MobileSettingsWindow::slideUp()
{
	//a spike
	QWidget *w = p->stackedWidget->currentWidget();

	if(w == p->categoryListWidget)
		return;
	if(w == p->settingsListWidget) {
		p->backAct->setVisible(false);
		p->stackedWidget->slideInIdx(p->stackedWidget->indexOf(p->categoryListWidget));
	}
	else {
		p->stackedWidget->slideInIdx(p->stackedWidget->indexOf(p->settingsListWidget));
	}
}

void MobileSettingsWindow::slideDown()
{
	//TODO
	p->stackedWidget->slideInNext();
}

}
