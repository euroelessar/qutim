#ifndef JINFOREQUEST_H
#define JINFOREQUEST_H

#include <qutim/inforequest.h>

namespace gloox
{
	class VCard;
}

namespace Jabber
{
	using namespace qutim_sdk_0_3;
	using namespace gloox;

	class JVCardManager;
	struct JInfoRequestPrivate;

	class JInfoRequest : public InfoRequest
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE(JInfoRequest)
		public:
			JInfoRequest(JVCardManager *manager, const QString &contact);
			~JInfoRequest();
			QList<InfoItem> items() const;
			InfoItem item(const QString &name) const;
			State state() const;
			void setFetchedVCard(const VCard *vcard);
		private:
			QScopedPointer<JInfoRequestPrivate> d_ptr;
	};
}

#endif // JINFOREQUEST_H
