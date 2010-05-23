#ifndef PROFILECREATIONWIZARD_H
#define PROFILECREATIONWIZARD_H

#include <QWizard>
#include "libqutim/modulemanager.h"

namespace Core
{
using namespace qutim_sdk_0_3;
class ProfileCreationWizard : public QWizard
{
	Q_OBJECT
public:
	explicit ProfileCreationWizard(ModuleManager *parent,
								   const QString &id, const QString &password,
								   bool singleProfile = false);
private:
	ModuleManager *m_manager;
};
}

#endif // PROFILECREATIONWIZARD_H
