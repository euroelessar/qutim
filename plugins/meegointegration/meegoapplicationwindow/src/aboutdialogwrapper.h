/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#ifndef ABOUTDIALOGWRAPPER_H
#define ABOUTDIALOGWRAPPER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include "quickaboutdialog.h"
#include <qutim/personinfo.h>

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;
class AboutDialogWrapper : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString developers READ developers NOTIFY developersChanged)
	Q_PROPERTY(QString translators READ translators NOTIFY translatorsChanged)
	Q_PROPERTY(QString qtVer READ qtVer NOTIFY qtVerChanged)
	Q_PROPERTY(QString version READ version NOTIFY versionChanged)
	Q_PROPERTY(QString license READ license NOTIFY licenseChanged)

public:
	AboutDialogWrapper();
	~AboutDialogWrapper();
	QString developers() const;
	QString translators() const;
	QString qtVer() const;
	QString version() const;
	QString license() const;

	static void init();
	static void showDialog(QuickAboutDialog * aboutDialog);

signals:
	void developersChanged();
	void translatorsChanged();
	void qtVerChanged();
	void versionChanged();
	void licenseChanged();
	void shown();

private:
	QString m_developers;
	QString m_translators;
	QString m_qtver;
	QString m_version;
	QString m_license;

	QString toHtml(const QList<qutim_sdk_0_3::PersonInfo> &persons, bool useTask);

	static QuickAboutDialog *m_currentDialog;

};
}

#endif /* ABOUTDIALOGWRAPPER_H */
