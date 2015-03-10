#ifndef QUTIM_SDK_0_3_KEYCHAIN_H
#define QUTIM_SDK_0_3_KEYCHAIN_H

#include "asyncresult.h"

namespace qutim_sdk_0_3 {

class Account;

class KeyChain : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "KeyChain")
public:
	enum Error
	{
		NoError,
		EntryNotFound,
		OtherError
	};

	KeyChain();
	~KeyChain();

	struct Result
	{
		Error error;
		QString errorString;
	};

	struct ReadResult : Result
	{
		QString textData;
		QByteArray binaryData;
	};

	AsyncResult<ReadResult> read(Account *account);
	AsyncResult<Result> write(Account *account, const QString &value);
	AsyncResult<Result> write(Account *account, const QByteArray &value);
	AsyncResult<Result> remove(Account *account);

protected:
	virtual AsyncResult<ReadResult> doRead(const QString &key) = 0;
	virtual AsyncResult<Result> doWrite(const QString &key, const QString &value) = 0;
	virtual AsyncResult<Result> doWrite(const QString &key, const QByteArray &value) = 0;
	virtual AsyncResult<Result> doRemove(const QString &key) = 0;
};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_KEYCHAIN_H
