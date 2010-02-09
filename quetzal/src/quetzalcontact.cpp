#include "quetzalcontact.h"
#include "quetzalaccount.h"
#include <qutim/debug.h>

Status quetzal_get_status(PurpleBuddy *buddy)
{
	PurplePresence *presence = purple_buddy_get_presence(buddy);
	PurpleStatus *status = purple_presence_get_active_status(presence);
	PurpleStatusType *status_type = purple_status_get_type(status);
	PurpleStatusPrimitive primitive = purple_status_type_get_primitive(status_type);
	switch (primitive) {
	case PURPLE_STATUS_UNSET:
		return Offline;
	case PURPLE_STATUS_OFFLINE:
		return Offline;
	case PURPLE_STATUS_AVAILABLE:
		return Online;
	case PURPLE_STATUS_UNAVAILABLE:
		return NA;
	case PURPLE_STATUS_INVISIBLE:
		return Invisible;
	case PURPLE_STATUS_AWAY:
		return Away;
	case PURPLE_STATUS_EXTENDED_AWAY:
		return Away;
	case PURPLE_STATUS_MOBILE:
		return Online;
	case PURPLE_STATUS_TUNE:
	default:
		return Online;
	}
}

QuetzalContact::QuetzalContact(PurpleBuddy *buddy) :
	Contact(reinterpret_cast<QuetzalAccount *>(buddy->account->ui_data))
{
	m_buddy = buddy;
	buddy->node.ui_data = this;
	PurpleBlistNode *node = &buddy->node;
	m_id = buddy->name;
	m_name = buddy->alias;
	m_status = quetzal_get_status(m_buddy);
	while (node = node->parent) {
		if (PURPLE_BLIST_NODE_IS_GROUP(node)) {
			PurpleGroup *group = PURPLE_GROUP(node);
			m_tags.insert(group->name);
		}
	}
}

void QuetzalContact::save(ConfigGroup group)
{
	group.setValue("name", m_name);
	group.setValue("tags", QStringList(m_tags.toList()));
}

void QuetzalContact::update()
{
	QSet<QString> tags_;
	PurpleBlistNode *node = &m_buddy->node;
	while (node = node->parent) {
		if (PURPLE_BLIST_NODE_IS_GROUP(node)) {
			PurpleGroup *group = PURPLE_GROUP(node);
			tags_.insert(group->name);
		}
	}
	if (m_tags != tags_) {
		m_tags = tags_;
		tagsChanged(m_tags);
	}
	QString name = m_buddy->alias;
	if (name != m_name) {
		m_name = name;
		emit nameChanged(m_name);
	}
	Status status = quetzal_get_status(m_buddy);
	if (m_status != status) {
		debug() << Q_FUNC_INFO << m_id << status;
		m_status = status;
		emit statusChanged(m_status);
		emit statusIconChanged(statusIcon());
	}
}

QString QuetzalContact::id() const
{
	return m_id;
}

QString QuetzalContact::name() const
{
	return m_name;
}

QSet<QString> QuetzalContact::tags() const
{
	return m_tags;
}

Status QuetzalContact::status() const
{
	return m_status;
}

void QuetzalContact::sendMessage(const Message &message)
{
}

void QuetzalContact::setName(const QString &name)
{
}

void QuetzalContact::setTags(const QSet<QString> &tags)
{
}

bool QuetzalContact::isInList() const
{
}

void QuetzalContact::setInList(bool inList)
{
}
