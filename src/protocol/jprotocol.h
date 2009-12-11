#ifndef JPROTOCOL_H
#define JPROTOCOL_H


namespace Jabber {

	using namespace qutim_sdk_0_3;

	class jProtocol : public QObject
	{
		Q_OBJECT

		public:
			jProtocol();
			static inline jProtocol *instance() {if(!self) qWarning("jProtocol isn't created"); return self;}
			virtual AccountCreationWizard *accountCreationWizard();
			virtual QList<Account *> accounts() const;
			virtual Account *account(const QString &id) const;
	};
} // Jabber namespace

#endif // JPROTOCOL_H
