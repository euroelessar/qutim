#ifndef JPERSONACTIVITYCONVERTER_H
#define JPERSONACTIVITYCONVERTER_H

#include "../../../sdk/jabber.h"
#include <qutim/localizedstring.h>
#include <qutim/extensionicon.h>

namespace Jabber {

	using namespace qutim_sdk_0_3;

	class JPersonActivity
	{
	public:
		JPersonActivity() {}
		JPersonActivity(const QString &name, const LocalizedString &value, const QString &generalActivity);
		QString name() const { return m_name; }
		LocalizedString value() const { return m_value; }
		QString generalActivity() const { return m_general; }
		bool isEmpty() const { return m_name.isEmpty(); }
		bool isGeneral() const { return m_general.isEmpty(); }
		QString iconName() const;
		ExtensionIcon icon() const;
	private:
		QString m_name;
		LocalizedString m_value;
		QString m_general; // Preferred general activity. If empty, that activity is general.
	};

	class JPersonActivityConverter : public QObject, public PersonEventConverter
	{
		Q_OBJECT
		Q_INTERFACES(Jabber::PersonEventConverter)
	public:
		JPersonActivityConverter();
		virtual ~JPersonActivityConverter();
		virtual std::string feature() const;
		virtual QString name() const;
		virtual gloox::Tag *toXml(const QVariantHash &map) const;
		virtual QVariantHash fromXml(gloox::Tag *tag) const;
		static QMultiHash<QString, JPersonActivity> *allActivities();
	private:
		std::string m_feature;
	};

	class JPersonActivityRegistrator : public QObject
	{
		Q_OBJECT
	public:
		JPersonActivityRegistrator();
	protected:
		bool eventFilter(QObject *obj, QEvent *event);
	};

} // namespace Jabber

#endif // JPERSONACTIVITYCONVERTER_H
