#include "jcontactresource_p.h"
#include "jcontact.h"
#include "../vcard/jinforequest.h"
#include "../jaccount.h"
#include "../../jprotocol.h"
#include "jmessagesession.h"
#include "jmessagehandler.h"
#include <qutim/status.h>
#include <qutim/inforequest.h>
#include <qutim/tooltip.h>
#include <QStringBuilder>
#include <jreen/chatstate.h>
#include <jreen/message.h>
#include <jreen/client.h>

using namespace qutim_sdk_0_3;

namespace Jabber
{

JContactResource::JContactResource(ChatUnit *parent, const QString &name) :
	Contact(parent->account()), d_ptr(new JContactResourcePrivate(parent))
{
	Q_D(JContactResource);
	d->name = name;
	d->id = parent->id() % QLatin1Char('/') % name;
	connect(parent, SIGNAL(avatarChanged(QString)), this, SIGNAL(avatarChanged(QString)));
}

JContactResource::JContactResource(ChatUnit *parent, JContactResourcePrivate &ptr) :
	Contact(parent->account()), d_ptr(&ptr)
{
}

JContactResource::JContactResource(JAccount *parent, const QString &resource) :
	Contact(parent), d_ptr(new JContactResourcePrivate(parent))
{
	Q_D(JContactResource);
	d->name = resource;
	d->id = parent->id() % QLatin1Char('/') % resource;
	connect(parent, SIGNAL(avatarChanged(QString)), this, SIGNAL(avatarChanged(QString)));
}

JContactResource::~JContactResource()
{
}

QString JContactResource::name() const
{
	return d_func()->name;
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
		return Buddy::title();
	}
}

bool JContactResource::sendMessage(const qutim_sdk_0_3::Message &message)
{
	JAccount *a = static_cast<JAccount*>(account());

	if(a->status() == Status::Offline || a->status() == Status::Connecting)
		return false;
	qDebug("%s", Q_FUNC_INFO);

	a->messageSessionManager()->sendMessage(this, message);
	return true;
}

void JContactResource::setPriority(int priority)
{
	d_func()->presence.setPriority(priority);
}

int JContactResource::priority()
{
	return d_func()->presence.priority();
}

void JContactResource::setStatus(const Jreen::Presence presence)
{
	Q_D(JContactResource);
	Status current = status();
	d->presence = presence;
	emit statusChanged(status(), current);
}

Status JContactResource::status() const
{
	Q_D(const JContactResource);
	Status status = JStatus::presenceToStatus(d->presence.subtype());
	status.setText(d->presence.status());
	status.setExtendedInfos(d->extInfo);
	return status;
}

bool JContactResource::event(QEvent *ev)
{
	Q_D(JContactResource);
	if (ev->type() == ChatStateEvent::eventType()) {
		ChatStateEvent *chatEvent = static_cast<ChatStateEvent *>(ev);
		Jreen::ChatState::State state = static_cast<Jreen::ChatState::State>(chatEvent->chatState());

		Jreen::Message msg(Jreen::Message::Chat, d->id);
		msg.addExtension(new Jreen::ChatState(state));
		JAccount *account = d->isAccountResource ? d->account : static_cast<JAccount*>(d->contact->account());
		account->messageSessionManager()->send(msg);
		return true;
	} else if (ev->type() == ToolTipEvent::eventType()) {
		ToolTipEvent *event = static_cast<ToolTipEvent*>(ev);
		event->addField(QT_TRANSLATE_NOOP("ContactResource", "Resource"),
						QString("%1 (%2)").arg(id()).arg(priority()), 75);
		if (!text().isEmpty())
			event->addField(text(), QString());
		event->addHtml("<font size=-1>", 50);
		QString client = property("client").toString();
		if (!client.isEmpty()) {
			event->addField(QT_TRANSLATE_NOOP("ContactResource", "Possible client"),
							client,
							property("clientIcon").toString(),
							ToolTipEvent::IconBeforeDescription,
							25);
			QString os = property("os").toString();
			if (!os.isEmpty())
				event->addField(QT_TRANSLATE_NOOP("ContactResource", "OS"), os, 25);
		}
		event->addHtml("</font>", 10);
		return true;
	}
	return Buddy::event(ev);
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
	return d_ptr->features.contains(feature);
}

bool JContactResource::checkFeature(const QString &feature) const
{
	return d_ptr->features.contains(feature);
}

bool JContactResource::checkFeature(const std::string &feature) const
{
	return d_ptr->features.contains(QString::fromStdString(feature));
}

ChatUnit *JContactResource::upperUnit()
{
	Q_D(JContactResource);
	return d->isAccountResource ? 0 : qobject_cast<Contact *>(d->contact);
}

QString JContactResource::avatar() const
{
	Q_D(const JContactResource);
	if (d->isAccountResource)
		return d->account->property("avatar").toString();
	else if (Buddy *buddy = qobject_cast<Buddy*>(d->contact))
		return buddy->avatar();
	return QString();
}

QString JContactResource::text() const
{
	return d_func()->presence.status();
}

void JContactResource::setExtendedInfo(const QString &name, const QVariantHash &info)
{
	Q_D(JContactResource);
	Status current = status();
	d->extInfo.insert(name, info);
	emit statusChanged(status(), current);
}

void JContactResource::removeExtendedInfo(const QString &name)
{
	Q_D(JContactResource);
	Status current = status();
	d->extInfo.remove(name);
	emit statusChanged(status(), current);
}

QStringList JContactResource::tags() const
{
	QStringList tags;
	if (d_func()->isAccountResource)
		tags << tr("My resources");
	return tags;
}

void JContactResource::setTags(const QStringList &tags)
{
	Q_UNUSED(tags);
}

bool JContactResource::isInList() const
{
	return d_func()->isAccountResource;
}

void JContactResource::setInList(bool inList)
{
	Q_UNUSED(inList);
}

}
