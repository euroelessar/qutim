#include "quetzalconverstion.h"
#include <qutim/message.h>

QuetzalConversation::QuetzalConversation(PurpleConversation *conv, Account *account) :
	ChatUnit(account)
{
	m_conv = conv;
	m_conv->ui_data = this;
	m_id = m_conv->name;
	m_title = m_conv->title;
}

QString QuetzalConversation::id() const
{
	return m_id;
}

QString QuetzalConversation::title() const
{
	return m_title;
}

void QuetzalConversation::sendMessage(const Message &message)
{
	if (m_conv->type == PURPLE_CONV_TYPE_IM) {
		purple_conv_im_send(m_conv->u.im, message.text().toUtf8().constData());
	} else {
		purple_conv_chat_send(m_conv->u.chat, message.text().toUtf8().constData());
	}
}
