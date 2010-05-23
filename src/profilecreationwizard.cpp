#include "profilecreationwizard.h"
#include "libqutim/profilecreatorpage.h"
#include "libqutim/objectgenerator.h"
#include "profilecreationpage.h"

namespace Core
{
inline bool creatorsLessThan(ProfileCreatorPage *a, ProfileCreatorPage *b)
{
	return a->priority() > b->priority();
}

ProfileCreationWizard::ProfileCreationWizard(ModuleManager *parent,
											 const QString &id, const QString &password,
											 bool singleProfile)
{
	m_manager = parent;
	addPage(new ProfileCreationPage(password, singleProfile, this));
	setField("id", id);
	setField("name", id);
	QList<ProfileCreatorPage *> creators;
	foreach (const ObjectGenerator *gen, moduleGenerators<ProfileCreatorPage>()) {
		ProfileCreatorPage *creator = gen->generate<ProfileCreatorPage>();
		creator->setParent(this);
		creators << creator;
	}
	qSort(creators.begin(), creators.end(), creatorsLessThan);
	foreach (ProfileCreatorPage *creator, creators) {
		foreach (QWizardPage *page, creator->pages(this)) {
			addPage(page);
		}
	}
	setAttribute(Qt::WA_DeleteOnClose, true);
	setAttribute(Qt::WA_QuitOnClose, false);
	connect(this, SIGNAL(destroyed()), m_manager, SLOT(initExtensions()));
}
}
