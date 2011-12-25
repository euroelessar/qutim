/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#include "aboutdialogwrapper.h"
#include "quickaboutdialog.h"
#include <QHash>
#include <QRegExp>
#include <QTextDocument>
#include <qdeclarative.h>
#include <qutim/plugin.h>

using namespace qutim_sdk_0_3;

namespace MeegoIntegration
{

Q_GLOBAL_STATIC(QList<AboutDialogWrapper*>, m_managers)
QuickAboutDialog* AboutDialogWrapper::m_currentDialog;

AboutDialogWrapper::AboutDialogWrapper()
{
	m_developers = toHtml(PersonInfo::authors(), true);
	emit developersChanged();

	m_translators = toHtml(PersonInfo::translators(), true);
	emit translatorsChanged();

	m_version = QLatin1String(versionString());
	emit versionChanged();

	m_qtver = tr("Based on Qt %1 (%2 bit).").arg(QLatin1String(qVersion()), QString::number(QSysInfo::WordSize));
	emit qtVerChanged();

	QString license = tr("<div><b>qutIM</b> %1 is licensed under GNU General Public License, version 2"
			     " or (at your option) any later version.</div>"
			     "<div>qutIM resources such as themes, icons, sounds may come along with a "
			     "different license.</div><br><hr><br>").arg(versionString());

	license += "<br><a href=\"http://www.gnu.org/licenses/gpl-2.0.html\">GPLv2 license text</a>";

	license.replace(QLatin1String("\n\n"), "<br><br>");
	m_license = license;
	emit licenseChanged();

	m_managers()->append(this);
}

AboutDialogWrapper::~AboutDialogWrapper()
{
	m_managers()->removeOne(this);
}

void AboutDialogWrapper::init()
{
	qmlRegisterType<AboutDialogWrapper>("org.qutim", 0, 3, "AboutDialogWrapper");
}

QString AboutDialogWrapper::developers()  const
{
	return m_developers;
}

QString AboutDialogWrapper::translators() const
{
	return m_translators;
}

QString AboutDialogWrapper::qtVer() const
{
	return m_qtver;
}

QString AboutDialogWrapper::version() const
{
	return m_version;
}

QString AboutDialogWrapper::license() const
{
	return m_license;
}

void AboutDialogWrapper::showDialog(QuickAboutDialog * aboutDialog)
{
	m_currentDialog = aboutDialog;
	for (int i = 0; i < m_managers()->count();i++)
	{
		emit m_managers()->at(i)->shown();
	}
}

QString AboutDialogWrapper::toHtml(const QList<PersonInfo> &persons, bool useTask)
{
	QString html;
	for (int i = 0; i < persons.size(); i++) {
		const PersonInfo &info = persons.at(i);
		html += QLatin1String("<p><div><b>");
		html += Qt::escape(info.name());
		html += QLatin1String("</b>");
		if (useTask) {
			html += QLatin1String("</div><div>");
			html += Qt::escape(info.task());
		}
		html += QLatin1String("</div>");
		if (!info.email().isEmpty()) {
			html += QLatin1String("<div><a href=\"mailto:\"");
			html += Qt::escape(info.email());
			html += QLatin1String("\">");
			html += Qt::escape(info.email());
			html += QLatin1String("</a></div>");
		}
		html += QLatin1String("</p>");
	}
	return html;
}
}

