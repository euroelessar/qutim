#include "domaininfo.h"

namespace qutim_sdk_0_3
{
	class DomainInfoPrivate
	{
		QHostAddress address;    // for A, Aaaa
		QByteArray name;         // for Mx, Srv, Cname, Ptr, Ns
		int priority;            // for Mx, Srv
		int weight;              // for Srv
		int port;                // for Srv
		QList<QByteArray> texts; // for Txt
		QByteArray cpu;          // for Hinfo
		QByteArray os;           // for Hinfo
	};

	//DomainInfo::DomainInfo()
	//{
	//}
}
