#include "asyncresult.h"

namespace qutim_sdk_0_3 {

Detail::AsyncInvoker::AsyncInvoker()
{
	qRegisterMetaType<Callback>();
}

Detail::AsyncInvoker::~AsyncInvoker()
{
}

void Detail::AsyncInvoker::invoke(const Callback &callback)
{
	callback();
}

} // namespace qutim_sdk_0_3
