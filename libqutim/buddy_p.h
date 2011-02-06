#ifndef BUDDY_P_H
#define BUDDY_P_H

#include "chatunit_p.h"
#include "buddy.h"

namespace qutim_sdk_0_3
{

class BuddyPrivate : public ChatUnitPrivate
{
	Q_DECLARE_PUBLIC(Buddy)
public:
	BuddyPrivate(Buddy *b) : ChatUnitPrivate(b) {}
	void _q_status_changed(const qutim_sdk_0_3::Status &now,const qutim_sdk_0_3::Status &old);

};

}

#endif // BUDDY_P_H
