#include "quetzalaccount.h"
#include "quetzalprotocol.h"
#include <qutim/debug.h>
#include <qutim/contactlist.h>
#include <QFile>
#include <qutim/json.h>

QuetzalAccount::QuetzalAccount(const QString &id, QuetzalProtocol *protocol) :
	Account(id, protocol)
{
	m_account = purple_account_new(id.toUtf8(), protocol->plugin()->info->id);
	m_account->ui_data = this;
	debug() << "created!" << this << m_account->protocol_id;


	Config cfg = config();
	QByteArray password = cfg.group("general").value("passwd", QString(), Config::Crypted).toUtf8();
	m_account->password = reinterpret_cast<char *>(qMalloc(password.size() + 1));
	qstrcpy(m_account->password, password);
	m_is_loading = true;
	load(cfg);
	m_is_loading = false;
	if (protocol->id() == "jabber" && id.contains("qutim.org")) {
		purple_account_set_enabled(m_account, "qutim", TRUE);
		purple_account_connect(m_account);
	} else
		purple_account_set_enabled(m_account, "qutim", FALSE);
}

ChatUnit *QuetzalAccount::getUnit(const QString &unitId, bool create)
{
	return 0;
}

void QuetzalAccount::createNode(PurpleBlistNode *node)
{
	if (m_is_loading)
		return;
	if (PURPLE_BLIST_NODE_IS_BUDDY(node)) {
		PurpleBuddy *buddy = PURPLE_BUDDY(node);
		if (m_contacts.contains(buddy->name))
			return;
		QuetzalContact *contact = new QuetzalContact(buddy);
		m_contacts.insert(contact->id(), contact);
		ContactList::instance()->addContact(contact);
	}
}

void QuetzalAccount::load(Config cfg)
{
	ConfigGroup group = cfg.group("contactlist");
	foreach (const QString &id, group.groupList()) {
		ConfigGroup contact = group.group(id);
		QString name = contact.value("name", QString());
		PurpleContact *pc = purple_contact_new();
		PurpleBuddy *buddy = purple_buddy_new(m_account,
											  id.toUtf8().constData(),
											  name.toUtf8().constData());
		buddy->node.parent = PURPLE_BLIST_NODE(pc);
		QuetzalContact *qc = new QuetzalContact(buddy);
		qc->m_tags = contact.value("tags", QStringList()).toSet();
		QByteArray tag;
		if (qc->m_tags.isEmpty()) {
			tag = tr("General").toUtf8();
		} else {
			tag = qc->m_tags.begin()->toUtf8();
		}
		PurpleGroup *group = purple_group_new(tag.constData());
		buddy->node.parent->parent = PURPLE_BLIST_NODE(group);
		ContactList::instance()->addContact(qc);
	}
}

void QuetzalAccount::save()
{
	ConfigGroup group = config("contactlist");
	QHash<QString, QuetzalContact *>::iterator it = m_contacts.begin();
	for (; it != m_contacts.end(); it++) {
		it.value()->save(group.group(it.value()->id()));
	}
	group.sync();
}

void QuetzalAccount::save(QuetzalContact *contact)
{
	ConfigGroup group = config("contactlist");
	contact->save(group.group(contact->id()));
	group.sync();
}

void QuetzalAccount::remove(QuetzalContact *contact)
{
	ConfigGroup group = config("contactlist");
	group.removeGroup(contact->id());
	group.sync();
	m_contacts.remove(contact->id());
	ContactList::instance()->removeContact(contact);
	contact->deleteLater();
}
