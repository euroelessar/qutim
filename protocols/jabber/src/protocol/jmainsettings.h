/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Denis Daschenko <daschenko@gmail.com>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef JABBERSETTINGS_H
#define JABBERSETTINGS_H

#include <qutim/settingswidget.h>
#include <qutim/configbase.h>
#include <QPointer>
#include <QtCrypto>

namespace Ui
{
class JMainSettings;
}

namespace Jabber
{
using namespace qutim_sdk_0_3;
class JAccount;
class JMainSettings: public SettingsWidget
{
	Q_OBJECT
public:
	JMainSettings();
	~JMainSettings();
	void loadImpl();
	void cancelImpl();
	void saveImpl();
	virtual void setController(QObject *controller);

	void updatePGPText();

private slots:
	void on_selectPGPButton_clicked();
	void on_removePGPButton_clicked();
	void onPGPKeyDialogFinished(int result);

private:
	Ui::JMainSettings *ui;
	QCA::KeyStoreEntry m_keyEntry;
	QPointer<JAccount> m_account;
};
}

#endif // JABBERSETTINGS_H

