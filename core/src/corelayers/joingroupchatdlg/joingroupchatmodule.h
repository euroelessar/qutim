/****************************************************************************
 *  joingroupchatmodule.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef JOINGROUPCHATMODULE_H
#define JOINGROUPCHATMODULE_H
#include <QObject>
#include <qutim/actiongenerator.h>

namespace Core
{

class JoinGroupChat;
using namespace qutim_sdk_0_3;

class JoinGroupChatModule : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("Service", "JoinGroupChat")
    Q_CLASSINFO("Uses", "IconLoader")
    Q_CLASSINFO("Uses", "ContactList")
public:
    explicit JoinGroupChatModule();
    virtual ~JoinGroupChatModule();
public slots:
    void onJoinGroupChatTriggered();
private:
    QWeakPointer<JoinGroupChat> m_chat;
    QScopedPointer<ActionGenerator> m_gen;
};

class JoinGroupChatGenerator : public ActionGenerator
{
public:
    JoinGroupChatGenerator(QObject *module);
    void showImpl(QAction *action, QObject *obj);
};

}
#endif // JOINGROUPCHATMODULE_H
