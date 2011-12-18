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
#ifndef ICQACCOUNTMAINSETTINGS_H
#define ICQACCOUNTMAINSETTINGS_H

#include <QWidget>
#include <qutim/settingswidget.h>

namespace Ui {
    class IcqAccountMainSettings;
}

class QVBoxLayout;

namespace qutim_sdk_0_3 {

namespace oscar {

class IcqAccount;

class IcqAccountMainSettings : public QWidget {
    Q_OBJECT
public:
	IcqAccountMainSettings(IcqAccount *account = 0, QWidget *parent = 0);
    ~IcqAccountMainSettings();
	void reloadSettings();
	void saveSettings();
	bool isComplete();
signals:
	void completeChanged();
protected:
    void changeEvent(QEvent *e);
	const QStringList &defaultServers() const;
	const QHash<QString, QString> &defaultSslServers() const;
private slots:
	void onSslChecked(bool checked);
	void onCurrentServerChanged(const QString &currentServer);
	void updatePort(bool ssl);
private:
    Ui::IcqAccountMainSettings *ui;
	IcqAccount *m_account;
};

class IcqAccountMainSettingsWidget: public SettingsWidget
{
	Q_OBJECT
public:
	IcqAccountMainSettingsWidget();
	virtual ~IcqAccountMainSettingsWidget();
	virtual void loadImpl();
	virtual void cancelImpl();
	virtual void saveImpl();
	virtual void setController(QObject *controller);
private:
	IcqAccountMainSettings *m_widget;
	QVBoxLayout *m_layout;
	IcqAccount *m_account;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // ICQACCOUNTMAINSETTINGS_H

