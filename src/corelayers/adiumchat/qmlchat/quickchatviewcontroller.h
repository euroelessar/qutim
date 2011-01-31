/****************************************************************************
 *  quickchatviewcontroller.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef QUICKCHATVIEWCONTROLLER_H
#define QUICKCHATVIEWCONTROLLER_H

#include <chatviewfactory.h>
#include <QtDeclarative/QDeclarativeComponent>

namespace Core {
namespace AdiumChat {

class QuickChatViewController : public QDeclarativeComponent, public ChatViewController
{
    Q_OBJECT
public:
	virtual void setChatSession(ChatSessionImpl *session);
	virtual ChatSessionImpl *getSession() const;
	virtual void appendMessage(const qutim_sdk_0_3::Message &msg);
	virtual void clear();
    virtual ~QuickChatViewController();
};

} // namespace AdiumChat
} // namespace Core

#endif // QUICKCHATVIEWCONTROLLER_H
