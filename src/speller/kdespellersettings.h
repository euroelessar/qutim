/****************************************************************************
 * kdespellersettings.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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
