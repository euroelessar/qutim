/****************************************************************************
 *  submitpage.h
 *
 *  (c) LLC INRUSCOM, Russia
 *
*****************************************************************************/

#ifndef SUBMITPAGE_H
#define SUBMITPAGE_H

#include <QWizardPage>

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

} // namespace Core

#endif // SUBMITPAGE_H
