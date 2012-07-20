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
#ifndef OLDDELEGATESETTINGS_H
#define OLDDELEGATESETTINGS_H

#include <QWidget>
#include <qutim/settingswidget.h>
#include <contactlistitemdelegate.h>
#include <qutim/simplecontactlist/servicechooser.h>
#include <qutim/config.h>

class QCheckBox;
class QComboBox;

namespace Ui {
	class OldDelegateSettings;
}

using namespace qutim_sdk_0_3;

class OldDelegateSettings : public Core::ContactListSettingsExtention
{
    Q_OBJECT
	Q_INTERFACES(Core::ContactListSettingsExtention)
	Q_CLASSINFO("ServiceSettings", "ContactListItemDelegate")
public:
	explicit OldDelegateSettings();
	~OldDelegateSettings();
    virtual void loadImpl();
    virtual void cancelImpl();
    virtual void saveImpl();
private:
	Ui::OldDelegateSettings *ui;
	void initCombobox(QComboBox *box);
	void setSize(QComboBox *box, int size);
	void storeSizeToConfig(QComboBox *box, Config &cfg, const char *value);
	QHash<QString, QCheckBox *> m_statusesBoxes;
};

#endif // SIMPLECONTACTLISTSETTINGS_H

