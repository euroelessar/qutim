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

#include "pasterinterface.h"

#include <qutim/chatsession.h>
#include <qutim/messagehandler.h>
#include <qutim/message.h>
#include <qutim/settingslayer.h>
#include <qutim/quickdialog.h>

#include <QQueue>
#include <QNetworkAccessManager>
#include <QTextDocumentFragment>

class AutoPasterHandler : public QObject, public qutim_sdk_0_3::MessageHandler
{
	Q_OBJECT
	Q_PROPERTY(QStringList pasters READ pasters CONSTANT)
	Q_PROPERTY(QVariantList syntaxes READ syntaxes CONSTANT)
	Q_PROPERTY(int currentPasterIndex READ currentPasterIndex NOTIFY currentPasterIndexChanged)
public:
	explicit AutoPasterHandler();
	~AutoPasterHandler();

	void addPaster(PasterInterface *paster);

	int currentPasterIndex();

	QStringList pasters();
	QVariantList syntaxes();

public slots:
	void readSettings();
	void upload(const QString &paster, const QString &syntax);
	void cancel();

protected:
	qutim_sdk_0_3::MessageHandlerAsyncResult doHandle(qutim_sdk_0_3::Message &message) override;

signals:
	void messageReceived();
	void currentPasterIndexChanged(int currentPasterIndex);

private:
	struct QueueItem
	{
		qutim_sdk_0_3::MessageHandlerAsyncResult::Handler handler;
		qutim_sdk_0_3::Message *message;
	};

	void upload(QueueItem item, PasterInterface *paster, const QString &syntax);

	QNetworkAccessManager m_manager;
	qutim_sdk_0_3::QuickDialog m_dialog;
	qutim_sdk_0_3::QmlSettingsItem m_settings;
	QList<PasterInterface*> m_pasters;
	QQueue<QueueItem> m_queue;

	bool m_autoSubmit;
	int m_lineCount;
	int m_defaultLocation;
};

#endif // PASTER_H
