#ifndef QUETZALCONVERSTION_H
#define QUETZALCONVERSTION_H

#include <qutim/chatunit.h>
#include <purple.h>

using namespace qutim_sdk_0_3;

class QuetzalConversation : public ChatUnit
{
	Q_OBJECT
public:
	explicit QuetzalConversation(PurpleConversation *conv, Account *account);

	virtual QString id() const;
	virtual QString title() const;
	virtual void sendMessage(const Message &message);
	PurpleConversation *conv() { return m_conv; }
private:
	PurpleConversation *m_conv;
	QString m_id;
	QString m_title;
};

#endif // QUETZALCONVERSTION_H
