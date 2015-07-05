/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef CORE_XSETTINGSWIDGET_H
#define CORE_XSETTINGSWIDGET_H

#include <qutim/settingswidget.h>
#include <qutim/settingslayer.h>
#include <QVBoxLayout>
#include <QSet>

namespace Core {

class XSettingsWidget : public qutim_sdk_0_3::SettingsWidget
{
	Q_OBJECT
public:
	explicit XSettingsWidget(QWidget *parent = 0);

	virtual void setController(QObject *controller);

	void addItem(qutim_sdk_0_3::SettingsItem *item);
	bool removeItem(qutim_sdk_0_3::SettingsItem *item);

protected:
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();

private slots:
	void onModifiedChanged(bool modified);

private:
	QObject *m_controller;
	qutim_sdk_0_3::SettingsItemList m_items;
	QList<SettingsWidget*> m_widgets;
	QSet<QObject*> m_changed;
	QVBoxLayout *m_layout;
};

} // namespace Core

#endif // CORE_XSETTINGSWIDGET_H
