#ifndef STACKEDCHATBEHAVIOR_H
#define STACKEDCHATBEHAVIOR_H

#include <QWidget>
#include <qutim/settingswidget.h>
#include <chatlayer/chatedit.h>
#include "../stackedchatwidget.h"

namespace Ui {
class StackedChatBehavior;
}

namespace Core
{
namespace AdiumChat
{
namespace Symbian
{


using namespace qutim_sdk_0_3;
class StackedChatBehavior : public SettingsWidget
{
	Q_OBJECT

public:
	explicit StackedChatBehavior();
	virtual ~StackedChatBehavior();
	virtual void cancelImpl();
	virtual void loadImpl();
	virtual void saveImpl();
protected:
	void changeEvent(QEvent *e);
private slots:
	void onValueChanged();
private:
	Ui::StackedChatBehavior *ui;
};

} // namespace Symbian
} // namespace AdiumChat
} // namespace Core
#endif // STACKEDCHATBEHAVIOR_H
