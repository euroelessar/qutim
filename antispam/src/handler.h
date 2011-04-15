/****************************************************************************
 *  handler.h
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

#ifndef ANTISPAM_HANDLER_H
#define ANTISPAM_HANDLER_H
#include <qutim/messagehandler.h>
#include <QStringList>

namespace Antispam {

class Handler : public QObject, public qutim_sdk_0_3::MessageHandler
{
	Q_OBJECT
public:
    explicit Handler();
public slots:
	void loadSettings();
protected:
	bool eventFilter(QObject *obj, QEvent *event);
    virtual Result doHandle(qutim_sdk_0_3::Message& message, QString* reason);
private:
	bool m_enabled;
	QString m_question;
	QString m_success;
	QStringList m_answers;
};

} // namespace Antispam

#endif // ANTISPAM_HANDLER_H
