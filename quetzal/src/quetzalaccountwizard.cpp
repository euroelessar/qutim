#include "quetzalaccountwizard.h"
#include "quetzalprotocol.h"
#include <qutim/debug.h>
#include <qutim/metaobjectbuilder.h>

Protocol *quetzal_find_protocol(const char *name)
{
	foreach (Protocol *proto, Protocol::all()) {
		if (!qstrcmp(proto->metaObject()->className(), name)) {
			return proto;
		}
	}
	return 0;
}

QuetzalAccountWizard::QuetzalAccountWizard(const QuetzalMetaObject *meta) :
		AccountCreationWizard(m_proto = static_cast<QuetzalProtocol*>(quetzal_find_protocol(MetaObjectBuilder::info(meta, "DependsOn"))))
{
}

QList<QWizardPage *> QuetzalAccountWizard::createPages(QWidget *parent)
{
	Q_UNUSED(parent);
	QList<QWizardPage*> pages;
	return pages;
}
