#ifndef QUETZALACCOUNT_H
#define QUETZALACCOUNT_H

#include <qutim/account.h>
#include <purple.h>
#include "quetzalcontact.h"

using namespace qutim_sdk_0_3;

class QuetzalProtocol;

class QuetzalAccount : public Account
{
	Q_OBJECT
public:
	QuetzalAccount(const QString &id, QuetzalProtocol *protocol);
	virtual ChatUnit *getUnit(const QString &unitId, bool create = false);
	void createNode(PurpleBlistNode *node);
	void load(Config cfg);
	void save();
	void save(QuetzalContact *contact);
	void remove(QuetzalContact *contact);
signals:

public slots:
private:
	QHash<QString, QuetzalContact *> m_contacts;
	PurpleAccount *m_account;
	bool m_is_loading;
};

#endif // QUETZALACCOUNT_H
