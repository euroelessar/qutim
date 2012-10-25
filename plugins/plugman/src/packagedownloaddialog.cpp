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

#include "packagedownloaddialog.h"
#include "packagedelegate.h"
#include <qutim/debug.h>
#include <qutim/config.h>
#include <qutim/thememanager.h>
#include <QVBoxLayout>

using namespace qutim_sdk_0_3;

PackageDownloadDialog::PackageDownloadDialog(const QStringList &categories, const QString &path) :
	m_view(new DeclarativeView(this))
{
	m_view->setResizeMode(DeclarativeView::SizeRootObjectToView);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(m_view);
	layout->setMargin(0);
	setLayout(layout);

	Config config("plugman");
	QString themeName = config.value("theme", "default");
	QString themePath = ThemeManager::path(QLatin1String("plugman"), themeName);
	qDebug("PlugMan: %s", qPrintable(themePath + "/main.qml"));
	m_view->setSource(QUrl::fromLocalFile(themePath + "/main.qml"));

	setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Get Hot New Stuff"));
    resize(800, 800);
    setMinimumSize(600, 600);
}

PackageDownloadDialog::~PackageDownloadDialog()
{
}

