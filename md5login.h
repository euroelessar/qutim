#ifndef MD5LOGIN_H
#define MD5LOGIN_H

#include "snachandler.h"

class Md5Login : public SNACHandler
{
public:
    Md5Login();
	virtual void handleSNAC(OscarConnection *conn, const SNAC &snac);
};

#endif // MD5LOGIN_H
