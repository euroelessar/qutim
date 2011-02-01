#ifndef STACKEDCHATFORM_H
#define STACKEDCHATFORM_H

#include <chatlayer/chatforms/abstractchatform.h>
#include <QPointer>

namespace qutim_sdk_0_3
{
	class SettingsItem;
}

namespace Core
{
namespace AdiumChat
{
namespace Symbian
{
	
class StackedChatWidget;
class StackedChatForm : public AbstractChatForm
{
	Q_OBJECT
	Q_CLASSINFO("Uses","SettingsLayer")
	Q_CLASSINFO("Uses","ContactList")
public:
	explicit StackedChatForm();
	~StackedChatForm();
protected:
	virtual AbstractChatWidget *createWidget(const QString &key);
private:
	qutim_sdk_0_3::SettingsItem *m_settingsItem;
	QPointer<StackedChatWidget> m_chatWidget;
};

} // namespace Symbian
} // namespace AdiumChat
} // namespace Core

#endif //STACKEDCHATFORM_H
