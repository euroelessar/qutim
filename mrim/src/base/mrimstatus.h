#ifndef MRIMSTATUS_H
#define MRIMSTATUS_H

#include <qutim/status.h>

using namespace qutim_sdk_0_3;

class MrimUserAgent;

class MrimStatus : public Status
{
public:
	MrimStatus(Type type = Offline);
	MrimStatus(const QString &uri,
			   const QString &title = QString(),
			   const QString &desc = QString());
	MrimStatus(const MrimStatus &status);
	MrimStatus(const Status &status);
	MrimStatus &operator =(const MrimStatus &o);
	MrimStatus &operator =(const Status &o);
	
	void setUserAgent(const MrimUserAgent &info);
	void setFlags(quint32 contactFlags);
	quint32 mrimType() const;
	
    QString toString() const;
    static Status fromString(const QString &uri,
                             const QString &title = QString(),
                             const QString &desc = QString());
    static LocalizedString defaultName(const Status &status);
};

#endif // MRIMSTATUS_H
