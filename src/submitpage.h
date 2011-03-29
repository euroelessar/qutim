/****************************************************************************
 *  submitpage.h
 *
 *  (c) LLC INRUSCOM, Russia
 *
*****************************************************************************/

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
