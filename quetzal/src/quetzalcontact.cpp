#include "quetzalcontact.h"
#include "quetzalaccount.h"
#include <qutim/debug.h>

QVariant quetzal_value2variant(const PurpleValue *value)
{
	switch (purple_value_get_type(value)) {
	default:
	case PURPLE_TYPE_UNKNOWN:
		return QVariant(QVariant::Invalid);
	case PURPLE_TYPE_SUBTYPE:
		return QVariant(QVariant::Invalid);
	case PURPLE_TYPE_CHAR:
		return QVariant(value->data.char_data);
	case PURPLE_TYPE_UCHAR:
		return QVariant(value->data.uchar_data);
	case PURPLE_TYPE_BOOLEAN:
		return QVariant(bool(value->data.boolean_data));
	case PURPLE_TYPE_SHORT:
		return QVariant(value->data.short_data);
	case PURPLE_TYPE_USHORT:
		return QVariant(value->data.ushort_data);
	case PURPLE_TYPE_INT:
		return QVariant(value->data.int_data);
	case PURPLE_TYPE_UINT:
		return QVariant(value->data.uint_data);
	case PURPLE_TYPE_LONG:
		if (sizeof(long) == sizeof(int))
			return QVariant(int(value->data.long_data));
		else
			return QVariant(qlonglong(value->data.long_data));
	case PURPLE_TYPE_ULONG:
		if (sizeof(ulong) == sizeof(uint))
			return QVariant(uint(value->data.ulong_data));
		else
			return QVariant(qulonglong(value->data.ulong_data));
	case PURPLE_TYPE_INT64:
		return QVariant(value->data.int64_data);
	case PURPLE_TYPE_UINT64:
		return QVariant(value->data.uint64_data);
	case PURPLE_TYPE_STRING:
		return QVariant(QString::fromUtf8(value->data.string_data));
	case PURPLE_TYPE_OBJECT:
	case PURPLE_TYPE_POINTER:
		return QVariant(QVariant::Invalid);
	case PURPLE_TYPE_ENUM:
		return QVariant(value->data.enum_data);
	case PURPLE_TYPE_BOXED:
		return QVariant(QVariant::Invalid);
	}
}

Status quetzal_get_status(PurpleBuddy *buddy)
{
	PurplePresence *presence = purple_buddy_get_presence(buddy);
	PurpleStatus *status = purple_presence_get_active_status(presence);
	const char *status_id = purple_status_get_id(status);
	PurpleStatusType *status_type = purple_status_get_type(status);
	PurpleStatusPrimitive primitive = purple_status_type_get_primitive(status_type);
	Status::Type type;
	switch (primitive) {
	case PURPLE_STATUS_UNSET:
		type = Status::Offline;
		break;
	case PURPLE_STATUS_OFFLINE:
		type = Status::Offline;
		break;
	case PURPLE_STATUS_AVAILABLE:
		if (g_str_equal(status_id, "freeforchat")
			|| g_str_equal(status_id, "free4chat"))
			type = Status::FreeChat;
		else
			type = Status::Online;
		break;
	case PURPLE_STATUS_UNAVAILABLE:
		if (g_str_equal(status_id, "dnd"))
			type = Status::DND;
//		else if (g_str_equal(status_id, "occupied"))
//			type = Status::
		else
			type = Status::NA;
		break;
	case PURPLE_STATUS_INVISIBLE:
		type = Status::Invisible;
		break;
	case PURPLE_STATUS_AWAY:
		type = Status::Away;
		break;
	case PURPLE_STATUS_EXTENDED_AWAY:
		type = Status::Away;
		break;
	case PURPLE_STATUS_MOBILE:
	case PURPLE_STATUS_TUNE:
	default:
		type = Status::Online;
		break;
	}
	Status qStatus(type);
	PurplePlugin *proto = purple_plugins_find_with_id(buddy->account->protocol_id);
	if (proto) {
		qStatus.initIcon(QString::fromUtf8(proto->info->name).toLower());
	} else {
		qStatus.initIcon();
	}
	for (GList *it = purple_status_type_get_attrs(status_type); it; it = it->next) {
		PurpleStatusAttr *attr = (PurpleStatusAttr *)it->data;
		const char *id = purple_status_attr_get_id(attr);
//		QString name = purple_status_attr_get_name(attr);
		QVariant value = quetzal_value2variant(purple_status_attr_get_value(attr));
		debug() << "!" << type << id /*<< name */<< value;
		if (g_str_equal(id, "message"))
			qStatus.setText(value.toString());
		else
			qStatus.setProperty(id, value);
	}
	return qStatus;
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
	if (m_status.subtype() != status.subtype() || m_status.text() != status.text()) {
		debug() << Q_FUNC_INFO << m_id << status;
		m_status = status;
		emit statusChanged(m_status);
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
