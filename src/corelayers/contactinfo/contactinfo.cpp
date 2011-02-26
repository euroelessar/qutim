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

void MainWindow::setObject(QObject *obj, RequestType type)
{
	object = obj;
	readWrite = type == InfoRequestCheckSupportEvent::ReadWrite;
	InfoRequestEvent event;
	qApp->sendEvent(object, &event);
	if (event.request())
		setRequest(event.request());
}

void MainWindow::setRequest(InfoRequest *req)
{
	if (request && req != request)
		request->deleteLater();
	int curPage = ui.detailsStackedWidget->currentIndex();
	if (request != req) {
		request = req;
		connect(request, SIGNAL(stateChanged(InfoRequest::State)),
				SLOT(onRequestStateChanged(InfoRequest::State)));
	}
	ui.infoListWidget->clear();
	QWidget *w;
	while ((w = ui.detailsStackedWidget->widget(0)) != 0)
		delete w;
	Buddy *buddy = qobject_cast<Buddy*>(object);
	QString avatar;
	if (buddy) {
		setWindowTitle(QT_TRANSLATE_NOOP("ContactInfo", "About contact %1 <%2>")
					   .toString()
					   .arg(buddy->name())
					   .arg(buddy->id()));
		avatar = buddy->avatar();
	} else {
		Account *account = qobject_cast<Account*>(object);
		if (account) {
			setWindowTitle(QT_TRANSLATE_NOOP("ContactInfo", "About account %1")
						   .toString()
						   .arg(account->name()));
		} else {
			setWindowTitle(QT_TRANSLATE_NOOP("ContactInfo", "About %1 <%2>")
						   .toString()
						   .arg(object->property("name").toString())
						   .arg(object->property("id").toString()));
		}
		avatar = object->property("avatar").toString();
	}
	ui.saveButton->setVisible(readWrite);
	{ // avatar field
		DataItem avatarItem(QT_TRANSLATE_NOOP("ContactInfo", "Avatar"), QPixmap(avatar));
		avatarItem.setProperty("hideTitle", true);
		avatarItem.setProperty("imageSize", QSize(64, 64));
		avatarItem.setProperty("defaultImage", QPixmap(":/icons/qutim_64.png"));
		if (!readWrite)
			avatarItem.setReadOnly(true);
		avatarWidget.reset(AbstractDataForm::get(avatarItem));
		if (avatarWidget) {
			avatarWidget->setParent(this);
			avatarWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			ui.gridLayout->addWidget(avatarWidget.data(), 0, 0, Qt::AlignCenter);
		}
	}
	addItems(request->item());
	if (curPage >= 0)
		ui.infoListWidget->setCurrentRow(curPage);
	InfoRequest::State state = request->state();
	if (state == InfoRequest::Done || state == InfoRequest::Cancel) {
		request->deleteLater(); request = 0;
	} else if (state == InfoRequest::Cache) {
		request->resend();
	}
}

void MainWindow::onRequestStateChanged(InfoRequest::State state)
{
	if (request != sender())
		return;
	Q_UNUSED(state);
	setRequest(request);
}

void MainWindow::onRequestButton()
{
	InfoRequestEvent event;
	qApp->sendEvent(object, &event);
	if (event.request())
		setRequest(event.request());
}

void MainWindow::onSaveButton()
{
	DataItem items;
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
	InfoItemUpdatedEvent event(items);
	qApp->sendEvent(object, &event);
	if (avatarWidget)
		object->setProperty("avatar", avatarWidget->item().property("imagePath", QString()));
}

void MainWindow::addItems(const DataItem &items)
{
	if (items.isNull() || !items.hasSubitems())
		return;
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
	ui.infoListWidget->addItem(QT_TRANSLATE_NOOP("ContactInfo", "Summary"));
	ui.detailsStackedWidget->addWidget(scrollArea);
	// Pages
	DataItem general;
	foreach (const DataItem &item, items.subitems()) {
		if (item.hasSubitems()) {
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

QString MainWindow::summary(const DataItem &items)
{
	QString text;
	bool first = true;
	foreach (const DataItem &item, items.subitems()) {
		if (item.property("additional", false) || item.property("notSet", false))
			continue;
		if (item.hasSubitems()) {
			text += summary(item);
		} else if (item.data().canConvert(QVariant::String)) {
			QString str = item.data().toString();
			if (str.isEmpty())
				continue;
			if (first) {
				text += QString("<b>[%1]:</b><br>").arg(items.title());
				first = false;
			}
			text += QString("<b>%1:</b>  ").arg(item.title());
			QVariant::Type type = item.data().type();
			if (type == QVariant::Date)
				text += item.data().toDate().toString(Qt::SystemLocaleLongDate);
			else if (type == QVariant::DateTime)
				text += item.data().toDateTime().toString(Qt::SystemLocaleLongDate);
			else if (type == QVariant::Bool)
				text += item.data().toBool() ?
							QT_TRANSLATE_NOOP("ContactInfo", "yes") :
							QT_TRANSLATE_NOOP("ContactInfo", "no");
			else
				text += str.replace(QRegExp("(\r\n|\n|\r)"), "<br>");
			text += "<br>";
		}
	}
	return text;
}

ContactInfo::ContactInfo()
{
}

void ContactInfo::show(QObject *object)
{
	InfoRequestCheckSupportEvent event;
	qApp->sendEvent(object, &event);
	RequestType type = event.supportType();
	if (type == InfoRequestCheckSupportEvent::NoSupport)
		return;
	if (!info) {
		info = new MainWindow();
		centerizeWidget(info);
		SystemIntegration::show(info);
		info->setAttribute(Qt::WA_DeleteOnClose, true);
	} else {
		info->raise();
	}
	info->setObject(object, type);
}

}
