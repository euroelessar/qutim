#pragma once
#include <QObject>
#include <qutim/config.h>

class Timeout : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int timeout READ timeout)

public:
    Timeout(){}
    ~Timeout(){}

    int timeout() const
    {
        qutim_sdk_0_3::Config cfg(QStringLiteral("behavior"));
        cfg.beginGroup(QStringLiteral("popup"));
        int timeout = cfg.value(QStringLiteral("timeout"), 5) * 1000;
        cfg.endGroup();
        return timeout;
    }

};
