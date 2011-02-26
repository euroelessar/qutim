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
	setAttribute(Qt::WA_MergeSoftkeysRecursively);

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
	action->setSoftKeyRole(QAction::PositiveSoftKey);
	actionBox->addAction(action);
	saveAction = new QAction(tr("Save"), actionBox);
	saveAction->setSoftKeyRole(QAction::PositiveSoftKey);
	connect(saveAction, SIGNAL(triggered()), SLOT(onSaveButton()));
	actionBox->addAction(saveAction);

	//Symbian close button
	action = new QAction(tr("Close"),this);
	action->setSoftKeyRole(QAction::NegativeSoftKey);
	connect(action, SIGNAL(triggered()), SLOT(close()));
	addAction(action);
}

void MobileContactInfoWindow::setObject(QObject *obj, RequestType type)
{
	object = obj;
	readWrite = type == InfoRequestCheckSupportEvent::ReadWrite;
	InfoRequestEvent event;
	qApp->sendEvent(object, &event);
	if (event.request())
		setRequest(event.request());
}

void MobileContactInfoWindow::setRequest(InfoRequest *req)
{
	if (request && req != request)
		request->deleteLater();
	if (request != req) {
		request = req;
		connect(request, SIGNAL(stateChanged(InfoRequest::State)),
				SLOT(onRequestStateChanged(InfoRequest::State)));
	}
	InfoRequest::State state = request->state();
	dataWidget.reset();
	avatarWidget.reset();
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
	saveAction->setVisible(readWrite);
	if (readWrite || !avatar.isEmpty()) {
		// avatar field
		DataItem avatarItem(QT_TRANSLATE_NOOP("ContactInfo", "Avatar"), QPixmap(avatar));
		avatarItem.setProperty("hideTitle", true);
		avatarItem.setProperty("imageSize", QSize(64, 64));
		avatarItem.setProperty("defaultImage", QPixmap(":/icons/qutim_64.png"));
		if (!readWrite)
			avatarItem.setReadOnly(true);
		avatarWidget.reset(AbstractDataForm::get(avatarItem));
		if (avatarWidget) {
			avatarWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			layout->addWidget(avatarWidget.data(), 0, Qt::AlignTop | Qt::AlignHCenter);
		}
	}
	DataItem item;
	if (state == InfoRequest::Done || state == InfoRequest::Cache)
		item = request->item();
	if (!item.isNull()) {
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
	if (state == InfoRequest::Done || state == InfoRequest::Cancel) {
		request->deleteLater(); request = 0;
	} else if (state == InfoRequest::Cache) {
		request->resend();
	}
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
	if (request != sender())
		return;
	Q_UNUSED(state);
	setRequest(request);
}

void MobileContactInfoWindow::onRequestButton()
{
	InfoRequestEvent event;
	qApp->sendEvent(object, &event);
	if (event.request())
		setRequest(event.request());
}

void MobileContactInfoWindow::onSaveButton()
{
	if (dataWidget) {
		InfoItemUpdatedEvent event(dataWidget->item());
		qApp->sendEvent(object, &event);
	}
	if (avatarWidget)
		object->setProperty("avatar", avatarWidget->item().property("imagePath", QString()));
}

MobileContactInfo::MobileContactInfo()
{
}

void MobileContactInfo::show(QObject *object)
{
	InfoRequestCheckSupportEvent event;
	qApp->sendEvent(object, &event);
	RequestType type = event.supportType();
	if (type == InfoRequestCheckSupportEvent::NoSupport)
		return;
	if (!info) {
		info = new MobileContactInfoWindow(qApp->activeWindow());
#ifdef Q_WS_MAEMO_5
		info->setParent(QApplication::activeWindow());
		info->setAttribute(Qt::WA_Maemo5StackedWindow);
#endif
		info->setWindowFlags(info->windowFlags() | Qt::Window);
		centerizeWidget(info);
		SystemIntegration::show(info);
		info->setAttribute(Qt::WA_DeleteOnClose, true);
	} else {
		info->raise();
	}
	info->setObject(object, type);
}

}
