#include "jdiscoitem.h"
#include <jreen/disco.h>

namespace Jabber
{
class JDiscoItemData : public QSharedData
{

public:
	JDiscoItemData() : expand(true) {}
	JDiscoItemData(const JDiscoItemData &o)
	    : QSharedData(o), name(o.name), jid(o.jid), node(o.node),
	      error(o.error), identities(o.identities), features(o.features),
	      actions(o.actions), expand(o.expand) {}
	QString name;
	QString jid;
	QString node;
	QString error;
	QList<Jreen::Disco::Identity> identities;
	QSet<QString> features;
	QList<JDiscoItem::Action> actions;
	bool expand;
};

JDiscoItem::JDiscoItem() : d(new JDiscoItemData)
{
}

JDiscoItem::JDiscoItem(const JDiscoItem &other) : d(other.d)
{
}

JDiscoItem::~JDiscoItem()
{
}

JDiscoItem &JDiscoItem::operator =(const JDiscoItem &other)
{
	d = other.d;
	return *this;
}

void JDiscoItem::setName(const QString &name)
{
	d->name = name;
}

void JDiscoItem::setJID(const QString &jid)
{
	d->jid = jid;
}

void JDiscoItem::setNode(const QString &node)
{
	d->node = node;
}

void JDiscoItem::setError(const QString &error)
{
	d->error = error;
}

void JDiscoItem::setExpandable(bool expand)
{
	d->expand = expand;
}

void JDiscoItem::addIdentity(const Jreen::Disco::Identity &identity)
{
	d->identities << identity;
}

bool JDiscoItem::hasIdentity(const QString &category, const QString &type) const
{
	foreach (const Jreen::Disco::Identity &identity, d->identities)
		if (identity.category == category && (type.isEmpty() || identity.type == type))
			return true;
	return false;
}

void JDiscoItem::addFeature(const QString &feature)
{
	d->features << feature;
}

bool JDiscoItem::hasFeature(const QString &feature) const
{
	return d->features.contains(feature);
}

void JDiscoItem::addAction(Action action)
{
	d->actions << action;
}

QString JDiscoItem::name() const
{
	return d->name;
}

QString JDiscoItem::jid() const
{
	return d->jid;
}

QString JDiscoItem::node() const
{
	return d->node;
}

QString JDiscoItem::error() const
{
	return d->error;
}

bool JDiscoItem::isExpandable() const
{
	return d->expand;
}

QList<Jreen::Disco::Identity> JDiscoItem::identities() const
{
	return d->identities;
}

QList<JDiscoItem::Action> JDiscoItem::actions() const
{
	return d->actions;
}

QSet<QString> JDiscoItem::features() const
{
	return d->features;
}
}
