#ifndef JDISKOITEM_H
#define JDISKOITEM_H

#include <QMetaType>
#include <QStringList>
#include <QSet>
#include <QSharedData>
#include <jreen/disco.h>

namespace Jabber
{
class JDiscoItemData;

class JDiscoItem
{
public:
	enum Action
	{
		ActionExecute,
		ActionRegister,
		ActionJoin,
		ActionSearch,
		ActionAdd,
		ActionVCard,
		ActionProxy
	};
	JDiscoItem();
	JDiscoItem(const JDiscoItem &other);
	~JDiscoItem();
	JDiscoItem &operator =(const JDiscoItem &other);
	void setName(const QString &name);
	void setJID(const QString &jid);
	void setNode(const QString &node);
	void setError(const QString &error);
	void setExpandable(bool expand);
	void addIdentity(const Jreen::Disco::Identity &identity);
	bool hasIdentity(const QString &category, const QString &type = QString()) const;
	void addFeature(const QString &feature);
	bool hasFeature(const QString &feature) const;
	void addAction(Action action);
	QString name() const;
	QString jid() const;
	QString node() const;
	QString error() const;
	bool isExpandable() const;
	QList<Jreen::Disco::Identity> identities() const;
	QList<Action> actions() const;
	QSet<QString> features() const;
private:
	QExplicitlySharedDataPointer<JDiscoItemData> d;
};
}

Q_DECLARE_METATYPE(Jabber::JDiscoItem)

#endif //JDISKOITEM_H
