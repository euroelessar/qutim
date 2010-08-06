/****************************************************************************
 *  xsettingsdialog.cpp
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

#include "xsettingsdialog.h"
#include "ui_xsettingsdialog.h"
#include <QLayout>
#include <QDebug>
#include <libqutim/icon.h>
#include "xsettingsgroup.h"
#include <libqutim/configbase.h>
#include <libqutim/settingswidget.h>
#include <QMessageBox>
#include <QCloseEvent>
#include <QApplication>
#include <QDesktopWidget>

struct ActionEntry
{
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
	return map;
}

ActionEntryMap *entries()
{
	static ActionEntryMap map (init_entry_map());
	return &map;
}

XSettingsDialog::XSettingsDialog(const SettingsItemList& settings, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::XSettingsDialog),
	m_group(new QActionGroup(this))
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	QSize desktop_size = qApp->desktop()->size();
	resize(desktop_size.width()/2,desktop_size.height()*2/3);
	centerizeWidget(this);
	setWindowIcon(Icon("configure"));
	//init toolbar
	ui->xtoolBar->setIconSize(QSize(32,32));
	ui->xtoolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	connect(ui->xtoolBar,SIGNAL(actionTriggered(QAction*)),SLOT(onActionTriggered(QAction*)));	

#ifdef Q_WS_WIN
	ui->xtoolBar->setStyleSheet("QToolBar{background:none;border:none}"); //HACK
#endif

	m_group->setExclusive(true);

	//init button box
	ui->buttonsWidget->setVisible(false);
	connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onSaveButtonTriggered()));
	connect(ui->buttonBox, SIGNAL(rejected()), SLOT(onCancelButtonTriggered()));
	
	//init categories

	update(settings);
}

XSettingsDialog::~XSettingsDialog()
{
	delete ui;
}

void XSettingsDialog::update(const SettingsItemList &settings)
{
	m_settings_items.clear();	
	qDeleteAllLater(m_group->actions());

	foreach (SettingsItem *item, settings) {
		m_settings_items[item->type()].append(item);
	}

	QMap<Settings::Type,SettingsItemList>::const_iterator it;
	for (it = m_settings_items.constBegin();it != m_settings_items.constEnd();it++) {
		QAction *action = create(it.key());
		m_group->addAction(action);
		ui->xtoolBar->addAction(action);

		//small spike
		if (it.key() == Settings::General)
			ui->xtoolBar->addSeparator();
	}

	ui->xtoolBar->setVisible(m_group->actions().count() > 1);
	if (m_group->actions().count())
		m_group->actions().first()->trigger();
}

QAction *XSettingsDialog::create(Settings::Type type)
{
	ActionEntry entry = entries()->value(type);
	QAction *action = new QAction(entry.icon,entry.text.toString(),this);
	action->setProperty("category",type);
	action->setCheckable(true);
	return action;
}

void XSettingsDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void XSettingsDialog::onActionTriggered(QAction* action)
{
	// Remove the old page.
	QWidget *currentWidget = ui->settingsStackedWidget->currentWidget();
	XSettingsGroup *group = qobject_cast<XSettingsGroup *>(currentWidget);
	SettingsWidget *page = group ? group->currentWidget() : qobject_cast<SettingsWidget *>(currentWidget);
	if (page && !page->isModified())
		page->deleteLater();
	// Set the new page
	Settings::Type type = static_cast<Settings::Type>(action->property("category").toInt());
	SettingsItemList setting_items = m_settings_items.value(type);
	if (setting_items.count() > 1) { // ==0 or >=0 need for testing, for normally usage use >1
		// TODO: need a way to add custom groups
		XSettingsGroup *group = m_group_widgets.value(type);
		if (!group) {
			group = new XSettingsGroup(setting_items, this);
			ui->settingsStackedWidget->addWidget(group);
			connect(group, SIGNAL(modifiedChanged(SettingsWidget*)), SLOT(onWidgetModifiedChanged(SettingsWidget*)));
			connect(group, SIGNAL(titleChanged(QString,QString)), SLOT(onTitleChanged(QString)));
			m_group_widgets.insert(type,group);
		} else {
			group->updateCurrentWidget();
		}
		ui->settingsStackedWidget->setCurrentWidget(group);
	} else {
		if (setting_items.count() == 0) {
			ui->settingsStackedWidget->setCurrentIndex(0);
			return;
		}
		SettingsWidget *widget = setting_items.at(0)->widget();
		if (widget == 0) {
			return;
		} else if (ui->settingsStackedWidget->indexOf(widget) == -1) {
			widget->setParent(this);
			widget->load();
			widget->layout()->setMargin(9);
			connect(widget, SIGNAL(modifiedChanged(bool)), SLOT(onWidgetModifiedChanged(bool)));
			ui->settingsStackedWidget->addWidget(widget);
		}
		ui->settingsStackedWidget->setCurrentWidget(widget);
		onTitleChanged(setting_items.at(0)->text());
	}
}


void XSettingsDialog::onWidgetModifiedChanged(bool haveChanges)
{
	SettingsWidget *widget = qobject_cast< SettingsWidget* >(sender());
	if (!widget)
		return;
	if (haveChanges)
		onWidgetModifiedChanged(widget);
}


void XSettingsDialog::onWidgetModifiedChanged(SettingsWidget* widget)
{
	if (!widget)
		return;
	if (!m_modified_widgets.contains(widget))
		m_modified_widgets.append(widget);
	ui->buttonsWidget->setVisible(true);
}


void XSettingsDialog::onTitleChanged(const QString& title)
{
	setWindowTitle(tr("qutIM settings - %1").arg(title));
}

void XSettingsDialog::onSaveButtonTriggered()
{
	XSettingsGroup *currentGroup = qobject_cast<XSettingsGroup*>(ui->settingsStackedWidget->currentWidget());
	SettingsWidget *currentWidget = currentGroup ? currentGroup->currentWidget() : 0;
	while (m_modified_widgets.count()) {
		SettingsWidget *widget = m_modified_widgets.takeFirst();
		qDebug() << "Saved config for:" << widget->objectName();
		widget->save();
		if (currentWidget != widget)
			widget->deleteLater();
	}
	ui->buttonsWidget->setVisible(false);
}

void XSettingsDialog::onCancelButtonTriggered()
{
	XSettingsGroup *currentGroup = qobject_cast<XSettingsGroup*>(ui->settingsStackedWidget->currentWidget());
	SettingsWidget *currentWidget = currentGroup ? currentGroup->currentWidget() : 0;
	while (m_modified_widgets.count()) {
		SettingsWidget *widget = m_modified_widgets.takeFirst();
		qDebug() << "Canceled:" << widget->objectName();
		if (currentWidget != widget)
			widget->deleteLater();
		else
			widget->cancel();
	}
	ui->buttonsWidget->setVisible(false);
}


void XSettingsDialog::closeEvent(QCloseEvent* e)
{
	if (m_modified_widgets.count()) {
		int ret = QMessageBox::question(this,
										tr("Apply Settings - System Settings"),
										tr("The settings of the current module have changed. \n Do you want to apply the changes or discard them?"),
										QMessageBox::Apply,
										QMessageBox::Discard,
										QMessageBox::Cancel);
		switch (ret) {
			case QMessageBox::Apply:
				onSaveButtonTriggered();
				break;
			case QMessageBox::Discard:
				onCancelButtonTriggered();
				break;
			case QMessageBox::Cancel:
				e->ignore();
				break;
			default:
				break;
		}
	}
}
