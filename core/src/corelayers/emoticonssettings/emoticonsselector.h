/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef EMOTICONSSELECTOR_H
#define EMOTICONSSELECTOR_H
#include <qutim/settingswidget.h>
#include "emoticonssettings.h"

namespace Ui {
class emoticonsSelector;
}

using namespace qutim_sdk_0_3;

class EmoticonsSelector : public SettingsWidget
{
	Q_OBJECT
public:
	EmoticonsSelector();
    virtual void loadImpl();
    virtual void cancelImpl();
    virtual void saveImpl();	
    virtual ~EmoticonsSelector();
private slots:
	void currentIndexChanged(const QString &text);
	void clearEmoticonsPreview();
private:
	Ui::emoticonsSelector *ui;
	QWidgetList m_active_emoticons;
	QString m_selected_theme;
};

#endif // EMOTICONSSELECTOR_H

