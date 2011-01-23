#ifndef JCONFERENCECONFIG_H
#define JCONFERENCECONFIG_H

#include <jreen/mucroom.h>
#include <qutim/icon.h>
#include <qutim/settingswidget.h>

namespace Jabber
{
using namespace qutim_sdk_0_3;

class JMUCSession;
struct JConferenceConfigPrivate;

class JConferenceConfig : public SettingsWidget
{
	Q_OBJECT
public:
	JConferenceConfig();
	~JConferenceConfig();
	
	virtual void setController(QObject *controller);
protected:
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();
protected slots:
	void onConfigurationReceived(const jreen::DataForm::Ptr &form);
	void onDataChanged();
private:
	QScopedPointer<JConferenceConfigPrivate> p;
};
}

#endif // JCONFERENCECONFIG_H
