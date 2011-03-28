/****************************************************************************
 *  submitpage.cpp
 *
 *  (c) LLC INRUSCOM, Russia
 *
*****************************************************************************/

#include "submitpage.h"
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QTextBrowser>
#include <qutim/systeminfo.h>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

namespace Core {

using namespace qutim_sdk_0_3;

SubmitPage::SubmitPage(QWidget* parent): QWizardPage(parent)
{
	setTitle(tr("Congratulations"));
	setSubTitle(tr("You've just created a profile for qutIM. Click Finish to proceed to adding user accounts"));

	QVBoxLayout *l = new QVBoxLayout(this);
	m_submitBox = new QCheckBox(tr("Are you want to report about this setup?"), this);
	QLabel *label = new QLabel(tr("Information to be transferred to the qutIM's authors:"), this);
	m_information = new QTextBrowser(this);
	m_information->setHtml(tr("<b>OS:</b> %1 <br />"
							  "<b>Short:</b> %2 <br />"
							  "<b>Full:</b> %3 <br />"
							  "<b>qutIM Version:</b> %4 <br />"
							  //"<b>Screen resolution</b>"
							  ).arg(SystemInfo::getVersion())
						   .arg(SystemInfo::getName())
						   .arg(SystemInfo::getFullName())
						   .arg(qutimVersionStr()));

	l->addWidget(m_submitBox);
	l->addWidget(label);
	l->addWidget(m_information);

}

bool SubmitPage::validatePage()
{
	if (m_submitBox->checkState() != Qt::Checked)
		QWizardPage::validatePage();

	QUrl url(QLatin1String("http://qutim.org/request.php"));
	url.addQueryItem(QLatin1String("os"), SystemInfo::getVersion());
	url.addQueryItem(QLatin1String("short"), SystemInfo::getName());
	url.addQueryItem(QLatin1String("full"), SystemInfo::getFullName());
	url.addQueryItem(QLatin1String("version"), qutimVersionStr());

	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	QNetworkRequest request(url);
	manager->get(request);
	QWizardPage::validatePage();
}


} // namespace Core
