#include "dataforms.h"
#include "dynamicpropertydata_p.h"
#include <QPointer>

Q_DECLARE_METATYPE(qutim_sdk_0_3::DataItem);

namespace qutim_sdk_0_3
{
	class DataItemPrivate : public DynamicPropertyData
	{
	public:
		DataItemPrivate()  :
				maxCount(1) {}
		QString name;
		LocalizedString title;
		QVariant data;
		QList<DataItem> subitems;
		int maxCount;
		DataItem defaultSubitem;

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
	};

	namespace CompiledProperty
	{
		static QList<QByteArray> names = QList<QByteArray>()
										 << "name"
										 << "title"
										 << "data"
										 << "maxCount"
										 << "defaultSubitem";
		static QList<Getter> getters   = QList<Getter>()
										 << static_cast<Getter>(&DataItemPrivate::getName)
										 << static_cast<Getter>(&DataItemPrivate::getTitle)
										 << static_cast<Getter>(&DataItemPrivate::getData)
										 << static_cast<Getter>(&DataItemPrivate::getMaxCount)
										 << static_cast<Getter>(&DataItemPrivate::getDefaultSubitem);
		static QList<Setter> setters   = QList<Setter>()
										 << static_cast<Setter>(&DataItemPrivate::setName)
										 << static_cast<Setter>(&DataItemPrivate::setTitle)
										 << static_cast<Setter>(&DataItemPrivate::setData)
										 << static_cast<Setter>(&DataItemPrivate::setMaxCount)
										 << static_cast<Setter>(&DataItemPrivate::setDefaultSubitem);
	}

	static inline void ensure_data(QSharedDataPointer<DataItemPrivate> &d)
	{
		if (!d)
			d = new DataItemPrivate;
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

	DataItem::DataItem(const DataItem &item) :
			d(item.d)
	{
	}

	DataItem::~DataItem()
	{
	}

	DataItem &DataItem::operator=(const DataItem &item)
								 {
		d = item.d;
		return *this;
	}

	QString DataItem::name() const
	{
		if (d->name.isEmpty())
			return d->title.original();
		else
			return d->name;
	}

	void DataItem::setName(const QString &name)
	{
		ensure_data(d);
		d->name = name;
	}

	LocalizedString DataItem::title() const
	{
		return d->title;
	}

	void DataItem::setTitle(const LocalizedString &title)
	{
		d->title = title;
	}

	QVariant DataItem::data() const
	{
		if (!d) return QVariant();
		return d->data;
	}

	void DataItem::setData(const QVariant &data)
	{
		ensure_data(d);
		d->data = data;
	}

	bool DataItem::isNull() const
	{
		return !d.constData();
	}

	QList<DataItem> DataItem::subitems() const
	{
		return d->subitems;
	}

	DataItem DataItem::subitem(const QString &name, bool recursive) const
	{
		if (!d)
			return DataItem();
		foreach (const DataItem &item, d->subitems) {
			if (item.name() == name)
				return item;
			if (recursive) {
				DataItem res = item.subitem(name);
				if (!res.isNull())
					return res;
			}
		}
		return DataItem();
	}

	void DataItem::addSubitem(const DataItem &item)
	{
		ensure_data(d);
		d->subitems << item;
	}

	bool DataItem::hasSubitems() const
	{
		return !d->subitems.isEmpty();
	}

	void DataItem::setMultiple(const DataItem &defaultSubitem, int maxCount)
	{
		ensure_data(d);
		d->defaultSubitem = defaultSubitem;
		d->maxCount = maxCount;
	}

	bool DataItem::isMultiple() const
	{
		return d->maxCount != 1;
	}

	int DataItem::maxCount() const
	{
		return d->maxCount;
	}

	DataItem DataItem::defaultSubitem() const
	{
		return d->defaultSubitem;
	}

	bool DataItem::isReadOnly() const
	{
		return property("readOnly", false);
	}

	void DataItem::setReadOnly(bool readOnly)
	{
		setProperty("readOnly", readOnly);
	}

	QVariant DataItem::property(const char *name, const QVariant &def) const
	{
		return d->property(name, def, CompiledProperty::names, CompiledProperty::getters);
	}

	void DataItem::setProperty(const char *name, const QVariant &value)
	{
		ensure_data(d);
		d->setProperty(name, value, CompiledProperty::names, CompiledProperty::setters);
	}

	void AbstractDataWidget::virtual_hook(int id, void *data)
	{
		Q_UNUSED(id);
		Q_UNUSED(data);
	}

	QWidget *AbstractDataForm::get(const DataItem &item, StandardButtons standartButtons,
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

	DataFormsBackend *DataFormsBackend::instance()
	{
		static QPointer<DataFormsBackend> self;
		if(self.isNull() && isCoreInited())
			self = qobject_cast<DataFormsBackend *>(getService("DataFormsBackend"));
		return self;
	}

}
