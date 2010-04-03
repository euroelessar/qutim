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
#include "xtoolbar.h"
#include <QLayout>
#include <QPropertyAnimation>
#include <QStateMachine>
#include <QDebug>
#include <libqutim/icon.h>
#include "xsettingsgroup.h"
#include <libqutim/configbase.h>
#include <libqutim/settingswidget.h>
#include <QMessageBox>
#include <QCloseEvent>

XSettingsDialog::XSettingsDialog(const SettingsItemList& settings, QWidget* parent) :
		QDialog(parent),    ui(new Ui::XSettingsDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	centerizeWidget(this);
	//load settings
	ConfigGroup general_group = Config("appearance").group("xsettings/general");
	//init toolbar
	connect(ui->xtoolBar,SIGNAL(actionTriggered(QAction*)),SLOT(onActionTriggered(QAction*)));

#ifdef Q_WS_WIN
	ui->xtoolBar->setStyleSheet("QToolBar{background:none;border:none}"); //HACK
#endif

	//init actions
	//TODO FIXME get rid of copypaste

	QActionGroup *group = new QActionGroup(this);

	QAction *general =  new QAction(Icon("preferences-system"),tr("General"),ui->xtoolBar);
	general->setToolTip(tr("General configuration"));
	addAction(general,Settings::General);
	group->addAction(general);

	ui->xtoolBar->addSeparator();

	QAction *protocols =  new QAction(Icon("applications-internet"),tr("Protocols"),ui->xtoolBar);
	protocols->setToolTip(tr("Accounts and protocols settings"));
	addAction(protocols,Settings::Protocol);
	group->addAction(protocols);

	QAction *appearance =  new QAction(Icon("applications-graphics"),tr("Appearance"),ui->xtoolBar);
	appearance->setToolTip(tr("Appearance settings"));
	addAction(appearance,Settings::Appearance);
	group->addAction(appearance);

	QAction *plugins =  new QAction(Icon("applications-other"),tr("Plugins"),ui->xtoolBar);
	plugins->setToolTip(tr("Additional plugins settings"));
	addAction(plugins,Settings::Plugin);
	m_group_widgets.resize(ui->xtoolBar->actions().count());
	group->addAction(plugins);

	group->setExclusive(true);

	//init button box
	ui->buttonsWidget->setVisible(false);
	connect(ui->buttonBox,SIGNAL(accepted()),SLOT(onSaveButtonTriggered()));
	connect(ui->buttonBox,SIGNAL(rejected()),SLOT(onCancelButtonTriggered()));
	
	//init categories

	foreach (SettingsItem *item, settings) {
		if (item->type() >= m_settings_items.size())
			m_settings_items.resize(item->type()+1);
		m_settings_items[item->type()].append(item);
	}
	general->trigger();
}

XSettingsDialog::~XSettingsDialog()
{
	delete ui;
}


void XSettingsDialog::addAction (QAction* action, Settings::Type type)
{
	action->setProperty("category",type);
	action->setCheckable(true);
	ui->xtoolBar->addAction(action);
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

void XSettingsDialog::onActionTriggered ( QAction* action )
{
	Settings::Type type = static_cast<Settings::Type>(action->property("category").toInt());
	SettingsItemList setting_items = m_settings_items.value(type);
	if (setting_items.count()>1) { // ==0 or >=0 need for testing, for normally usage use >1
		//TODO need way to add custom group
		XSettingsGroup *group = m_group_widgets.value(type);
		if (!group) {
			group = new XSettingsGroup(setting_items,this);
			ui->settingsStackedWidget->addWidget(group);
			connect(group,SIGNAL(modifiedChanged(SettingsWidget*)),SLOT(onWidgetModifiedChanged(SettingsWidget*)));
			connect(group,SIGNAL(titleChanged(QString)),SLOT(onTitleChanged(QString)));
			m_group_widgets.insert(type,group);
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
			widget->load();
			connect(widget,SIGNAL(modifiedChanged(bool)),SLOT(onWidgetModifiedChanged(bool)));
			ui->settingsStackedWidget->addWidget(widget);
		}
		ui->settingsStackedWidget->setCurrentWidget(widget);
		onTitleChanged(setting_items.at(0)->text());
	}
}


void XSettingsDialog::onWidgetModifiedChanged ( bool haveChanges )
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
	while (m_modified_widgets.count()) {
		SettingsWidget *widget = m_modified_widgets.takeFirst();
		qDebug() << "Saved config for:" << widget->objectName();
		widget->save();
	}
	ui->buttonsWidget->setVisible(false);
}

void XSettingsDialog::onCancelButtonTriggered()
{
	while (m_modified_widgets.count()) {
		SettingsWidget *widget = m_modified_widgets.takeFirst();
		qDebug() << "Canceled:" << widget->objectName();
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
