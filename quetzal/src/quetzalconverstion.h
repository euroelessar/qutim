/****************************************************************************
 *  quetzalconverstion.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef QUETZALCONVERSTION_H
#define QUETZALCONVERSTION_H

#include <qutim/chatunit.h>
#include <purple.h>

using namespace qutim_sdk_0_3;

class QuetzalConversation : public ChatUnit
{
	Q_OBJECT
public:
	explicit QuetzalConversation(PurpleConversation *conv);

	virtual QString id() const;
	virtual QString title() const;
	virtual void sendMessage(const Message &message);
	PurpleConversation *conv() { return m_conv; }
	virtual void update(PurpleConvUpdateType type);
protected:
	PurpleConversation *m_conv;
	QString m_id;
	QString m_title;
};

#endif // QUETZALCONVERSTION_H
