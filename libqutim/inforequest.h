#ifndef INFOREQUEST_H
#define INFOREQUEST_H

#include "localizedstring.h"
#include <QSharedPointer>
#include <QVariant>

namespace qutim_sdk_0_3
{
	class InfoItemPrivate;

	class LIBQUTIM_EXPORT InfoItem
	{
	public:
		InfoItem();
		InfoItem(const QString &name, const LocalizedString &title, const QVariant &data,
				 const LocalizedStringList &group = LocalizedStringList());
		InfoItem(const LocalizedString &title, const QVariant &data,
				 const LocalizedStringList &group = LocalizedStringList());
		InfoItem(const InfoItem &item);
		~InfoItem();
		InfoItem &operator=(const InfoItem &item);
		QString name() const;
		void setName(const QString &name);
		LocalizedStringList group() const;
		void setGroup(const QList<LocalizedString> &group);
		LocalizedString title() const;
		void setTitle(const LocalizedString &title);
		QVariant data() const;
		void setData(const QVariant &data);
		bool isNull() const;
		QVariant property(const char *name, const QVariant &def = QVariant()) const;
		template<typename T>
		T property(const char *name, const T &def = T()) const
		{ return qVariantValue<T>(property(name, qVariantFromValue<T>(def))); }
		void setProperty(const char *name, const QVariant &value);
	protected:
		QSharedDataPointer<InfoItemPrivate> d;
	};

	class LIBQUTIM_EXPORT InfoRequest : public QObject
	{
		Q_OBJECT
		Q_DISABLE_COPY(InfoRequest)
	public:
		enum State {
			Request,
			Done,
			Cancel
		};
		InfoRequest();
		virtual ~InfoRequest();
		virtual QList<InfoItem> items() const = 0;
		virtual InfoItem item(const QString &name) const = 0;
		virtual State state() const = 0;
	signals:
		void stateChanged(InfoRequest::State state);
	};
}

#endif // INFOREQUEST_H
