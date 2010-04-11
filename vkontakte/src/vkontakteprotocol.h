#ifndef VKONTAKTEPROTOCOL_H
#define VKONTAKTEPROTOCOL_H
#include <qutim/protocol.h>
#include "vkontakte_global.h"
#include <qutim/debug.h>

struct VkontakteProtocolPrivate;
class LIBVKONTAKTE_EXPORT VkontakteProtocol : public Protocol
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(VkontakteProtocol)
	Q_CLASSINFO("Protocol", "vkontakte")
public:
	VkontakteProtocol();
	virtual ~VkontakteProtocol();
	virtual Account* account(const QString& id) const;
	virtual void loadAccounts();
	virtual QList< Account* > accounts() const;
	static inline VkontakteProtocol *instance() { if (!self) warning() << "IcqProtocol isn't created"; return self; }
private:
	static VkontakteProtocol *self;
	QScopedPointer<VkontakteProtocolPrivate> d_ptr;
	friend class VAccountCreator;
};

#endif // VKONTAKTEPROTOCOL_H
