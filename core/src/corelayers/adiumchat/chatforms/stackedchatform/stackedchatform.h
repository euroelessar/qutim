#ifndef STACKEDCHATFORM_H
#define STACKEDCHATFORM_H

#include <chatforms/abstractchatform.h>

namespace qutim_sdk_0_3
{
	class SettingsItem;
}

namespace Core
{
namespace AdiumChat
{

class StackedChatForm : public AbstractChatForm
{
	Q_OBJECT
	Q_CLASSINFO("Uses","SettingsLayer")
public:
	explicit StackedChatForm();
	~StackedChatForm();
protected:
	virtual AbstractChatWidget *createWidget(const QString &key);
private:
	qutim_sdk_0_3::SettingsItem *m_settingsItem;
};

}
}

#endif //STACKEDCHATFORM_H
