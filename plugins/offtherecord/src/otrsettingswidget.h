/*
  
	Copyright (c) 2010 by Stanislav (proDOOMman) Kosolapov <prodoomman@shell.tor.hu>
	
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef OTR_SETTINGSWIDGET_H
#define OTR_SETTINGSWIDGET_H

#include "otrmessaging.h"
#include "smpdialog.h"
#include <QMessageBox>
#include <qutim/settingswidget.h>

namespace Ui {
class OtrSettingsWidget;
}

class OtrSettingsWidget : public qutim_sdk_0_3::SettingsWidget
{
	Q_OBJECT
public:
	OtrSettingsWidget();
	
protected:
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();
	
private slots:
	void on_pushButton_released();
	void on_forgetButton_released();
	void updateFingerprints();
	void updateKeys();
	void keyChanged(int row);
	void fingerprintChanged(int row);
	
private:
	Ui::OtrSettingsWidget *ui;
	OtrSupport::OtrMessaging *m_otr;
	QList<OtrSupport::Fingerprint> m_fingerprints;
};

#endif // OTR_SETTINGSWIDGET_H
