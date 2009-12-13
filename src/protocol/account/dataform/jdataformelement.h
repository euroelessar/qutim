#ifndef JDATAFORMELEMENT_H
#define JDATAFORMELEMENT_H

namespace Jabber
{	
	class JDataFormElement
	{
		public:
			virtual QString dfValue() = 0;
			virtual void dfSetValue(const QString &value) = 0;
			virtual QString dfName() = 0;
			virtual void dfSetName(const QString &name) = 0;
			virtual QObject* instance() = 0;
	};
}
Q_DECLARE_INTERFACE(Jabber::JDataFormElement, "org.qutim.jabber.DataFormElement");

#endif // JDATAFORMELEMENT_H
