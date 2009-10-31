/****************************************************************************
 * kdespellersettings.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

using namespace Sonnet;

namespace Ui {
    class KdeSpellerSettings;
}

class KdeSpellerSettings : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(KdeSpellerSettings)
public:
    explicit KdeSpellerSettings(QWidget *parent = 0);
	virtual ~KdeSpellerSettings();
	static QString suggestLanguage( QString lang, Speller *speller );
	void loadSettings( const QString &profile_name, Speller *speller );
	void saveSettings();

signals:
	void settingsChanged();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::KdeSpellerSettings *m_ui;
	QString m_profile_name;
};

#endif // KDESPELLERSETTINGS_H
