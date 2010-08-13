/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef SCRIPTMESSAGEPROPERTYITERATOR_H
#define SCRIPTMESSAGEPROPERTYITERATOR_H

#include <QScriptClassPropertyIterator>
#include <qutim/message.h>

class ScriptMessagePropertyIterator : public QScriptClassPropertyIterator
{
public:
    ScriptMessagePropertyIterator(const QScriptValue &object, qutim_sdk_0_3::Message *msg);
	
    virtual bool hasNext() const;
    virtual void next();

    virtual bool hasPrevious() const;
    virtual void previous();

    virtual void toFront();
    virtual void toBack();

    virtual QScriptString name() const;
private:
	qutim_sdk_0_3::Message *m_msg;
	int m_id;
};

#endif // SCRIPTMESSAGEPROPERTYITERATOR_H
