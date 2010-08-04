#include "migrationpagecreator.h"
#include "src/modulemanagerimpl.h"
#include "migrationstartpage.h"

namespace Core
{
	static CoreModuleHelper<MigrationPageCreator> creator_static(
			QT_TRANSLATE_NOOP("Plugin", "Mirgation 0.2 to 0.3"),
			QT_TRANSLATE_NOOP("Plugin", "Module for migration from 0.2 to 0.3")
			);

	MigrationPageCreator::MigrationPageCreator(QObject *parent)
	{
		setParent(parent);
	}

	double MigrationPageCreator::priority() const
	{
		return 100.0;
	}

	QList<QWizardPage *> MigrationPageCreator::pages(QWidget *parent)
	{
		QList<QWizardPage *> list;
		list << new MigrationStartPage(parent);
		return list;
	}
}
