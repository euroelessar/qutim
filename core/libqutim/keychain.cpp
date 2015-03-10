#include "keychain.h"
#include "account.h"
#include "protocol.h"

#include <QStringBuilder>

namespace qutim_sdk_0_3 {

KeyChain::KeyChain()
{
}

KeyChain::~KeyChain()
{
}

static QString generateId(Account *account)
{
	return account->protocol()->id() % QLatin1Char('.') % account->id();
}

AsyncResult<KeyChain::ReadResult> KeyChain::read(Account *account)
{
	return doRead(generateId(account));
}

AsyncResult<KeyChain::Result> KeyChain::write(Account *account, const QString &value)
{
	return doWrite(generateId(account), value);
}

AsyncResult<KeyChain::Result> KeyChain::write(Account *account, const QByteArray &value)
{
	return doWrite(generateId(account), value);
}

AsyncResult<KeyChain::Result> KeyChain::remove(Account *account)
{
	return doRemove(generateId(account));
}

} // namespace qutim_sdk_0_3
