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

#include "contactinfo.h"
#include <qutim/icon.h>
#include <qutim/contact.h>
#include <qutim/account.h>
#include <qutim/dataforms.h>
#include <QDate>
#include <QDateEdit>
#include <QFileDialog>
#include <QLabel>
#include <QScrollArea>
#include <QTextDocument>
#include <QUrl>
#include <qutim/debug.h>
#include <qutim/systemintegration.h>

namespace Core
{
MainWindow::MainWindow() :
	request(0)
{
	ui.setupUi(this);
	connect(ui.requestButton, SIGNAL(clicked()), SLOT(onRequestButton()));
	connect(ui.saveButton, SIGNAL(clicked()), SLOT(onSaveButton()));
}

void MainWindow::setObject(QObject *obj, SupportLevel type)
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
	ui.saveButton->setVisible(readWrite);

	if (request)
		onRequestStateChanged(request->state());
}

void MainWindow::onRequestStateChanged(qutim_sdk_0_3::InfoRequest::State state)
{
	if (state != InfoRequest::RequestDone &&
		state != InfoRequest::LoadedFromCache &&
		state != InfoRequest::Initialized)
	{
		return;
	}

	ui.infoListWidget->clear();
	int curPage = ui.detailsStackedWidget->currentIndex();
	QWidget *w;
	while ((w = ui.detailsStackedWidget->widget(0)) != 0)
		delete w;

	addItems(request->dataItem());
	if (curPage >= 0)
		ui.infoListWidget->setCurrentRow(curPage);
}

void MainWindow::onRequestButton()
{
	request->cancel();
	request->requestData();
}

void MainWindow::onSaveButton()
{
	DataItem items;
	if (avatarWidget) {
		DataItem avatarItem = avatarWidget->item();
		items.addSubitem(avatarItem);
	}
	for (int i = 0; i < ui.detailsStackedWidget->count(); ++i) {
		QWidget *widget = ui.detailsStackedWidget->widget(i);
		Q_ASSERT(qobject_cast<QScrollArea*>(widget));
		QScrollArea *scrollArea = static_cast<QScrollArea*>(widget);
		AbstractDataForm *dataFrom = qobject_cast<AbstractDataForm*>(scrollArea->widget());
		if (!dataFrom)
			continue;
		if (dataFrom->objectName() == "General") {
			foreach (const DataItem &item, dataFrom->item().subitems())
				items.addSubitem(item);
		} else {
			items.addSubitem(dataFrom->item());
		}
	}
	request->cancel();
	request->updateData(items);
}

void MainWindow::addItems(DataItem items)
{
	if (items.isNull() || !items.hasSubitems())
		return;
	// Avatar
	DataItem avatarItem = items.takeSubitem(QLatin1String("avatar"), true);
	avatarItem.setReadOnly(!readWrite);
	avatarWidget.reset(AbstractDataForm::get(avatarItem));
	if (avatarWidget) {
		QString avatarPath = avatarItem.property("imagePath", QString());

		if (!avatarPath.isEmpty()) {
			avatarWidget->setToolTip(QString::fromLatin1("<img src=\"%1\"/>")
									 .arg(Qt::escape(avatarPath)));
		}

		avatarWidget->setParent(this);
		avatarWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		ui.gridLayout->addWidget(avatarWidget.data(), 0, 0, Qt::AlignCenter);
	}
	// Summary
	QScrollArea *scrollArea = new QScrollArea(ui.detailsStackedWidget);
	QLabel *w = new QLabel(summary(items), scrollArea);
	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(w);
	w->setWordWrap(true);
	w->setAlignment(Qt::AlignTop);
	w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	w->setTextInteractionFlags(Qt::LinksAccessibleByMouse |
							   Qt::LinksAccessibleByKeyboard |
							   Qt::TextSelectableByMouse |
							   Qt::TextSelectableByKeyboard);
	w->setOpenExternalLinks(true);
	ui.infoListWidget->addItem(QT_TRANSLATE_NOOP("ContactInfo", "Summary"));
	ui.detailsStackedWidget->addWidget(scrollArea);
	// Pages
	DataItem general;
	foreach (const DataItem &item, items.subitems()) {
		if (item.hasSubitems() || item.isAllowedModifySubitems()) {
			QWidget *page = getPage(item);
			ui.infoListWidget->addItem(item.title());
			ui.detailsStackedWidget->addWidget(page);
		} else {
			general.addSubitem(item);
		}
	}
	if (!general.isNull()) {
		QWidget *page = getPage(general);
		ui.infoListWidget->insertItem(0, QT_TRANSLATE_NOOP("ContactInfo", "General"));
		ui.detailsStackedWidget->insertWidget(0, page);
	}
	ui.infoListWidget->setVisible(ui.infoListWidget->count() > 1);
}

