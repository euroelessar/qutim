/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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
#include <QScrollArea>
#include <slidingstackedwidget.h>
#include <qutim/actionbox.h>
#include <qutim/debug.h>
#include <qutim/servicemanager.h>

namespace Core
{

struct MobileSettingsWindowPrivate
{
	SlidingStackedWidget *stackedWidget;
	QListWidget *settingsListWidget;
	QListWidget *categoryListWidget;
	//ActionBox *actionBox;
	QObject *controller;
	ActionBox *actionBox;
	QAction *closeAct;
	QAction *backAct;
	QMap<Settings::Type,SettingsItem*> items;
	QHash<QWidget*,QWidget*> slideMap;
	QHash<SettingsWidget*,QScrollArea*> scrollAreas;
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
	p->settingsListWidget = new QListWidget(w);
	p->categoryListWidget = new QListWidget(w);

	p->stackedWidget->addWidget(p->categoryListWidget);
	p->stackedWidget->addWidget(p->settingsListWidget);

	p->categoryListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	p->settingsListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

	p->actionBox = new ActionBox(this);
	p->backAct = new QAction(tr("Back"),this);
	p->backAct->setVisible(false);
	p->actionBox->addAction(p->backAct);

	p->closeAct = new QAction(tr("Close"),this);
#ifndef Q_OS_SYMBIAN
	p->closeAct->setVisible(false);
#endif
	p->actionBox->addAction(p->closeAct);

	l->addWidget(p->stackedWidget);
	l->addWidget(p->actionBox);

	int width = style()->pixelMetric(QStyle::PM_LargeIconSize);
	QSize size = QSize(width, width);

	p->categoryListWidget->setIconSize(size);
	p->settingsListWidget->setIconSize(size);

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
	connect(p->closeAct,SIGNAL(triggered()),SLOT(close()));
	connect(p->stackedWidget,
			SIGNAL(fingerGesture(enum SlidingStackedWidget::SlideDirection)),
			this,SLOT(fingerGesture(enum SlidingStackedWidget::SlideDirection)));

	loadSettings(settings);
	QTimer::singleShot(0, this, SLOT(initScrolling()));
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
	QListWidgetItem *item = p->categoryMap.value(type);
	if (!item) {
		item = new QListWidgetItem(p->categoryListWidget);
		item->setText(Settings::getTypeTitle(type));
		item->setIcon(Settings::getTypeIcon(type));
		p->categoryMap.insert(type,item);
	}
	return item;
}


void MobileSettingsWindow::ensureActions()
{
	p->slideMap.clear();
	p->categoryMap.clear();
	qDeleteAll(p->categoryMap);
	p->categoryListWidget->clear();
	foreach (Settings::Type type,p->items.keys()) {
		get(type);
	}

	if(p->categoryMap.count() > 1) {
		p->slideMap.insert(p->settingsListWidget,p->categoryListWidget);
		p->stackedWidget->setCurrentWidget(p->categoryListWidget);
	}
	else if(p->categoryListWidget->count()){
		onCategoryActivated(p->categoryListWidget->item(0));
		p->stackedWidget->setCurrentWidget(p->settingsListWidget);
	}
}

void MobileSettingsWindow::onCategoryActivated(const QModelIndex &index)
{
	QListWidgetItem *i = p->categoryListWidget->item(index.row());
	onCategoryActivated(i);
}
void MobileSettingsWindow::onCategoryActivated(QListWidgetItem *i)
{
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
		QIcon icon = item->icon();
		if (!icon.actualSize(QSize(1,1)).isValid())
			icon = Icon("applications-system");
		QListWidgetItem *listItem = new QListWidgetItem(icon, item->text(), p->settingsListWidget);
		listItem->setData(Qt::UserRole,reinterpret_cast<qptrdiff>(item));
	}
	int currentRow = 0; //TODO save current row
	p->settingsListWidget->setCurrentRow(currentRow);
	setWindowTitle(tr("qutIM settings - %1").arg(i->text()));
	slideDown(p->settingsListWidget);
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
	if(!p->scrollAreas.contains(w)) {
		qDebug() << "create widget";
		QScrollArea *area = new QScrollArea(this);
		area->setFrameShape(QScrollArea::NoFrame);
		area->setWidget(w);
		area->setWidgetResizable(true);
		if(QObject *scroller = ServiceManager::getByName("Scroller"))
			QMetaObject::invokeMethod(scroller, "enableScrolling", Q_ARG(QObject*, area->viewport()));
		p->stackedWidget->addWidget(area);
		w->setController(p->controller);
		w->load();

#ifdef QUTIM_MOBILE_UI
		//simple hack for form layouts
		foreach(QFormLayout *l, w->findChildren<QFormLayout*>()) {
			l->setRowWrapPolicy(QFormLayout::WrapAllRows);
		}
		foreach (QLabel *label, w->findChildren<QLabel*>())
			label->setWordWrap(true);
#endif
		p->slideMap.insert(area,p->settingsListWidget);
		p->scrollAreas.insert(w,area);		
		connect(w,SIGNAL(modifiedChanged(bool)),SLOT(onModifiedChanged(bool)));
		connect(w,SIGNAL(destroyed(QObject*)),SLOT(onWidgetDestroyed(QObject*)));
	}
	slideDown(p->scrollAreas.value(w));
	setWindowTitle(tr("qutIM settings - %1").arg(settingsItem->text()));
}

void MobileSettingsWindow::onModifiedChanged(bool haveChanges)
{
	SettingsWidget *w = qobject_cast<SettingsWidget*>(sender());
	Q_ASSERT(w);
	if (haveChanges)
		p->modifiedWidgets.append(w);
}

void MobileSettingsWindow::onWidgetDestroyed(QObject *obj)
{
	SettingsWidget *w = reinterpret_cast<SettingsWidget*>(obj);
	p->slideMap.remove(p->scrollAreas.take(w));
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
		widget->cancel();
		if (widget != c)
			widget->deleteLater();
	}	
}

void MobileSettingsWindow::slideUp()
{
	QWidget *w = p->slideMap.value(p->stackedWidget->currentWidget());
	if(w) {
		p->stackedWidget->slideInIdx(p->stackedWidget->indexOf(w));
		p->backAct->setVisible(p->slideMap.value(w)); //nice hack :)
	}
}

void MobileSettingsWindow::slideDown(QWidget *w)
{
	p->stackedWidget->slideInIdx(p->stackedWidget->indexOf(w));
	p->backAct->setVisible(p->slideMap.value(w));
}

void MobileSettingsWindow::fingerGesture( enum SlidingStackedWidget::SlideDirection direction)
{
	if (direction==SlidingStackedWidget::LeftToRight) {
		slideUp();
    }

}

void MobileSettingsWindow::initScrolling()
{
	if(QObject *scroller = ServiceManager::getByName("Scroller")) {
		QMetaObject::invokeMethod(scroller, "enableScrolling", Q_ARG(QObject*, p->categoryListWidget->viewport()));
		QMetaObject::invokeMethod(scroller, "enableScrolling", Q_ARG(QObject*, p->settingsListWidget->viewport()));
	}
}

}

