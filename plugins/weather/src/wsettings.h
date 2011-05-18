/****************************************************************************
 * wsettings.h
 *
 *  Copyright (c) 2010 by Belov Nikita <null@deltaz.org>
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

#ifndef WSETTINGS_H
#define WSETTINGS_H

#include <qutim/configbase.h>
#include <qutim/localizedstring.h>
#include <qutim/settingswidget.h>

#include <QDesktopServices>
#include <QFileDialog>
#include <QFocusEvent>
#include <QtNetwork>
#include <QtXml>

#include "ui_wsettings.h"
#include "wlistitem.h"

using namespace qutim_sdk_0_3;

class WSettings : public SettingsWidget
{
	Q_OBJECT

public:
	WSettings();
	~WSettings();

	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();

protected:
	bool eventFilter( QObject *o, QEvent *e );

private slots:
	void on_addCityButton_clicked();
	void on_searchButton_clicked();
	void on_chooseButton_clicked();
	void addButton_clicked();
	void deleteButton_clicked();

	void searchFinished( QNetworkReply *reply );

private:
	Ui::WSettingsClass ui;

	QNetworkAccessManager *m_networkManager;
	QList<QString> m_searchResults;
};

#endif // WSETTINGS_H