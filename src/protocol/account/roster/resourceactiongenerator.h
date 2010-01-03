#ifndef RESOURCEACTIONGENERATOR_H
#define RESOURCEACTIONGENERATOR_H

#include <qutim/actiongenerator.h>

namespace Jabber
{
class JResourceActionGenerator : public qutim_sdk_0_3::ActionGenerator
{
public:
	JResourceActionGenerator(const QIcon &icon, const qutim_sdk_0_3::LocalizedString &text,
							const QObject *receiver, const char *member);
	virtual ~JResourceActionGenerator();
protected:
	virtual QObject *generateHelper() const;
};
}

#endif // RESOURCEACTIONGENERATOR_H
