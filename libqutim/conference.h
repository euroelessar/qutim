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
	Q_OBJECT
public:
	Conference(Account *account);
	virtual ~Conference();
	virtual QString topic() const;
	virtual void setTopic(const QString &topic);
	virtual Buddy *me() const = 0;
public slots:
	virtual void join() = 0;
	virtual void leave() = 0;
	virtual void invite(qutim_sdk_0_3::Contact *contact, const QString &reason = QString());
signals:
	void topicChanged(const QString &current, const QString &previous);
	void meChanged(qutim_sdk_0_3::Buddy *me);
	void left();
	void joined();
protected:
	bool event(QEvent *ev);
};

//small draft

//events
//groupchat-bookmark-list QList<AbstractBookMarksItem>
//groupchat-bookmark-fields AbstractBookMarksItem
//groupchat-bookmark-add AbstractBookMarksItem
//groupchat-bookmark-remove AbstractBookMarksItem
//groupchat-bookmark-update AbstractBookMarksItem
class DataItem;
class AbstractBookMarksItem
{
public:
	AbstractBookMarksItem(const QString &name = QString());
	virtual bool isRecent() = 0;
	virtual DataItem *fields() = 0;
	virtual QString setName() = 0;
	virtual QString name() = 0;
	virtual QVariant description() = 0;
	virtual ~AbstractBookMarksItem() {}
};

}

Q_DECLARE_METATYPE(qutim_sdk_0_3::Conference*)
Q_DECLARE_METATYPE(QList<qutim_sdk_0_3::Conference*>)

#endif // CONFERENCE_H
