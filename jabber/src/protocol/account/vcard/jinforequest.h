#ifndef JINFOREQUEST_H
#define JINFOREQUEST_H

#include <qutim/inforequest.h>
#include <string>
#include <list>

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
			enum DataType
			{
				Nick,
				FirstName,
				MiddleName,
				LastName
			};

			JInfoRequest(JVCardManager *manager, const QString &contact);
			~JInfoRequest();
			DataItem item(const QString &name) const;
			State state() const;
			void setFetchedVCard(const VCard *vcard);
		private:
			void addItem(DataType type, DataItem &group, const QVariant &data);
			void addItem(DataType type, DataItem &group, const std::string &data);
			void addItemList(DataType type, DataItem &group, const std::string &data);
			void addItem(DataType type, DataItem &group, const std::list<std::string> &data);
			QScopedPointer<JInfoRequestPrivate> d_ptr;
	};
}

#endif // JINFOREQUEST_H