QWidget *MainWindow::getPage(DataItem item)
{
	if (!readWrite)
		item.setReadOnly(true);
	QWidget *data = AbstractDataForm::get(item);
	QScrollArea *scrollArea = 0;
	if (data) {
		scrollArea = new QScrollArea(this);
		data->setParent(scrollArea);
		scrollArea->setWidgetResizable(true);
		scrollArea->setWidget(data);
	}
	return scrollArea;
}

QString MainWindow::summary(const DataItem &items, bool *titlePrinted)
{
	QString text;
	if (titlePrinted) *titlePrinted = false;
	bool needTitle = false;
	foreach (const DataItem &item, items.subitems()) {
		if (item.property("additional", false) || item.property("notSet", false))
			continue;
		if (item.hasSubitems()) {
			bool title = 0;
			QString s = summary(item, &title);
			if (!title)
				needTitle = true;
			text += s;
		} else if (item.data().canConvert(QVariant::String)) {
			QString str = item.data().toString();
			if (str.isEmpty())
				continue;
			needTitle = true;
			text += QString("<b>%1:</b>  ").arg(item.title());
			QVariant::Type type = item.data().type();
			if (type == QVariant::Date) {
				text += item.data().toDate().toString(Qt::SystemLocaleLongDate);
			} else if (type == QVariant::DateTime) {
				text += item.data().toDateTime().toString(Qt::SystemLocaleLongDate);
			} else if (type == QVariant::Bool) {
				text += item.data().toBool() ?
							QT_TRANSLATE_NOOP("ContactInfo", "yes") :
							QT_TRANSLATE_NOOP("ContactInfo", "no");
			} else if (type == QVariant::Url) {
				QUrl url = item.data().toUrl();
				QByteArray urlEncoded = url.toEncoded();
				text += QString::fromLatin1("<a href='%1' title='%2' target='_blank'>%3</a>")
						.arg(QString::fromLatin1(urlEncoded, urlEncoded.size()),
							 url.toString(),
							 Qt::escape(url.toString()));
			} else {
				text += str.replace(QRegExp("(\r\n|\n|\r)"), "<br>");
			}
			text += "<br>";
		}
	}
	if (needTitle) {
		QString title = items.title();
		if (!title.isEmpty()) {
			text.prepend(QString("<b>[%1]:</b><br>").arg(title));
			if (titlePrinted) *titlePrinted = true;
		}
	}
	return text;
}

ContactInfo::ContactInfo()
{
}

void ContactInfo::show(QObject *object)
{
	InfoRequestFactory *factory = InfoRequestFactory::factory(object);
	if (!factory)
		return;

	SupportLevel type = factory->supportLevel(object);
	if (type <= InfoRequestFactory::Unavailable)
		return;

	if (!info) {
		info = new MainWindow();
		centerizeWidget(info.data());
		SystemIntegration::show(info.data());
		info.data()->setAttribute(Qt::WA_DeleteOnClose, true);
	} else {
		info.data()->raise();
	}
	info.data()->setObject(object, type);
}

}

