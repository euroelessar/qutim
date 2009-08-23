#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "libqutim_global.h"

namespace qutim_sdk_0_3
{
	class Contact;
	class Protocol;

	class Account : public QObject
	{
		Q_OBJECT
	public:
		Account(Protocol *protocol);
	};
}

#endif // ACCOUNT_H
