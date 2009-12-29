#ifndef RESOURCEACTIONGENERATOR_H
#define RESOURCEACTIONGENERATOR_H

#include <qutim/actiongenerator.h>

namespace Jabber
{
class ResourceActionGenerator : public qutim_sdk_0_3::ActionGenerator
{
public:
	ResourceActionGenerator(const QIcon &icon, const qutim_sdk_0_3::LocalizedString &text,
							const QObject *receiver, const char *member);
	virtual ~ResourceActionGenerator();
protected:
	virtual QObject *generateHelper() const;
};
}

#endif // RESOURCEACTIONGENERATOR_H
