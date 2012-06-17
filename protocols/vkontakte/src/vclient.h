#ifndef VCLIENT_H
#define VCLIENT_H
#include <client.h>

class VClient : public vk::Client
{
    Q_OBJECT
public:
    explicit VClient(const QString &login, QObject *parent = 0);
    
};

#endif // VCLIENT_H
