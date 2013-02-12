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

#include <QDialog>
#include <qutim/chatsession.h>
#include <QNetworkAccessManager>
#include <qutim/messagehandler.h>
#include <ui_handler.h>
#include <QTextDocumentFragment>
#include <qutim/message.h>

class Handler : public QDialog, public qutim_sdk_0_3::MessageHandler
{
	Q_OBJECT

public:
	explicit Handler(QWidget *parent = 0);
	~Handler();

private:
	Ui::Handler *ui;
	QTextDocumentFragment *m_fragment;
	qutim_sdk_0_3::Message m_message;
	QNetworkAccessManager *m_manager;
	QString m_link;
	void append_part(QHttpMultiPart *multi, const QByteArray &name, const QByteArray &value);
	bool m_showDialogEverytime;
	int m_lineCount;
	int m_defaultLocation;

public slots:
	virtual void accept();
	void finishedSlot(QNetworkReply *reply);
	void readSettings();
protected:
	virtual qutim_sdk_0_3::MessageHandler::Result doHandle(qutim_sdk_0_3::Message &message, QString *reason);

};

#endif // PASTER_H
