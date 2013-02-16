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
#ifndef SIMPLECONTACTLISTSETTINGS_H
#define SIMPLECONTACTLISTSETTINGS_H

#include <QWidget>
#include <qutim/settingswidget.h>
#include "../contactdelegate.h"
#include <qutim/simplecontactlist/servicechooser.h>
#include <QPushButton>

class QCheckBox;

namespace Ui {
class SimpleContactlistSettings;
}

namespace Core
{

using namespace qutim_sdk_0_3;

class SimpleContactlistSettings : public ContactListSettingsExtention
{
    Q_OBJECT
    Q_INTERFACES(Core::ContactListSettingsExtention)
    Q_CLASSINFO("ServiceSettings", "Core::ContactDelegate")
public:
    explicit SimpleContactlistSettings();
    ~SimpleContactlistSettings();
    virtual void loadImpl();
    virtual void cancelImpl();
    virtual void saveImpl();

private:
    Ui::SimpleContactlistSettings *ui;
    void reloadCombobox();
    QHash<QString, QCheckBox *> m_statusesBoxes;
    QFont m_headerFont;
    QFont m_contactFont;
    void setButtonText(QFont font, QPushButton *button);
    void showFontDialog(QFont &font);

public slots:
    void headerFontSettings();
    void contactFontSettings();
};

}


#endif // SIMPLECONTACTLISTSETTINGS_H

