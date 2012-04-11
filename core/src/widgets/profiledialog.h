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

#ifndef PROFILEDIALOG_H
#define PROFILEDIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include "modulemanagerimpl.h"

namespace qutim_sdk_0_3
{
	class Config;
}

namespace Ui {
    class ProfileDialog;
}

namespace Core
{
class ProfileDialog : public QDialog
{
    Q_OBJECT
public:
	ProfileDialog(Config &config, ModuleManager *parent = 0);
	~ProfileDialog();
	static Config profilesInfo();
	static QString profilesConfigPath(bool *isSystem = 0);
	static bool acceptProfileInfo(const Config &config, const QString &password, bool checkHash = true);

protected slots:
	void login(const QString &password);
	void on_profilesButton_clicked();
	void currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

protected:
    void changeEvent(QEvent *e);

private:
	ModuleManager *m_manager;
	Ui::ProfileDialog *ui;
};
}

#endif // PROFILEDIALOG_H

