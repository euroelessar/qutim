#ifndef ACCOUNTCREATOR_H
#define ACCOUNTCREATOR_H

#include "libqutim/libqutim_global.h"

using namespace qutim_sdk_0_3;

namespace Core
{
	class AccountCreator : public QObject
	{
		Q_OBJECT
	public:
		AccountCreator();
		virtual ~AccountCreator();
	};
}

#endif // ACCOUNTCREATOR_H
