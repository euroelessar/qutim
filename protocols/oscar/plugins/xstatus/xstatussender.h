#ifndef XSTATUSSENDER_H
#define XSTATUSSENDER_H


#include <QTimer>
#include "../../src/icqcontact.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class XStatusSender;

class XStatusSenderList : public QObject
{
	Q_OBJECT
public:
	XStatusSenderList();
	XStatusSender *getSender(IcqAccount *account);
private slots:
	void accountDestroyed(QObject *obj);
private:
	QHash<IcqAccount*, XStatusSender*> m_senders;
};

class XStatusSender : public QObject
{
	Q_OBJECT
public:
	static void sendXStatus(IcqContact *contact, quint64 cookie);
private slots:
	void sendXStatus();
	void statusChanged(const qutim_sdk_0_3::Status &current, const qutim_sdk_0_3::Status &previous);
private:
	friend class XStatusSenderList;
	XStatusSender(IcqAccount *account);
	void sendXStatusImpl(IcqContact *contact, quint64 cookie);
	QList<QPointer<IcqContact> > m_contacts;
	QTimer m_timer;
	uint m_lastTime; // unix time when the last xstraz packet was sent
};

} } // namespace qutim_sdk_0_3::oscar

#endif // XSTATUSSENDER_H
