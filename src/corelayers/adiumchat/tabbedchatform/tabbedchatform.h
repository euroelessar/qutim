#ifndef ADIUMCHATFORM_H
#define ADIUMCHATFORM_H

#include <chatlayer/chatforms/abstractchatform.h>

namespace Core
{
namespace AdiumChat
{

class TabbedChatForm : public AbstractChatForm
{
	Q_OBJECT
public:
	explicit TabbedChatForm();
	~TabbedChatForm();
protected:
	virtual AbstractChatWidget *createWidget(const QString &key);
};

}
}

#endif //ADIUMCHATFORM_H
