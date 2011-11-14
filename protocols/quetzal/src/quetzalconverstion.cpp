/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "quetzalconverstion.h"
#include <qutim/message.h>

QuetzalConversation::QuetzalConversation(PurpleConversation *conv) :
	ChatUnit(reinterpret_cast<Account *>(conv->account->ui_data))
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

struct QuetzalScopedPointerDeleter
{
	static inline void cleanup(void *pointer) { if (pointer) g_free(pointer); }
};

bool QuetzalConversation::sendMessage(const Message &message)
{
	if (m_conv->type == PURPLE_CONV_TYPE_IM) {
		purple_conv_im_send(m_conv->u.im, message.text().toUtf8().constData());
	} else {
//		QByteArray plain_text = message.text().toUtf8();
//		if (message.text().startsWith("/") && !message.text().startsWith("/me ")) {
//			QScopedPointer<gchar, QuetzalScopedPointerDeleter> error;
//			QString html = message.property("html").toString();
//			QScopedPointer<char, QuetzalScopedPointerDeleter> markup(
//					html.isEmpty() ? purple_markup_escape_text(plain_text.constData(), plain_text.size()) : NULL);
//			if (PURPLE_CMD_STATUS_OK == purple_cmd_do_command(m_conv, plain_text.constData(),
//															  html.isEmpty() ? markup.data() : html.toUtf8().constData(),
//															  reinterpret_cast<gchar **>(&error))) {
//				return;
//			}
//		}
		purple_conv_chat_send(m_conv->u.chat, message.text().toUtf8().constData());
	}
	return true;
}

void QuetzalConversation::update(PurpleConvUpdateType type)
{
	Q_UNUSED(type);
}

