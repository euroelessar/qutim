/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2013 Tretyakov Roman <roman@trett.ru>
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
#ifndef PASTER_H
#define PASTER_H

#include <ui_handler.h>
#include "pasterinterface.h"

#include <qutim/chatsession.h>
#include <qutim/messagehandler.h>
#include <qutim/message.h>

#include <QDialog>
#include <QNetworkAccessManager>
#include <QTextDocumentFragment>

class AutoPasterHandler : public qutim_sdk_0_3::MessageHandler
{
public:
	explicit AutoPasterHandler();
	~AutoPasterHandler();

	void addPaster(PasterInterface *paster);

	static QList<PasterInterface*> pasters();

public slots:
	void readSettings();

protected:
	virtual qutim_sdk_0_3::MessageHandler::Result doHandle(qutim_sdk_0_3::Message &message, QString *reason);

private:
	QNetworkAccessManager m_manager;
	QList<PasterInterface*> m_pasters;

	bool m_autoSubmit;
	int m_lineCount;
	int m_defaultLocation;
	int m_delay;
};

#endif // PASTER_H
