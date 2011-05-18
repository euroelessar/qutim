#ifndef STATUSACTIONGENERATOR_P_H
#define STATUSACTIONGENERATOR_P_H

#include "actiongenerator_p.h"
#include "statusactiongenerator.h"

namespace qutim_sdk_0_3
{
class StatusActionGeneratorPrivate : public ActionGeneratorPrivate
{
public:
	Status status;
	Status::Type statusType;
};

class StatusActionHandler : public QObject
{
	Q_OBJECT
public:
	StatusActionHandler(QObject *parent = 0);
	QByteArray memberName() { return m_memberName; }
public slots:
	void changeStatus(QAction *action,QObject *controller);
private:
	QByteArray m_memberName;
};
}

#endif // STATUSACTIONGENERATOR_P_H
