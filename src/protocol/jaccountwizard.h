#ifndef JACCOUNTWIZARD_H
#define JACCOUNTWIZARD_H

#include <qutim/protocol.h>
#include "jprotocol.h"
#include "jaccountwizardpage.h"

namespace Jabber {

	using namespace qutim_sdk_0_3;

	class JAccountWizardPage;

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
			JProtocol *protocol;
			enum JAccountType {JABBER, LIVEJOURNAL, GOOGLETALK, QIP, YANDEX};
			JAccountType type;
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
