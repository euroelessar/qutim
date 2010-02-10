#ifndef STATUSACTIONGENERATOR_H
#define STATUSACTIONGENERATOR_H

#include "actiongenerator.h"
#include "status.h"

namespace qutim_sdk_0_3
{
	class StatusActionGeneratorPrivate;

	class LIBQUTIM_EXPORT StatusActionGenerator : public ActionGenerator
	{
		Q_DECLARE_PRIVATE(StatusActionGenerator)
	public:
		StatusActionGenerator(const Status &status);
	protected:
		virtual QObject *generateHelper() const;
	};
}

#endif // STATUSACTIONGENERATOR_H
