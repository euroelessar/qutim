#include "inforequest.h"
#include "dynamicpropertydata_p.h"

namespace qutim_sdk_0_3
{
	class InfoItemPrivate : public DynamicPropertyData
	{
	public:
		QString name;
		LocalizedStringList group;
		LocalizedString title;
		QVariant data;

		QVariant getName() const { return QVariant::fromValue(name); }
		void setName(const QVariant &val) { name = val.value<QString>(); }
		QVariant getGroup() const { return QVariant::fromValue(group); }
		void setGroup(const QVariant &val) { group = val.value<LocalizedStringList>(); }
		QVariant getTitle() const { return QVariant::fromValue(title); }
		void setTitle(const QVariant &val) { title = val.value<LocalizedString>(); }
		QVariant getData() const { return data; }
		void setData(const QVariant &val) { data = val; }
	};

	namespace CompiledProperty
	{
		static QList<QByteArray> names = QList<QByteArray>()
										 << "name"
										 << "group"
										 << "title"
										 << "data";
		static QList<Getter> getters   = QList<Getter>()
										 << static_cast<Getter>(&InfoItemPrivate::getName)
										 << static_cast<Getter>(&InfoItemPrivate::getGroup)
										 << static_cast<Getter>(&InfoItemPrivate::getTitle)
										 << static_cast<Getter>(&InfoItemPrivate::getData);
		static QList<Setter> setters   = QList<Setter>()
										 << static_cast<Setter>(&InfoItemPrivate::setName)
										 << static_cast<Setter>(&InfoItemPrivate::setGroup)
										 << static_cast<Setter>(&InfoItemPrivate::setTitle)
										 << static_cast<Setter>(&InfoItemPrivate::setData);
	}

	static inline void ensure_data(QSharedDataPointer<InfoItemPrivate> &d)
	{
		if (!d)
			d = new InfoItemPrivate;
	}

	InfoItem::InfoItem()
	{
	}

	InfoItem::InfoItem(const QString &name, const LocalizedString &title,
					   const QVariant &data, const QList<LocalizedString> &group) :
		d(new InfoItemPrivate)
	{
		d->name = name;
		d->title = title;
		d->data = data;
		d->group = group;
	}

	InfoItem::InfoItem(const LocalizedString &title, const QVariant &data, const QList<LocalizedString> &group) :
		d(new InfoItemPrivate)
	{
		d->title = title;
		d->data = data;
		d->group = group;
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

	QVariant InfoItem::property(const char *name, const QVariant &def) const
	{
		return d->property(name, def, CompiledProperty::names, CompiledProperty::getters);
	}

	void InfoItem::setProperty(const char *name, const QVariant &value)
	{
		d->setProperty(name, value, CompiledProperty::names, CompiledProperty::setters);
	}

	InfoRequest::InfoRequest()
	{
	}

	InfoRequest::~InfoRequest()
	{
	}
}
