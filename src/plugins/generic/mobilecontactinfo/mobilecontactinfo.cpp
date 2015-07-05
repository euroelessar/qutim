/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "mobilecontactinfo.h"
#include <qutim/icon.h>
#include <qutim/contact.h>
#include <qutim/account.h>
#include <qutim/dataforms.h>
#include <QVBoxLayout>
#include <QApplication>
#include <QResizeEvent>
#include <QScrollBar>
#include <QApplication>
#include <qutim/systemintegration.h>
#include <qutim/debug.h>

namespace Core
{

MobileContactInfoWindow::MobileContactInfoWindow(QWidget *parent) :
	QScrollArea(parent),
	request(0),
	actionBox(new ActionBox(this))
{
	scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);
	scrollArea->setFrameShape(QFrame::NoFrame);
	scrollArea->setAlignment(Qt::AlignTop);
	QWidget *scrollWidget = new QWidget(scrollArea);
	layout = new QVBoxLayout(scrollWidget);
	scrollArea->setWidget(scrollWidget);
	QVBoxLayout *l = new QVBoxLayout(this);
	l->addWidget(scrollArea);
	l->addWidget(actionBox, 0, Qt::AlignCenter);
	l->setMargin(0);
	resize(400, 500);

	QAction *action = new QAction(tr("Request details"), actionBox);
	connect(action, SIGNAL(triggered()), SLOT(onRequestButton()));
	actionBox->addAction(action);
	saveAction = new QAction(tr("Save"), actionBox);
	connect(saveAction, SIGNAL(triggered()), SLOT(onSaveButton()));
	actionBox->addAction(saveAction);

	//Symbian close button
	action = new QAction(tr("Close"),this);
	connect(action, SIGNAL(triggered()), SLOT(close()));
	addAction(action);
}

void MobileContactInfoWindow::setObject(QObject *obj, SupportLevel type)
{
	object = obj;
	readWrite = type == InfoRequestFactory::ReadWrite;
	request = InfoRequestFactory::dataFormRequest(obj);

	if (request) {
		connect(request, SIGNAL(stateChanged(qutim_sdk_0_3::InfoRequest::State)),
				SLOT(onRequestStateChanged(qutim_sdk_0_3::InfoRequest::State)));
		request->requestData();
	}

	QString title;
	QString avatar;
	if (Buddy *buddy = qobject_cast<Buddy*>(object)) {
		title = QApplication::translate("ContactInfo", "About contact %1 <%2>")
					.arg(buddy->name())
					.arg(buddy->id());
	} else {
		if (Account *account = qobject_cast<Account*>(object)) {
			title = QApplication::translate("ContactInfo", "About account %1")
						.arg(account->name());
		} else {
			title = QApplication::translate("ContactInfo", "About %1 <%2>")
						.arg(object->property("name").toString())
						.arg(object->property("id").toString());
		}
	}
	setWindowTitle(title);
	saveAction->setVisible(readWrite);
	if (request)
		onRequestStateChanged(request->state());
}

DataItem MobileContactInfoWindow::filterItems(const DataItem &item, bool readOnly)
{
	DataItem result = item;
	result.setSubitems(QList<DataItem>());
	foreach (const DataItem &subitem, item.subitems()) {
		if (subitem.isAllowedModifySubitems()) {
			if (!readOnly || subitem.hasSubitems())
				result.addSubitem(subitem);
		} else if (subitem.hasSubitems()) {
			filterItemsHelper(subitem, result, readOnly);
		} else {
			if (readOnly && isItemEmpty(subitem))
				continue;
			result.addSubitem(subitem);
		}
	}
	return result;
}

void MobileContactInfoWindow::filterItemsHelper(const DataItem &item, DataItem &result, bool readOnly)
{
	Q_ASSERT(item.hasSubitems());
	DataItem group = item;
	group.setSubitems(QList<DataItem>());
	foreach (const DataItem &subitem, item.subitems()) {
		if (subitem.isAllowedModifySubitems()) {
			if (!readOnly || subitem.hasSubitems())
				result.addSubitem(subitem);
		} else if (subitem.hasSubitems()) {
			filterItemsHelper(subitem, result, readOnly);
		} else {
			if (readOnly && isItemEmpty(subitem))
				continue;
			group.addSubitem(subitem);
		}
	}
	if (group.hasSubitems())
		result.addSubitem(group);
}

bool MobileContactInfoWindow::isItemEmpty(const DataItem &item)
{
	if (item.data().isNull() || item.property("notSet", false))
		return true;
	QVariant::Type type = item.data().type();
	if (type == QVariant::Icon)
		return item.data().value<QIcon>().isNull();
	else if (type == QVariant::Pixmap)
		return item.data().value<QPixmap>().isNull();
	else if (type == QVariant::Image)
		return item.data().value<QImage>().isNull();
	else if (type == QVariant::StringList)
		return item.data().value<QStringList>().isEmpty();
	else if (item.data().canConvert<LocalizedString>())
		return item.data().value<LocalizedString>().toString().isEmpty();
	else if (item.data().canConvert<LocalizedStringList>())
		return item.data().value<LocalizedStringList>().isEmpty();
	else
		return item.data().toString().isEmpty();
}

void MobileContactInfoWindow::onRequestStateChanged(InfoRequest::State state)
{
	if (state == InfoRequest::RequestDone)
	{
		DataItem item = request->dataItem();
		if (!readWrite) {
			item = filterItems(item, true);
			item.setProperty("readOnly", true);
		} else {
			item = filterItems(item);
		}
		dataWidget.reset(AbstractDataForm::get(item));
		if (dataWidget)
			layout->addWidget(dataWidget.data());
	}
}

void MobileContactInfoWindow::onRequestButton()
{
	request->cancel();
	request->requestData();
}

void MobileContactInfoWindow::onSaveButton()
{
	if (dataWidget) {
		request->cancel();
		request->updateData(dataWidget->item());
	}
}

MobileContactInfo::MobileContactInfo()
{
}

void MobileContactInfo::show(QObject *object)
{
	InfoRequestFactory *factory = InfoRequestFactory::factory(object);
	if (!factory)
		return;

	SupportLevel type = factory->supportLevel(object);
	if (type <= InfoRequestFactory::Unavailable)
		return;

	if (!info) {
		info = new MobileContactInfoWindow(qApp->activeWindow());
#ifdef Q_WS_MAEMO_5
		info.data()->setParent(QApplication::activeWindow());
		info.data()->setAttribute(Qt::WA_Maemo5StackedWindow);
#endif
		info.data()->setWindowFlags(info.data()->windowFlags() | Qt::Window);
		centerizeWidget(info.data());
		SystemIntegration::show(info.data());
		info.data()->setAttribute(Qt::WA_DeleteOnClose, true);
	} else {
		info.data()->raise();
	}
	info.data()->setObject(object, type);
}

}

