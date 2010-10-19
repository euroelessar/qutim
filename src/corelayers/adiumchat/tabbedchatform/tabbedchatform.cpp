#include "tabbedchatform.h"
#include "tabbedchatwidget.h"

namespace Core
{
namespace AdiumChat
{

using namespace qutim_sdk_0_3;

TabbedChatForm::TabbedChatForm()
{
}

TabbedChatForm::~TabbedChatForm()
{
}

AbstractChatWidget *TabbedChatForm::createWidget(const QString &key)
{
	return new TabbedChatWidget(key);
}
}
}
