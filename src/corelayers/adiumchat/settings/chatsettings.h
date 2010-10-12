#ifndef CHATSETTINGS_H
#define CHATSETTINGS_H

#include <QObject>
#include <qutim/startupmodule.h>

namespace Core
{
namespace AdiumChat
{

class ChatSettings : public QObject
{
	Q_OBJECT
public:
	ChatSettings();
};

}
}
#endif // CHATSETTINGS_H
