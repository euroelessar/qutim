#ifndef MIGRATIONPAGECREATOR_H
#define MIGRATIONPAGECREATOR_H

#include "libqutim/profilecreatorpage.h"

using namespace qutim_sdk_0_3;

namespace Core
{
class MigrationPageCreator : public ProfileCreatorPage
{
	Q_OBJECT
public:
	explicit MigrationPageCreator(QObject *parent = 0);
	virtual double priority() const;
	virtual QList<QWizardPage *> pages(QWidget *parent);
};
}

#endif // MIGRATIONPAGECREATOR_H
