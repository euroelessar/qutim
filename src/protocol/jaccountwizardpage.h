#ifndef JACCOUNTWIZARDPAGE_H
#define JACCOUNTWIZARDPAGE_H

#include <QWizardPage>
#include <QValidator>
#include "jaccountwizard.h"

namespace Ui
{
	class JAccountWizardPage;
}

namespace Jabber
{
	class JJidValidatorPrivate;

	class JJidValidator : public QValidator
	{
		Q_DECLARE_PRIVATE(JJidValidator)
		Q_OBJECT
	public:
		JJidValidator(const QString &server = QString(), QObject *parent = 0);
		JJidValidator(const std::string &server, QObject *parent = 0);
		~JJidValidator();
		QString server() const;
		virtual State validate(QString &, int &) const;
		virtual void fixup(QString &) const;
	protected:
		QScopedPointer<JJidValidatorPrivate> d_ptr;
	};

	class JAccountWizardPage : public QWizardPage
	{
		Q_OBJECT

		public:
			JAccountWizardPage(JAccountWizard *accountWizard, JAccountType type, QWidget *parent = 0);
			~JAccountWizardPage();
			bool validatePage();
			QString jid();
			QString passwd();
			bool isSavePasswd();
		private:
			JAccountWizard *m_accountWizard;
			JAccountType m_type;
			Ui::JAccountWizardPage *ui;
	};
}

#endif // JACCOUNTWIZARDPAGE_H
