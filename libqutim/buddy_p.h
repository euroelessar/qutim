#ifndef BUDDY_P_H
#define BUDDY_P_H

#include "chatunit_p.h"
#include "buddy.h"

namespace qutim_sdk_0_3
{
	class BuddyPrivate : public ChatUnitPrivate
	{
	public:
		BuddyPrivate(Buddy *b) : ChatUnitPrivate(b) {}
	};
}

#endif // BUDDY_P_H
