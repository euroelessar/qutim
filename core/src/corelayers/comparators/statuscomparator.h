#ifndef CORE_STATUSCOMPARATOR_H
#define CORE_STATUSCOMPARATOR_H
#include <qutim/contact.h>

namespace Core {

class StatusComparator : public Ureen::ContactComparator
{
	Q_OBJECT
	Q_CLASSINFO("SettingsDescription", "Sort by contact's status")
public:
	explicit StatusComparator();
	virtual int compare(Ureen::Contact *a, Ureen::Contact *b);
protected:
	virtual void doStartListen(Ureen::Contact *contact);
	virtual void doStopListen(Ureen::Contact *contact);
private slots:
	void onContactChanged();
};

} // namespace Core

#endif // CORE_STATUSCOMPARATOR_H
