#ifndef QUTIM_SDK_0_3_KEYCHAIN_H
#define QUTIM_SDK_0_3_KEYCHAIN_H

#include "asyncresult.h"

namespace qutim_sdk_0_3 {

class KeyChain : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "KeyChain")
public:
	enum Error
	{
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

	virtual AsyncResult<ReadResult> read(const QString &key) = 0;
	virtual AsyncResult<Result> write(const QString &key, const QString &value) = 0;
	virtual AsyncResult<Result> write(const QString &key, const QByteArray &value) = 0;
	virtual AsyncResult<Result> remove(const QString &key) = 0;
};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_KEYCHAIN_H
