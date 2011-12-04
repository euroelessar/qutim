/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Sidorov Aleksey <sauron@citadelspb.com>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

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
	QWidget *parent;
};

XSettingsWindow::XSettingsWindow(const qutim_sdk_0_3::SettingsItemList& settings, QObject* controller, QWidget *parent) :
	QMainWindow(parent),
	p(new XSettingsWindowPrivate)
{
	setAttribute(Qt::WA_DeleteOnClose);
	p->controller = controller;
    setWindowModality(controller ? Qt::WindowModal : Qt::NonModal);
	//setup ui
	QWidget *w = new QWidget(this);
	QVBoxLayout *l = new QVBoxLayout(w);
	Config cfg;
	cfg.beginGroup("xsettings/window");
	QByteArray data;

	p->parent = qobject_cast<XSettingsWindow*>(qApp->activeWindow());
	if(p->parent) {
		QRect geom = p->parent->geometry();
		int width = geom.width()/15;
		int height = geom.height()/15;
		geom.adjust(width,height,-width,-height);
		setGeometry(geom);
	} else {
		data = cfg.value("geometry", QByteArray());
		if (data.isEmpty() || !restoreGeometry(data)) {
			QSize desktopSize = QApplication::desktop()->availableGeometry(QCursor::pos()).size();
			resize(desktopSize.width() / 2, desktopSize.height() * 2 / 3);
			centerizeWidget(this);
		}
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

    QDialogButtonBox::StandardButtons buttons;
    if (controller)
        buttons = QDialogButtonBox::Ok;
    else
        buttons = QDialogButtonBox::Save | QDialogButtonBox::Cancel;

    p->buttonBox = new QDialogButtonBox(buttons, Qt::Horizontal, w);
	l->addWidget(p->buttonBox);
    p->buttonBox->setVisible(controller);
	//init actiontoolbar
	setCentralWidget(w);
    setUnifiedTitleAndToolBarOnMac(true);

	p->toolBar = new ActionToolBar(w);
	addToolBar(Qt::TopToolBarArea,p->toolBar);

	int width = style()->pixelMetric(QStyle::PM_IconViewIconSize);
	QSize size = QSize(width, width);

	p->toolBar->setIconSize(size);
	p->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	p->toolBar->setObjectName(QLatin1String("SettingsBar"));
	p->toolBar->setMovable(false);

#if defined (Q_WS_WIN32) || defined(Q_WS_MAC)
	width = 22;
#else
	width = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
#endif
	size = QSize(width, width);
	p->listWidget->setIconSize(size);

	p->group = new QActionGroup(w);
	p->group->setExclusive(true);
	//connections
    connect(p->group,SIGNAL(triggered(QAction*)), SLOT(onGroupActionTriggered(QAction*)));
	connect(p->listWidget,
			SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
			SLOT(onCurrentItemChanged(QListWidgetItem*))
			);
    connect(p->buttonBox,SIGNAL(accepted()), SLOT(save()));
    connect(p->buttonBox,SIGNAL(rejected()), SLOT(cancel()));
	loadSettings(settings);
	if (p->group->actions().count())
		p->group->actions().first()->trigger();
}


void XSettingsWindow::update(const qutim_sdk_0_3::SettingsItemList& settings)
{
	foreach (SettingsItem *item, (p->items.values().toSet() -= settings.toSet()))
		item->clearWidget();
	p->items.clear();
	loadSettings(settings);
	onCurrentItemChanged(p->listWidget->currentItem());
}

XSettingsWindow::~XSettingsWindow()
{
	if(p->parent)
		return;

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
			if (type == Settings::General) {
				QAction *sep = p->toolBar->addSeparator();
				p->actionMap.insertMulti(Settings::General, sep);
			}
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
		QIcon icon = item->icon();
		if (!icon.actualSize(QSize(1,1)).isValid())
			icon = Icon("applications-system");
		QListWidgetItem *listItem = new QListWidgetItem(icon, item->text(), p->listWidget);
		listItem->setData(Qt::UserRole,reinterpret_cast<qptrdiff>(item));
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
    if (p->controller)
        close();
    else
        p->buttonBox->close();
}

void XSettingsWindow::cancel()
{
	QWidget *c = p->stackedWidget->currentWidget();
	while (p->modifiedWidgets.count()) {
		SettingsWidget *widget = p->modifiedWidgets.takeFirst();
		widget->cancel();
		if (widget != c)
			widget->deleteLater();
	}
    if (p->controller)
        close();
    else
        p->buttonBox->close();
}


}

