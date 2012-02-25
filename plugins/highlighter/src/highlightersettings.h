/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Nicolay Izoderov <nico-izo@ya.ru>
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

#ifndef HIGHLIGHTERSETTINGS_H
#define HIGHLIGHTERSETTINGS_H

#include <qutim/settingswidget.h>
#include "highlighteritemlist.h"
#include <QComboBox>
#include <QEvent>
#include "ui_highlightersettings.h"

class HighlighterSettings : public qutim_sdk_0_3::SettingsWidget
{
	Q_OBJECT
public:
	HighlighterSettings();
	~HighlighterSettings();
protected:
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();
	virtual void changeEvent(QEvent *e);

private slots:
	void onRemoveButtonClicked();
	void onAddButtonClicked();
	void validateInputRegexp();

signals:
	void reloadSettings();
private:
	Ui::HighlighterSettingsForm ui;
	QList<HighlighterItemList*> m_items;
};
#endif // HIGHLIGHTERSETTINGS_H

