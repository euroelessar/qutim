/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "dataforms.h"
#include "dynamicpropertydata_p.h"
#include "objectgenerator.h"
#include "servicemanager.h"

#include <QValidator>

Q_DECLARE_METATYPE(QList<QIcon>)
Q_DECLARE_METATYPE(QList<QPixmap>)
Q_DECLARE_METATYPE(QList<QImage>)
Q_DECLARE_METATYPE(QValidator*)

namespace qutim_sdk_0_3
{
class DataItemPrivate : public DynamicPropertyData
{
public:
	DataItemPrivate()  :
		maxCount(1),
		parent(0),
		onDataChangedReceiver(0),
		onDataChangedMethod(0)
	{}
	QString name;
	LocalizedString title;
	QVariant data;
	QList<DataItem> subitems;
	int maxCount;
	DataItem defaultSubitem;
	DataItemPrivate *parent;
	QObject *onDataChangedReceiver;
	QLatin1String onDataChangedMethod;

	QVariant property(const char *name, const QVariant &def) const;
	DataItem foundSubitem(const QString &name, bool recursive, bool removeItem);

	QVariant getName() const { return QVariant::fromValue(name); }
	void setName(const QVariant &val) { name = val.value<QString>(); }
	QVariant getTitle() const { return QVariant::fromValue(title); }
	void setTitle(const QVariant &val) { title = val.value<LocalizedString>(); }
	QVariant getData() const { return data; }
	void setData(const QVariant &val) { data = val; }
	QVariant getMaxCount() const { return maxCount; }
	void setMaxCount(const QVariant &val) { maxCount = val.toInt(); }
	QVariant getDefaultSubitem() const { return QVariant::fromValue(defaultSubitem); }
	void setDefaultSubitem(const QVariant &val) { defaultSubitem = val.value<DataItem>(); }
	QVariant getOnDataChangedReceiver() const { return QVariant::fromValue(onDataChangedReceiver); }
	void setOnDataChangedReceiver(const QVariant &val) { onDataChangedReceiver = val.value<QObject*>(); }
	QVariant getOnDataChangedMethod() const { return onDataChangedMethod; }
	void setOnDataChangedMethod(const QVariant &val) { onDataChangedMethod = QLatin1String(val.toString().toLatin1()); }

