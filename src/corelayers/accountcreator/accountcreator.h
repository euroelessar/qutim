#ifndef ACCOUNTCREATOR_H
#define ACCOUNTCREATOR_H

#include <qutim/profilecreatorpage.h>

using namespace qutim_sdk_0_3;

namespace Core
{
	class AccountCreator : public QObject
	{
		Q_OBJECT
	public:
		AccountCreator();
		virtual ~AccountCreator();
	};

//	class AccountPageCreator : public ProfileCreatorPage
//	{
//		Q_OBJECT
//	public:
//		explicit AccountPageCreator(QObject *parent = 0);
//		virtual double priority() const;
//		virtual QList<QWizardPage *> pages(QWidget *parent);
//	};
}

#endif // ACCOUNTCREATOR_H
