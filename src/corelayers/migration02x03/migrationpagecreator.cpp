#include "migrationpagecreator.h"
#include "migrationstartpage.h"

namespace Core
{
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
