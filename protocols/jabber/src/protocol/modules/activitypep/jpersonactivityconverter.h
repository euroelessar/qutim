#ifndef JPERSONACTIVITYCONVERTER_H
#define JPERSONACTIVITYCONVERTER_H

#include "../../../sdk/jabber.h"
#include <qutim/localizedstring.h>
#include <qutim/extensionicon.h>
#include <jreen/activity.h>

namespace Jabber {

using namespace qutim_sdk_0_3;

class JPersonActivityConverter : public QObject , public PersonEventConverter
{
	Q_OBJECT
	Q_INTERFACES(Jabber::PersonEventConverter)
public:
	JPersonActivityConverter();
	virtual ~JPersonActivityConverter();
	virtual QString name() const;
	virtual int entityType() const;
	virtual QSharedPointer<Jreen::Payload> convertTo(const QVariantHash &map) const;
	virtual QVariantHash convertFrom(const QSharedPointer<Jreen::Payload> &entity) const;
	static qutim_sdk_0_3::LocalizedString generalTitle(Jreen::Activity::General general);
	static qutim_sdk_0_3::LocalizedString specificTitle(Jreen::Activity::Specific specific);
};

class JPersonActivityRegistrator : public QObject
{
	Q_OBJECT
public:
	JPersonActivityRegistrator();
protected:
	bool eventFilter(QObject *obj, QEvent *event);
};

} // namespace Jabber

#endif // JPERSONACTIVITYCONVERTER_H
