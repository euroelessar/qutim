#ifndef QUETZALCONTACT_H
#define QUETZALCONTACT_H

#include <qutim/contact.h>
#include <purple.h>
#include <qutim/configbase.h>

using namespace qutim_sdk_0_3;

class QuetzalAccount;

class QuetzalContact : public Contact
{
	Q_OBJECT
public:
	explicit QuetzalContact(PurpleBuddy *buddy);

	void save(ConfigGroup group);
	void update();
	virtual QString id() const;
	virtual QString name() const;
	virtual QSet<QString> tags() const;
	virtual Status status() const;
//	virtual QIcon statusIcon() const;
	virtual void sendMessage(const Message &message);
	virtual void setName(const QString &name);
	virtual void setTags(const QSet<QString> &tags);
	virtual bool isInList() const;
	virtual void setInList(bool inList);

signals:

public slots:
private:
	Status m_status;
	QSet<QString> m_tags;
	QString m_id;
	QString m_name;
	PurpleBuddy *m_buddy;
	friend class QuetzalAccount;
};

#endif // QUETZALCONTACT_H
