#ifndef JCERTINFO_H
#define JCERTINFO_H

#include <gloox/gloox.h>
#include <QtCore/QDateTime>
#include "ui_jcertinfo.h"

namespace Jabber
{

	struct JCertInfoPrivate;

	class JCertInfo : public QDialog
	{
		Q_OBJECT
	public:
		JCertInfo(const gloox::CertInfo &info, QWidget *parent = 0);
		virtual ~JCertInfo();
		
		bool exec(bool &result);
	private:
		JCertInfoPrivate *p;
	};
}


#endif // JCERTINFO_H
