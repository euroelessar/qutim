/****************************************************************************
 *  quickchatviewfactory.h
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

#ifndef QUICKCHATVIEWFACTORY_H
#define QUICKCHATVIEWFACTORY_H

#include <QWidget>
#include <chatlayer/chatviewfactory.h>

class QDeclarativeEngine;
namespace Core {
namespace AdiumChat {

class QuickChatViewFactory : public ChatViewFactory
{
    Q_OBJECT
public:
	QuickChatViewFactory();
	virtual QWidget *createViewWidget();
	virtual QObject *createViewController();
    virtual ~QuickChatViewFactory();
private:
	QDeclarativeEngine *m_engine;
};

} // namespace AdiumChat
} // namespace Core

#endif // QUICKCHATVIEWFACTORY_H
