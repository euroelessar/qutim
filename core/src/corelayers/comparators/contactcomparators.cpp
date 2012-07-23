#include "contactcomparators.h"
#include "statuscomparator.h"
#include "lastactivitycomparator.h"

namespace Core {

void ContactComparators::init()
{
	qutim_sdk_0_3::ExtensionIcon icon(QLatin1String(""));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Contact comparators"),
			QT_TRANSLATE_NOOP("Plugin", "Default qutIM contacts comparators collection"),
			QUTIM_VERSION, icon);

	addExtension<StatusComparator>(QT_TRANSLATE_NOOP("Plugin", "Status comparator"),
								QT_TRANSLATE_NOOP("Plugin", "Default contacts comparator, which sort by status"), icon);
    addExtension<LastActivityComparator>(QT_TRANSLATE_NOOP("Plugin", "Last activity comparator"),
                                        QT_TRANSLATE_NOOP("Plugin", "Contacts comparator, which sort by last last activity"), icon);
}

bool ContactComparators::load()
{
	return true;
}

bool ContactComparators::unload()
{
	return true;
}

} // namespace Core

QUTIM_EXPORT_PLUGIN(Core::ContactComparators)
