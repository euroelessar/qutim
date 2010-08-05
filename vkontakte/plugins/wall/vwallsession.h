#ifndef VWALLSESSION_H
#define VWALLSESSION_H

#include <QObject>
#include <qutim/conference.h>



using namespace qutim_sdk_0_3;
class VAccount;
class VWallSessionPrivate;
class VWallSession : public Conference
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(VWallSession)
public:
	explicit VWallSession(const QString& id, VAccount *account);
	virtual QString id() const;
	virtual void join();
	virtual void leave();
	virtual Buddy* me() const;
	virtual bool sendMessage(const qutim_sdk_0_3::Message& message);
    virtual ~VWallSession();
    virtual QString title() const;
signals:

public slots:
	
private:
	QScopedPointer<VWallSessionPrivate> d_ptr;
};

#endif // VWALLSESSION_H
