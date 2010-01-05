#include "jcontactresource_p.h"
#include "jcontact.h"
#include "../jaccount.h"
#include <QStringBuilder>

using namespace gloox;
using namespace qutim_sdk_0_3;

namespace Jabber
{
	JContactResource::JContactResource(ChatUnit *parent, const QString &name) :
			ChatUnit(parent->account()), d_ptr(new JContactResourcePrivate)
	{
		Q_D(JContactResource);
		d->name = name;
		d->id = parent->id() % QLatin1Char('/') % name;
	}

	JContactResource::JContactResource(ChatUnit *parent, JContactResourcePrivate &ptr) :
			ChatUnit(parent->account()), d_ptr(&ptr)
	{
	}

	JContactResource::~JContactResource()
	{
	}

	QString JContactResource::id() const
	{
		return d_func()->id;
	}

	QString JContactResource::title() const
	{
		Q_D(const JContactResource);
		if (Contact *contact = qobject_cast<Contact *>(d->contact)) {
			return contact->title() % "/" % d->name;
		} else {
			return ChatUnit::title();
		}
	}

	void JContactResource::sendMessage(const qutim_sdk_0_3::Message &message)
	{
	}

	void JContactResource::setPriority(int priority)
	{
		d_func()->priority = priority;
	}

	int JContactResource::priority()
	{
		return d_func()->priority;
	}

	void JContactResource::setStatus(Presence::PresenceType presence, int priority)
	{
		Q_D(JContactResource);
		d->presence = presence;
		d->priority = priority;
	}

	Presence::PresenceType JContactResource::status()
	{
		return d_func()->presence;
	}

	void JContactResource::setChatState(ChatState state)
	{
	}

	QSet<QString> JContactResource::features() const
	{
		return d_ptr->features;
	}

	void JContactResource::setFeatures(const QSet<QString> &features)
	{
		d_ptr->features = features;
	}

	bool JContactResource::checkFeature(const QLatin1String &feature) const
	{
		d_ptr->features.contains(feature);
	}

	bool JContactResource::checkFeature(const QString &feature) const
	{
		d_ptr->features.contains(feature);
	}

	bool JContactResource::checkFeature(const std::string &feature) const
	{
		d_ptr->features.contains(QString::fromStdString(feature));
	}
}
