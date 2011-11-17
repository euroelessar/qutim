/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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

#include "submitpage.h"
#include <qutim/libqutim_version.h>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QTextBrowser>
#include <qutim/systeminfo.h>
#include <QNetworkRequest>
#include <QApplication>
#include <QDesktopWidget>
#include <QLocale>
#include <qutim/debug.h>

namespace Core {

using namespace qutim_sdk_0_3;

SubmitPage::SubmitPage(QWidget* parent): QWizardPage(parent)
{
	setTitle(tr("Congratulations"));
	setSubTitle(tr("You've just created a profile for qutIM. Click Finish to proceed to adding user accounts"));

	QVBoxLayout *l = new QVBoxLayout(this);
	m_submitBox = new QCheckBox(tr("Would you like to send details about your current setup?"), this);
	QLabel *label = new QLabel(tr("Information to be transferred to the qutIM's authors:"), this);
	m_information = new QTextBrowser(this);
	QRect size = qApp->desktop()->screenGeometry();
	QLocale locale = QLocale::system();

	m_information->setHtml(tr("<b>Short:</b> %1 <br />"
							  "<b>Version:</b> %2 <br />"
							  "<b>Full:</b> %3 <br />"
							  "<b>qutIM Version:</b> %4 <br />"
							  "<b>Qt Version:</b> %5 (%6 bit) <br />"
							  "<b>Screen resolution:</b> %7 x %8 <br />"
							  "<b>System locale:</b> %9<br />"
							  ).arg(SystemInfo::getName())
						   .arg(SystemInfo::getVersion())
						   .arg(SystemInfo::getFullName())
						   .arg(QLatin1String(versionString()))
						   .arg(QLatin1String(qVersion()))
						   .arg(QString::number(QSysInfo::WordSize))
						   .arg(size.width())
						   .arg(size.height())
						   .arg(locale.name()));

	l->addWidget(m_submitBox);
	l->addWidget(label);
	l->addWidget(m_information);

	m_submitBox->setCheckState(Qt::Checked);
	registerField("StatisticsSent", m_submitBox, "checked", SIGNAL(stateChanged(int)));
}

bool SubmitPage::validatePage()
{
	if (m_submitBox->checkState() != Qt::Checked)
		return QWizardPage::validatePage();

	new RequestHelper;
	return QWizardPage::validatePage();
}

RequestHelper::RequestHelper(QObject *parent) : QNetworkAccessManager(parent)
{
	QRect size = qApp->desktop()->screenGeometry();
	QLocale locale = QLocale::system();

	//QByteArray lang = qgetenv("LANGUAGE");
	//if (lang.isEmpty())
	//	lang = qgetenv("LANG");

	QUrl url(QLatin1String("http://qutim.org/stats"));
	//QUrl url(QLatin1String("http://q.nico-izo.ru/stats.php")); //temporary

	url.addQueryItem(QLatin1String("os"), SystemInfo::getVersion());
	url.addQueryItem(QLatin1String("short"), SystemInfo::getName());
	url.addQueryItem(QLatin1String("full"), SystemInfo::getFullName());
	url.addQueryItem(QLatin1String("version"), versionString());
	url.addQueryItem(QLatin1String("qt"), QLatin1String(qVersion()));
	url.addQueryItem(QLatin1String("wordSize"), QString::number(QSysInfo::WordSize));
	url.addQueryItem(QLatin1String("width"), QString::number(size.width()));
	url.addQueryItem(QLatin1String("height"), QString::number(size.height()));
	url.addQueryItem(QLatin1String("locale"), locale.name());
	//url.addQueryItem(QLatin1String("lang"), lang);

	QNetworkRequest request(url);
	QNetworkReply *reply = get(request);
	connect(reply, SIGNAL(finished()), SLOT(onFinished()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
			this, SLOT(onError(QNetworkReply::NetworkError)));
}

void RequestHelper::onFinished()
{
	deleteLater();
}

void RequestHelper::onError(QNetworkReply::NetworkError code)
{
	debug() << code;
	deleteLater();
}

} // namespace Core

