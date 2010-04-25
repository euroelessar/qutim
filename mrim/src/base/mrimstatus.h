#ifndef MRIMSTATUS_H
#define MRIMSTATUS_H

#include <qutim/status.h>

using namespace qutim_sdk_0_3;

class MrimStatus
{
public:
    static QString toString(const Status &status);
    static Status fromString(const QString &uri,
                             const QString &title = QString(),
                             const QString &desc = QString());
    static LocalizedString defaultName(const Status &status);
};

#endif // MRIMSTATUS_H
