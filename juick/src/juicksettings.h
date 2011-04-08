/*

    Copyright (c) 2011 by Stanislav (proDOOMman) Kosolapov <prodoomman@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef JUICKSETTINGS_H
#define JUICKSETTINGS_H

#include <QtGui/QWidget>
#include <QAbstractButton>
#include <qutim/settingswidget.h>
#include "qtcolorcombobox.h"

namespace Ui {
    class JuickSettings;
}

using namespace qutim_sdk_0_3;

class JuickSettings : public SettingsWidget
{
    Q_OBJECT

    QtColorComboBox *user_color_box;
    QtColorComboBox *post_color_box;

public:
    explicit JuickSettings();
    ~JuickSettings();

    virtual void loadImpl();
    virtual void saveImpl();
    virtual void cancelImpl();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::JuickSettings *ui;
};

#endif // JUICKSETTINGS_H