	static void updateParents(QList<DataItem> &subitems, QSharedDataPointer<DataItemPrivate> &parent);
};

namespace CompiledProperty
{
static QList<QByteArray> names = QList<QByteArray>()
	<< "name"
	<< "title"
	<< "data"
	<< "maxCount"
	<< "defaultSubitem"
	<< "onDataChangedReceiver"
	<< "onDataChangedMethod";
static QList<Getter> getters = QList<Getter>()
	<< static_cast<Getter>(&DataItemPrivate::getName)
	<< static_cast<Getter>(&DataItemPrivate::getTitle)
	<< static_cast<Getter>(&DataItemPrivate::getData)
	<< static_cast<Getter>(&DataItemPrivate::getMaxCount)
	<< static_cast<Getter>(&DataItemPrivate::getDefaultSubitem)
	<< static_cast<Getter>(&DataItemPrivate::getOnDataChangedReceiver)
	<< static_cast<Getter>(&DataItemPrivate::getOnDataChangedMethod);
static QList<Setter> setters = QList<Setter>()
	<< static_cast<Setter>(&DataItemPrivate::setName)
	<< static_cast<Setter>(&DataItemPrivate::setTitle)
	<< static_cast<Setter>(&DataItemPrivate::setData)
	<< static_cast<Setter>(&DataItemPrivate::setMaxCount)
	<< static_cast<Setter>(&DataItemPrivate::setDefaultSubitem)
	<< static_cast<Setter>(&DataItemPrivate::setOnDataChangedReceiver)
	<< static_cast<Setter>(&DataItemPrivate::setOnDataChangedMethod);
}

static inline void ensure_data(QSharedDataPointer<DataItemPrivate> &d)
{
	if (!d)
		d = new DataItemPrivate;
}

inline void DataItemPrivate::updateParents(QList<DataItem> &subitems, QSharedDataPointer<DataItemPrivate> &parent)
{
	if (subitems.isEmpty())
		return;
	DataItemPrivate *p = parent.data();
	for (QList<DataItem>::iterator itr = subitems.begin(), end = subitems.end(); itr != end; ++itr)
		itr->d->parent = p;
}

DataItem::DataItem()
{
}

DataItem::DataItem(const QString &name, const LocalizedString &title, const QVariant &data) :
	d(new DataItemPrivate)
{
	d->name = name;
	d->title = title;
	d->data = data;
}

DataItem::DataItem(const LocalizedString &title, const QVariant &data) :
	d(new DataItemPrivate)
{
	d->title = title;
	d->data = data;
}

DataItem::DataItem(const DataItem &other) :
	d(other.d)
{
	if (d) {
		if (d->parent)
			d->parent = 0;
		DataItemPrivate::updateParents(d->subitems, d);
	}
}

DataItem::~DataItem()
{
}

DataItem &DataItem::operator=(const DataItem &other)
{
	d = other.d;
	if (d) {
		if (d->parent)
			d->parent = 0;
		DataItemPrivate::updateParents(d->subitems, d);
	}
	return *this;
}

QString DataItem::name() const
{
	return d->name;
}

void DataItem::setName(const QString &itemName)
{
	ensure_data(d);
	d->name = itemName;
}

LocalizedString DataItem::title() const
{
	return d ? d->title : LocalizedString();
}

void DataItem::setTitle(const LocalizedString &itemTitle)
{
	ensure_data(d);
	d->title = itemTitle;
}

QVariant DataItem::data() const
{
	if (!d) return QVariant();
	return d->data;
}

void DataItem::setData(const QVariant &itemData)
{
	ensure_data(d);
	d->data = itemData;
}

bool DataItem::isNull() const
{
	return !d.constData();
}

QList<DataItem> DataItem::subitems() const
{
	return d ? d->subitems : QList<DataItem>();
}

void DataItem::setSubitems(const QList<DataItem> &newSubitems)
{
	if (d && !d->subitems.isEmpty()) {
		for (QList<DataItem>::iterator itr = d->subitems.begin(), end = d->subitems.end(); itr != end; ++itr)
			itr->d->parent = 0;
	}
	ensure_data(d);
	d->subitems = newSubitems;
	DataItemPrivate::updateParents(d->subitems, d);
}

DataItem DataItemPrivate::foundSubitem(const QString &name, bool recursive, bool removeItem)
{
	QMutableListIterator<DataItem> itr(subitems);
	while (itr.hasNext()) {
		DataItem item = itr.next();
		if (itr.value().name() == name) {
			if (removeItem)
				itr.remove();
			return item;
		}
		if (recursive) {
			item = itr.value().d->foundSubitem(name, recursive, removeItem);
			if (!item.isNull())
				return item;
		}
	}
	return DataItem();
}

DataItem DataItem::subitem(const QString &name, bool recursive) const
{
	return d ? const_cast<DataItemPrivate*>(d.data())->foundSubitem(name, recursive, false) : DataItem();
}

int DataItem::removeSubitems(const QString &name, bool recursive)
{
	if (d)
		return 0;
	int c = 0;
	QMutableListIterator<DataItem> itr(d->subitems);
	while (itr.hasNext()) {
		itr.next();
		if (itr.value().name() == name) {
			itr.remove();
			++c;
			continue;
		}
		if (recursive) {
			c += itr.value().removeSubitem(name, recursive);
		}
	}
	return c;
}

bool DataItem::removeSubitem(const QString &name, bool recursive)
{
	return d ? !d->foundSubitem(name, recursive, true).isNull() : false;
}

DataItem DataItem::takeSubitem(const QString &name, bool recursive)
{
	return d ? d->foundSubitem(name, recursive, true) : DataItem();
}

void DataItem::addSubitem(const DataItem &subitem)
{
	ensure_data(d);
	d->subitems << subitem;
	DataItem &val = d->subitems.last();
	ensure_data(val.d);
	val.d->parent = d.data();
}

bool DataItem::hasSubitems() const
{
	return d && !d->subitems.isEmpty();
}

void DataItem::allowModifySubitems(const DataItem &defaultSubitem, int maxSubitemsCount)
{
	ensure_data(d);
	d->defaultSubitem = defaultSubitem;
	d->maxCount = maxSubitemsCount;
}

bool DataItem::isAllowedModifySubitems() const
{
	return d && d->maxCount != 1 && !isReadOnly();
}

int DataItem::maxSubitemsCount() const
{
	return d ? d->maxCount : 1;
}

DataItem DataItem::defaultSubitem() const
{
	return d ? d->defaultSubitem : DataItem();
}

bool DataItem::isReadOnly() const
{
	return property("readOnly", false);
}

void DataItem::setReadOnly(bool readOnly)
{
	setProperty("readOnly", readOnly);
}

void DataItem::setDataChangedHandler(QObject *receiver, const char *method)
{
	ensure_data(d);
	d->onDataChangedReceiver = receiver;
	d->onDataChangedMethod = QLatin1String(method);
}

QObject *DataItem::dataChangedReceiver() const
{
	return d ? d->onDataChangedReceiver : 0;
}

const char *DataItem::dataChangedMethod() const
{
	return d ? d->onDataChangedMethod.latin1() : 0;
}

DataItem &DataItem::operator<<(const DataItem &subitem)
{
	addSubitem(subitem);
	return *this;
}

QVariant DataItemPrivate::property(const char *name, const QVariant &def) const
{
	QByteArray prop = QByteArray::fromRawData(name, strlen(name));
	int id = CompiledProperty::names.indexOf(prop);
	if (id < 0) {
		for (const DataItemPrivate *p = this; p != 0; p = p->parent) {
			id = p->names.indexOf(prop);
			if (id >= 0)
				return p->values.at(id);
		}
		return def;
	}
	return (this->*CompiledProperty::getters.at(id))();
}

QVariant DataItem::property(const char *name, const QVariant &def) const
{
	if (!d && !qstrcmp(name, "maxCount"))
		return 1;
	return d ? d->property(name, def) : QVariant();
}

QList<QByteArray> DataItem::dynamicPropertyNames() const
{
	return d ? d->names : QList<QByteArray>();
}

void DataItem::setProperty(const char *name, const QVariant &value)
{
	ensure_data(d);
	d->setProperty(name, value, CompiledProperty::names, CompiledProperty::setters);
}

ReadOnlyDataItem::ReadOnlyDataItem(const LocalizedString &title, const QStringList &data) :
	DataItem(title, data)
{
}

ReadOnlyDataItem::ReadOnlyDataItem(const LocalizedString &title, const LocalizedStringList &data) :
	DataItem(title, QVariant::fromValue(data))
{
}

ReadOnlyDataItem::ReadOnlyDataItem(const LocalizedString &title, bool data) :
	DataItem(title, data)
{
}

ReadOnlyDataItem::ReadOnlyDataItem(const LocalizedString &title, const QDate &data) :
	DataItem(title, data)
{
}

ReadOnlyDataItem::ReadOnlyDataItem(const LocalizedString &title, const QDateTime &data) :
	DataItem(title, data)
{
}

ReadOnlyDataItem::ReadOnlyDataItem(const LocalizedString &title, int data) :
	DataItem(title, data)
{
}

ReadOnlyDataItem::ReadOnlyDataItem(const LocalizedString &title, uint data) :
	DataItem(title, data)
{
}

ReadOnlyDataItem::ReadOnlyDataItem(const LocalizedString &title, double data) :
	DataItem(title, data)
{
}

ReadOnlyDataItem::ReadOnlyDataItem(const LocalizedString &title, const QIcon &data) :
	DataItem(title, data)
{
}

ReadOnlyDataItem::ReadOnlyDataItem(const LocalizedString &title, const QImage &data) :
	DataItem(title, data)
{
}

ReadOnlyDataItem::ReadOnlyDataItem(const LocalizedString &title, const QPixmap &data) :
	DataItem(title, data)
{
}

ReadOnlyDataItem::ReadOnlyDataItem(const LocalizedString &title, const QString &data) :
	DataItem(title, data)
{
}

ReadOnlyDataItem::ReadOnlyDataItem(const LocalizedString &title, const LocalizedString &data) :
	DataItem(title, QVariant::fromValue(data))
{
}

StringListDataItem::StringListDataItem(const QString &name, const LocalizedString &title,
									   const QStringList &data, int maxStringsCount) :
	DataItem(name, title, data)
{
	setProperty("maxStringsCount", maxStringsCount);
}

StringListDataItem::StringListDataItem(const QString &name, const LocalizedString &title,
									   const LocalizedStringList &data, int maxStringsCount) :
	DataItem(name, title, QVariant::fromValue(data))
{
	setProperty("maxStringsCount", maxStringsCount);
}

IntDataItem::IntDataItem(const QString &name, const LocalizedString &title, int data,
						 int minValue, int maxValue) :
	DataItem(name, title, data)
{
	setProperty("minValue", minValue);
	setProperty("maxValue", maxValue);
}

DoubleDataItem::DoubleDataItem(const QString &name, const LocalizedString &title, double data,
							   double minValue, double maxValue) :
	DataItem(name, title, data)
{
	setProperty("minValue", minValue);
	setProperty("maxValue", maxValue);
}

IconDataItem::IconDataItem(const QString &name, const LocalizedString &title, const QIcon &data,
						   const QSize &imageSize, const QList<QIcon> &alternatives) :
	DataItem(name, title, data)
{
	setProperty("imageSize", imageSize);
	setProperty("alternatives", QVariant::fromValue(alternatives));
}

ImageDataItem::ImageDataItem(const QString &name, const LocalizedString &title, const QImage &data,
							 const QSize &imageSize, const QList<QImage> &alternatives) :
	DataItem(name, title, data)
{
	setProperty("imageSize", imageSize);
	setProperty("alternatives", QVariant::fromValue(alternatives));
}

PixmapDataItem::PixmapDataItem(const QString &name, const LocalizedString &title, const QPixmap &data,
							   const QSize &imageSize, const QList<QPixmap> &alternatives) :
	DataItem(name, title, data)
{
	setProperty("imageSize", imageSize);
	setProperty("alternatives", QVariant::fromValue(alternatives));
}

StringChooserDataItem::StringChooserDataItem(const QString &name, const LocalizedString &title,
											 const QStringList &alternatives, const QString &data,
											 bool editable, QValidator *validator) :
	DataItem(name, title, data)
{
	setProperty("alternatives", QVariant::fromValue(alternatives));
	setProperty("editable", editable);
	if (validator)
		setProperty("validator", QVariant::fromValue(validator));
}

StringChooserDataItem::StringChooserDataItem(const QString &name, const LocalizedString &title,
											 const QStringList &alternatives, const QString &data,
											 bool editable, QRegExp validator) :
	DataItem(name, title, data)
{
	setProperty("alternatives", QVariant::fromValue(alternatives));
	setProperty("editable", editable);
	if (!validator.isEmpty())
		setProperty("validator", QVariant::fromValue(validator));
}

StringChooserDataItem::StringChooserDataItem(const QString &name, const LocalizedString &title,
											 const LocalizedStringList &alternatives, const LocalizedString &data,
											 bool editable, QValidator *validator) :
	DataItem(name, title, QVariant::fromValue(data))
{
	setProperty("alternatives", QVariant::fromValue(alternatives));
	setProperty("editable", editable);
	if (validator)
		setProperty("validator", QVariant::fromValue(validator));
}

StringChooserDataItem::StringChooserDataItem(const QString &name, const LocalizedString &title,
											 const LocalizedStringList &alternatives, const LocalizedString &data,
											 bool editable, QRegExp validator) :
	DataItem(name, title, QVariant::fromValue(data))
{
	setProperty("alternatives", QVariant::fromValue(alternatives));
	setProperty("editable", editable);
	if (!validator.isEmpty())
		setProperty("validator", QVariant::fromValue(validator));
}

MultiLineStringDataItem::MultiLineStringDataItem(const QString &name, const LocalizedString &title,
												 const QString &data) :
	DataItem(name, title, data)
{
	setProperty("multiline", true);
}

MultiLineStringDataItem::MultiLineStringDataItem(const QString &name, const LocalizedString &title,
												 const LocalizedString &data) :
	DataItem(name, title, QVariant::fromValue(data))
{
	setProperty("multiline", true);
}

StringDataItem::StringDataItem(const QString &name, const LocalizedString &title,
							   const QString &data, QValidator *validator, bool password) :
	DataItem(name, title, data)
{
	if (validator)
		setProperty("validator", QVariant::fromValue(validator));
	setProperty("password", password);
}

StringDataItem::StringDataItem(const QString &name, const LocalizedString &title,
							   const QString &data, QRegExp validator, bool password) :
	DataItem(name, title, data)
{
	if (!validator.isEmpty())
		setProperty("validator", QVariant::fromValue(validator));
	setProperty("password", password);
}

StringDataItem::StringDataItem(const QString &name, const LocalizedString &title,
							   const LocalizedString &data, QValidator *validator, bool password) :
	DataItem(name, title, QVariant::fromValue(data))
{
	if (validator)
		setProperty("validator", QVariant::fromValue(validator));
	setProperty("password", password);
}

StringDataItem::StringDataItem(const QString &name, const LocalizedString &title,
							   const LocalizedString &data, QRegExp validator, bool password) :
	DataItem(name, title, QVariant::fromValue(data))
{
	if (!validator.isEmpty())
		setProperty("validator", QVariant::fromValue(validator));
	setProperty("password", password);
}

AbstractDataForm *AbstractDataForm::get(const DataItem &item,
										StandardButtons standartButtons,
										const AbstractDataForm::Buttons &buttons)
{
	DataFormsBackend *b = DataFormsBackend::instance();
	if (b)
		return b->get(item, standartButtons, buttons);
	else
		return 0;
}

void AbstractDataForm::accept()
{
	emit accepted();
}

void AbstractDataForm::reject()
{
	emit rejected();
}

void AbstractDataForm::virtual_hook(int id, void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(data);
}

DataFormsBackend *DataFormsBackend::instance()
{
    static QPointer<DataFormsBackend> self;
	if(self.isNull() && ObjectGenerator::isInited())
		self = qobject_cast<DataFormsBackend *>(ServiceManager::getByName("DataFormsBackend"));
    return self.data();
}

}

