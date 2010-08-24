/****************************************************************************
 *  emoticonsselector.h
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
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
