#include "quickcontactinfo.h"
#include <qutim/icon.h>
#include <qutim/contact.h>
#include <qutim/account.h>
#include <qutim/dataforms.h>
#include <QApplication>
#include <QDebug>

namespace MeegoIntegration {

using namespace qutim_sdk_0_3;

typedef InfoRequestFactory::SupportLevel SupportLevel;

QuickContactInfo::QuickContactInfo()
    : m_request(0),
      m_object(0),
      m_readWrite(false)
{
}

QObject *QuickContactInfo::object()
{
    return m_object;
}

void QuickContactInfo::setObject(QObject *object)
{
    show(object);
}

DataItem QuickContactInfo::item()
{
    return m_item;
}

bool QuickContactInfo::readOnly()
{
    return !m_readWrite;
}

void QuickContactInfo::show(QObject *object)
{
	InfoRequestFactory *factory = InfoRequestFactory::factory(object);
    SupportLevel type = factory ? factory->supportLevel(object) : InfoRequestFactory::Unavailable;
    if (type <= InfoRequestFactory::Unavailable)
        object = 0;
    
    if (m_object != object) {
        m_object = object;
        emit objectChanged(object);
    }
    if (!object) {
        m_item = DataItem("No data");
        emit itemChanged(m_item);
        delete m_request;
        m_request = 0;
        return;
    }
	bool readWrite = (type == InfoRequestFactory::ReadWrite);
    if (m_readWrite != readWrite) {
        m_readWrite = readWrite;
        emit readOnlyChanged(!readWrite);
    }
    delete m_request;
	m_request = InfoRequestFactory::dataFormRequest(object);

	if (m_request) {
		connect(m_request, SIGNAL(stateChanged(qutim_sdk_0_3::InfoRequest::State)),
				SLOT(onRequestStateChanged(qutim_sdk_0_3::InfoRequest::State)));
		m_request->requestData();
	}

	QString title;
	QString avatar;
	if (Buddy *buddy = qobject_cast<Buddy*>(m_object)) {
		title = QApplication::translate("ContactInfo", "About contact %1 <%2>")
					.arg(buddy->name())
					.arg(buddy->id());
	} else {
		if (Account *account = qobject_cast<Account*>(m_object)) {
			title = QApplication::translate("ContactInfo", "About account %1")
						.arg(account->name());
		} else {
			title = QApplication::translate("ContactInfo", "About %1 <%2>")
						.arg(m_object->property("name").toString())
						.arg(m_object->property("id").toString());
		}
	}
	if (m_request)
		onRequestStateChanged(m_request->state());
    emit showRequest();
}

void QuickContactInfo::request()
{
    if (m_request) {
        m_request->cancel();
        m_request->requestData();
    }
}

void QuickContactInfo::save(const qutim_sdk_0_3::DataItem &item)
{
	if (m_request) {
		m_request->cancel();
		m_request->updateData(item);
	}
}

DataItem QuickContactInfo::filterItems(const DataItem &item, bool readOnly)
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

void QuickContactInfo::filterItemsHelper(const DataItem &item, DataItem &result, bool readOnly)
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

bool QuickContactInfo::isItemEmpty(const DataItem &item)
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

void QuickContactInfo::onRequestStateChanged(InfoRequest::State state)
{
	if (state == InfoRequest::RequestDone)
	{
		DataItem item = m_request->dataItem();
		if (!m_readWrite) {
			item = filterItems(item, true);
			item.setProperty("readOnly", true);
		} else {
			item = filterItems(item);
		}
        m_item = item;
        emit itemChanged(item);
	}
}

} // namespace MeegoIntegration
