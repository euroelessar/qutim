#ifndef ADIUMCHATFORM_H
#define ADIUMCHATFORM_H

#include <chatforms/abstractchatform.h>

namespace qutim_sdk_0_3
{
	class SettingsItem;
}

namespace Core
{
namespace AdiumChat
{

class TabbedChatForm : public AbstractChatForm
{
	Q_OBJECT
	Q_CLASSINFO("Uses","SettingsLayer")
public:
	explicit TabbedChatForm();
	~TabbedChatForm();
protected:
	virtual AbstractChatWidget *createWidget(const QString &key);
private:
	qutim_sdk_0_3::SettingsItem *m_settingsItem;
};

}
}

#endif //ADIUMCHATFORM_H
