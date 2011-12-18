/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef HISTORYMANAGERWINDOW_H
#define HISTORYMANAGERWINDOW_H

#include <QWizard>
#include <QMap>
#include <QHash>
#include <QEvent>
#include "clients/qutim.h"
#include "../include/qutim/historymanager.h"

namespace HistoryManager {

class HistoryManagerWindow : public QWizard, public DataBaseInterface {
    Q_OBJECT
    Q_DISABLE_COPY(HistoryManagerWindow)
public:
	enum State
	{
		ChooseClient,
		ConfigClient,
		ImportHistory,
		ChooseOrDump,
		PreviewHistory,
		ExportHistory
	};
    explicit HistoryManagerWindow(QWidget *parent = 0);
    virtual ~HistoryManagerWindow();
	virtual void appendMessage(const qutim_sdk_0_3::Message &message);
	virtual void setProtocol(const QString &protocol);
	virtual void setAccount(const QString &account);
	virtual void setContact(const QString &contact);
	virtual void setMaxValue(int max);
	virtual void setValue(int value);
	virtual ConfigWidget createAccountWidget(const QString &protocol);
	inline void setCurrentClient(HistoryImporter *client) { m_current_client = client; }
	inline HistoryImporter *getCurrentClient() const { return m_current_client; }
	inline qutim *getQutIM() const { return m_qutim; }
	inline quint64 getMessageNum() const { return m_message_num; }
	void saveMessages(char format);
	QString finishStr() { if(m_finish.isEmpty()) m_finish = buttonText(QWizard::FinishButton); return m_finish; }
	QString nextStr() { if(m_next.isEmpty()) m_next = buttonText(QWizard::NextButton); return m_next; }
	QString dumpStr() { return m_dump; }
	void setCharset(const QByteArray &charset) { m_charset = charset; }
	QByteArray charset() const { return m_charset; }

protected:
	virtual void changeEvent(QEvent *e);

signals:
	void maxValueChanged(int value);
	void valueChanged(int value);
	void saveMaxValueChanged(int value);
	void saveValueChanged(int value);

private:
	QHash<QString, Protocol> m_protocols;
	Protocol *m_protocol;
	Account  *m_account;
	Contact  *m_contact;
	quint64 m_message_num;
	HistoryImporter *m_current_client;
	qutim *m_qutim;
	QString m_finish;
	QString m_next;
	QString m_dump;
	QByteArray m_charset;
	bool m_is_dumping;
};

}

#endif // HISTORYMANAGERWINDOW_H

