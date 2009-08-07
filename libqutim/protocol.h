#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "libqutim_global.h"

namespace qutim_sdk_0_3
{
	class Contact;
	class MetaContact;
	class Account;

	class Protocol : public QObject
	{
		Q_OBJECT
	public:
		Protocol();
	};
}

#endif // PROTOCOL_H
