/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2014 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "aboutinfo.h"
#include <qqml.h>
#include <qutim/plugin.h>
#include <QFile>

namespace Core {
namespace SimpleAbout {

using namespace qutim_sdk_0_3;

AboutInfo::AboutInfo(QObject *parent) :
    QObject(parent)
{
	QFile licenseFile(QStringLiteral(":/GPL"));
	if (licenseFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		m_license = QString::fromUtf8(licenseFile.readAll()).toHtmlEscaped();
		m_license.replace(QStringLiteral("\n\n"), QStringLiteral("<br><br>"));
	} else {
		m_license = QStringLiteral("<a href=\"http://www.gnu.org/licenses/gpl-3.0.html\">GPLv3</a>");
	}
}

QString AboutInfo::qutimVersion() const
{
	return QLatin1String(versionString());
}

QString AboutInfo::qtVersion() const
{
	return QLatin1String(qVersion());
}

int AboutInfo::wordSize() const
{
	return QSysInfo::WordSize;
}

QString AboutInfo::license() const
{
	QString license = tr("<div><b>qutIM</b> is licensed under GNU General Public License, version 3"
								" or (at your option) any later version.</div>"
								"<div>qutIM resources such as themes, icons, sounds may come along with a "
								"different license.</div><br><hr><br>");
	return license + m_license;
}

QString AboutInfo::developers() const
{
	return toHtml(PersonInfo::authors(), true);
}

QString AboutInfo::translators() const
{
	QString result = toHtml(PersonInfo::translators(), false);
	if (result.isEmpty())
		return QStringLiteral("Sorry, it is untranslated version :(");
	return result;
}

QString AboutInfo::toHtml(const QList<PersonInfo> &persons, bool useTask) const
{
	QString html;
	for (int i = 0; i < persons.size(); i++) {
		const PersonInfo &info = persons.at(i);
		html += QStringLiteral("<p><div><b>");
		html += info.name().toString().toHtmlEscaped();
		html += QStringLiteral("</b>");
		if (useTask) {
			html += QStringLiteral("</div><div>");
			html += info.task().toString().toHtmlEscaped();
		}
		html += QStringLiteral("</div>");
		if (!info.email().isEmpty()) {
			html += QStringLiteral("<div><a href=\"mailto:\"");
			html += info.email().toHtmlEscaped();
			html += QStringLiteral("\">");
			html += info.email().toHtmlEscaped();
			html += QStringLiteral("</a></div>");
		}
		html += QStringLiteral("</p>");
	}
	return html;
}

void registerTypes()
{
    qmlRegisterType<AboutInfo>("org.qutim.simpleabout", 0, 4, "AboutInfo");
}

} // namespace SimpleAbout
} // namespace Core
