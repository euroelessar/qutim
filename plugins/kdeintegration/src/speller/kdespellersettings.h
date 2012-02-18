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

#ifndef KDESPELLERSETTINGS_H
#define KDESPELLERSETTINGS_H

#include <QtGui/QWidget>
#include <sonnet/speller.h>
#include <qutim/settingswidget.h>
#include <qutim/settingslayer.h>
#include <KIcon>

using namespace Sonnet;
using namespace qutim_sdk_0_3;

namespace Ui {
    class KdeSpellerSettings;
}

class KdeSpellerSettings : public SettingsWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(KdeSpellerSettings)
public:
	explicit KdeSpellerSettings();
	virtual ~KdeSpellerSettings();
	static QString suggestLanguage( QString lang, Speller *speller );

protected:
	void loadImpl();
	void saveImpl();
	void cancelImpl() { loadImpl(); }

protected:
    virtual void changeEvent(QEvent *e);

private:
	Ui::KdeSpellerSettings *m_ui;
};

#endif // KDESPELLERSETTINGS_H

