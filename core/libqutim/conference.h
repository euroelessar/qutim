#ifndef CONFERENCE_H
#define CONFERENCE_H

#include "chatunit.h"
#include "buddy.h"

namespace qutim_sdk_0_3
{
class ConferencePrivate;
class Buddy;

class LIBQUTIM_EXPORT Conference : public ChatUnit
{
	Q_DECLARE_PRIVATE(Conference)
	Q_PROPERTY(QString topic READ topic WRITE setTopic NOTIFY topicChanged)
	Q_PROPERTY(qutim_sdk_0_3::Buddy* me READ me NOTIFY meChanged)
	Q_PROPERTY(bool isJoined READ isJoined NOTIFY joinedChanged)
	Q_OBJECT
public:
	Conference(Account *account);
	virtual ~Conference();
	virtual QString topic() const;
	virtual void setTopic(const QString &topic);
	virtual Buddy *me() const = 0;
	bool isJoined() const;
public slots:
	virtual void join() = 0;
	virtual void leave() = 0;
	virtual void setJoined(bool isJoined);
	virtual void invite(qutim_sdk_0_3::Contact *contact, const QString &reason = QString());
signals:
	void topicChanged(const QString &current, const QString &previous);
	void meChanged(qutim_sdk_0_3::Buddy *me);
	void left();
	void joined();
	void joinedChanged(bool isJoined);
protected:
	bool event(QEvent *ev);
private:
	Q_PRIVATE_SLOT(d_func(),void _q_joined());
	Q_PRIVATE_SLOT(d_func(),void _q_leaved());
};

}

Q_DECLARE_METATYPE(qutim_sdk_0_3::Conference*)
Q_DECLARE_METATYPE(QList<qutim_sdk_0_3::Conference*>)

#endif // CONFERENCE_H
