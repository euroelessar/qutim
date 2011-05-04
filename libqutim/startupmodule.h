#ifndef STARTUPMODULE_H
#define STARTUPMODULE_H

#include "libqutim_global.h"

namespace qutim_sdk_0_3 {

class LIBQUTIM_EXPORT StartupModule
{
public:
	StartupModule();
	virtual ~StartupModule();
};

} // namespace qutim_sdk_0_3

Q_DECLARE_INTERFACE(qutim_sdk_0_3::StartupModule, "org.qutim.StartupModule")

#endif // STARTUPMODULE_H
