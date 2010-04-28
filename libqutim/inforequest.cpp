#include "inforequest.h"
#include "dynamicpropertydata_p.h"

Q_DECLARE_METATYPE(qutim_sdk_0_3::InfoItem);

namespace qutim_sdk_0_3
{
	class InfoItemPrivate : public DynamicPropertyData
	{
	public:
		InfoItemPrivate()  :
			maxCount(1) {}
		QString name;
		LocalizedStringList group;
		LocalizedString title;
		QVariant data;
		QList<InfoItem> subitems;
		int maxCount;
		InfoItem defaultSubitem;

		QVariant getName() const { return QVariant::fromValue(name); }
		void setName(const QVariant &val) { name = val.value<QString>(); }
		QVariant getTitle() const { return QVariant::fromValue(title); }
		void setTitle(const QVariant &val) { title = val.value<LocalizedString>(); }
		QVariant getData() const { return data; }
		void setData(const QVariant &val) { data = val; }
		QVariant getMaxCount() const { return maxCount; }
		void setMaxCount(const QVariant &val) { maxCount = val.toInt(); }
		QVariant getDefaultSubitem() const { return QVariant::fromValue(defaultSubitem); }
		void setDefaultSubitem(const QVariant &val) { defaultSubitem = val.value<InfoItem>(); }
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
										 << static_cast<Getter>(&InfoItemPrivate::getName)
										 << static_cast<Getter>(&InfoItemPrivate::getTitle)
										 << static_cast<Getter>(&InfoItemPrivate::getData)
										 << static_cast<Getter>(&InfoItemPrivate::getMaxCount)
										 << static_cast<Getter>(&InfoItemPrivate::getDefaultSubitem);
		static QList<Setter> setters   = QList<Setter>()
										 << static_cast<Setter>(&InfoItemPrivate::setName)
										 << static_cast<Setter>(&InfoItemPrivate::setTitle)
										 << static_cast<Setter>(&InfoItemPrivate::setData)
										 << static_cast<Setter>(&InfoItemPrivate::setMaxCount)
										 << static_cast<Setter>(&InfoItemPrivate::setDefaultSubitem);
	}

	static inline void ensure_data(QSharedDataPointer<InfoItemPrivate> &d)
	{
		if (!d)
			d = new InfoItemPrivate;
	}

	InfoItem::InfoItem()
	{
	}

	InfoItem::InfoItem(const QString &name, const LocalizedString &title, const QVariant &data) :
		d(new InfoItemPrivate)
	{
		d->name = name;
		d->title = title;
		d->data = data;
	}

	InfoItem::InfoItem(const LocalizedString &title, const QVariant &data) :
		d(new InfoItemPrivate)
	{
		d->title = title;
		d->data = data;
	}

	InfoItem::InfoItem(const InfoItem &item) :
		d(item.d)
	{
	}

	InfoItem::~InfoItem()
	{
	}

	InfoItem &InfoItem::operator=(const InfoItem &item)
	{
		d = item.d;
		return *this;
	}

	QString InfoItem::name() const
	{
		if (d->name.isEmpty())
			return d->title.original();
		else
			return d->name;
	}

	void InfoItem::setName(const QString &name)
	{
		ensure_data(d);
		d->name = name;
	}

	LocalizedStringList InfoItem::group() const
	{
		return d->group;
	}

	void InfoItem::setGroup(const QList<LocalizedString> &group)
	{
		ensure_data(d);
		d->group = group;
	}

	LocalizedString InfoItem::title() const
	{
		return d->title;
	}

	void InfoItem::setTitle(const LocalizedString &title)
	{
		d->title = title;
	}

	QVariant InfoItem::data() const
	{
		return d->data;
	}

	void InfoItem::setData(const QVariant &data)
	{
		ensure_data(d);
		d->data = data;
	}

	bool InfoItem::isNull() const
	{
		return !d.constData();
	}

	QList<InfoItem> InfoItem::subitems() const
	{
		return d->subitems;
	}

	void InfoItem::addSubitem(const InfoItem &item)
	{
		ensure_data(d);
		d->subitems << item;
	}

	bool InfoItem::hasSubitems() const
	{
		return !d->subitems.isEmpty();
	}

	void InfoItem::setMultiple(const InfoItem &defaultSubitem, int maxCount)
	{
		ensure_data(d);
		d->defaultSubitem = defaultSubitem;
		d->maxCount = maxCount;
	}

	bool InfoItem::isMultiple() const
	{
		return d->maxCount != 1;
	}

	int InfoItem::maxCount() const
	{
		return d->maxCount;
	}

	InfoItem InfoItem::defaultSubitem() const
	{
		return d->defaultSubitem;
	}

	QVariant InfoItem::property(const char *name, const QVariant &def) const
	{
		return d->property(name, def, CompiledProperty::names, CompiledProperty::getters);
	}

	void InfoItem::setProperty(const char *name, const QVariant &value)
	{
		ensure_data(d);
		d->setProperty(name, value, CompiledProperty::names, CompiledProperty::setters);
	}

	InfoRequest::InfoRequest()
	{
	}

	InfoRequest::~InfoRequest()
	{
	}

	void InfoRequest::resend() const
	{
	}

	void InfoRequest::virtual_hook(int id, void *data)
	{
		Q_UNUSED(id);
		Q_UNUSED(data);
	}
}
