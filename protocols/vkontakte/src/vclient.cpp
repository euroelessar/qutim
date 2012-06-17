#include "vclient.h"

VClient::VClient(const QString &login, QObject *parent) :
    vk::Client(parent)
{
    setLogin(login);
}

