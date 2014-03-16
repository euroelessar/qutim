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

#include "handler.h"
#include "autopasterdialog.h"
#include "ubuntupaster.h"
#include "hastebinpaster.h"
#include "kdepaster.h"

#include <QNetworkReply>
#include <QTimer>

#include <qutim/config.h>
#include <qutim/chatsession.h>
#include <qutim/json.h>

using namespace qutim_sdk_0_3;

static AutoPasterHandler *self = NULL;

AutoPasterHandler::AutoPasterHandler()
{
	Q_ASSERT(self == NULL);
	self = this;

	addPaster(new UbuntuPaster);
	addPaster(new HastebinPaster);
	addPaster(new KdePaster);

	readSettings();
}

AutoPasterHandler::~AutoPasterHandler()
{
	Q_ASSERT(self == this);
	self = NULL;

	qDeleteAll(m_pasters);
}

void AutoPasterHandler::addPaster(PasterInterface *paster)
{
	m_pasters << paster;
}

QList<PasterInterface *> AutoPasterHandler::pasters()
{
	return self ? self->m_pasters : QList<PasterInterface*>();
}

void AutoPasterHandler::doHandle(Message &message, const MessageHandler::Handler &handler)
{
	if (!message.isIncoming()
			&& !message.property("service", false)
			&& !message.property("history", false)
			&& message.text().count('\n') + 1 >= m_lineCount) {
        AutoPasterDialog *dialog = new AutoPasterDialog(&m_manager, message.text(), m_pasters, m_defaultLocation);
        QObject::connect(dialog, &QDialog::finished, [dialog, &message, handler] (int result) {
            dialog->deleteLater();

            switch (result) {
            case AutoPasterDialog::Accepted:
                message.setText(dialog->url().toString());
                break;
            case AutoPasterDialog::Rejected:
                break;
            case AutoPasterDialog::Failed:
                const QString reason = QCoreApplication::translate(
                              "AutoPaster",
                              "Failed to send message to paste service, service reported error: %1")
                          .arg(dialog->errorString());
                handler(Error, reason);
            }

            handler(Accept, QString());
        });

        if (m_autoSubmit)
            dialog->accept();
        else
            dialog->open();
	}
    handler(Accept, QString());
}

void AutoPasterHandler::readSettings()
{
	Config cfg;
	cfg.beginGroup("autoPaster");
	m_autoSubmit = cfg.value(QLatin1String("autoSubmit"), false);
	m_defaultLocation = cfg.value(QLatin1String("defaultLocation"), 0);
    m_lineCount = cfg.value(QLatin1String("lineCount"), 5);
}
