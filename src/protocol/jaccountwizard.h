#ifndef JACCOUNTWIZARD_H
#define JACCOUNTWIZARD_H

#include <qutim/protocol.h>
#include <qutim/extensioninfo.h>
#include "jprotocol.h"

namespace Jabber {


	class JAccountWizardPage;

	enum JAccountType {
		AccountTypeJabber,
		AccountTypeLivejournal,
		AccountTypeGoogletalk,
		AccountTypeQip,
		AccountTypeYandex
	};

	class JAccountWizard : public AccountCreationWizard
	{
		Q_OBJECT

		public:
			JAccountWizard();
			~JAccountWizard();
			QList<QWizardPage *> createPages(QWidget *parent);
			void createAccount();
		protected:
			JAccountWizardPage *page;
			JAccountType type;
		private:
			JProtocol *protocol;
	};

	class LJAccountWizard : public JAccountWizard
	{
		Q_OBJECT

		public:
			LJAccountWizard();
			~LJAccountWizard();
	};

	class GTAccountWizard : public JAccountWizard
	{
		Q_OBJECT

		public:
			GTAccountWizard();
			~GTAccountWizard();
	};

	class YAccountWizard : public JAccountWizard
	{
		Q_OBJECT

		public:
			YAccountWizard();
			~YAccountWizard();
	};

	class QIPAccountWizard : public JAccountWizard
	{
		Q_OBJECT

		public:
			QIPAccountWizard();
			~QIPAccountWizard();
	};
}

#endif // JACCOUNTWIZARD_H
