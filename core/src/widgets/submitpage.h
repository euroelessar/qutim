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

#ifndef SUBMITPAGE_H
#define SUBMITPAGE_H

#include <QWizardPage>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class QTextBrowser;
class QCheckBox;
namespace Core {

class SubmitPage : public QWizardPage
{
	Q_OBJECT
public:
    SubmitPage(QWidget *parent = 0);
	virtual bool validatePage();
private:
	QCheckBox *m_submitBox;
	QTextBrowser *m_information;
};

class RequestHelper : public QNetworkAccessManager
{
	Q_OBJECT
public:
	RequestHelper(QObject *parent = 0);
public slots:
	void onFinished();
	void onError(QNetworkReply::NetworkError);
};

} // namespace Core

#endif // SUBMITPAGE_H

