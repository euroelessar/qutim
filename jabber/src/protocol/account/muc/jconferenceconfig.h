#ifndef JCONFERENCECONFIG_H
#define JCONFERENCECONFIG_H

#include <jreen/mucroom.h>
#include <qutim/icon.h>
#include <QWidget>

namespace Jabber
{
using namespace qutim_sdk_0_3;

class JMUCSession;
struct JConferenceConfigPrivate;

class JConferenceConfig : public QWidget
{
	Q_OBJECT
public:
	JConferenceConfig(jreen::MUCRoom *room, QWidget *parent = 0);
	~JConferenceConfig();
protected:
	void closeEvent(QCloseEvent *);
signals:
	void destroyDialog();
protected slots:
	void onConfigurationReceived(const jreen::DataForm::Ptr &form);
	void on_applyButton_clicked();
	void on_okButton_clicked();
private:
	QScopedPointer<JConferenceConfigPrivate> p;
};
}

#endif // JCONFERENCECONFIG_H
