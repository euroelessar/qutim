#ifndef JSERVICERECEIVER_H
#define JSERVICERECEIVER_H

namespace Jabber
{
	class JDiscoItem;

	class JServiceReceiver
	{
		public:
			virtual void setInfo(const QString &id) = 0;
			virtual void setItems(const QString &id, const QList<JDiscoItem *> &items) = 0;
			virtual void setError(const QString &id, JDiscoItem *di) = 0;
	};
}
Q_DECLARE_INTERFACE(Jabber::JServiceReceiver, "org.qutim.jabber.ServiceReceiver");

#endif // JSERVICERECEIVER_H
