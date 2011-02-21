#ifndef PROFILECREATIONWIZARD_H
#define PROFILECREATIONWIZARD_H

#include <QWizard>
#include <qutim/modulemanager.h>

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
protected:
    void done(int result);
private:
	ModuleManager *m_manager;
	bool m_singleProfile;
};
}

#endif // PROFILECREATIONWIZARD_H
