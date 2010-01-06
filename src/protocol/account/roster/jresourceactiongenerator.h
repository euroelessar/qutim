#ifndef RESOURCEACTIONGENERATOR_H
#define RESOURCEACTIONGENERATOR_H

#include <qutim/actiongenerator.h>

namespace Jabber
{
	struct JResourceActionGeneratorPrivate;

	class JResourceActionGenerator : public qutim_sdk_0_3::ActionGenerator
	{
		Q_DECLARE_PRIVATE(JResourceActionGenerator)
	public:
		JResourceActionGenerator(const QIcon &icon, const qutim_sdk_0_3::LocalizedString &text,
								const QObject *receiver, const char *member);
		virtual ~JResourceActionGenerator();
		void setFeature(const QString &feature);
		void setFeature(const std::string &feature);
		void setFeature(const QLatin1String &feature);
	protected:
		virtual QObject *generateHelper() const;
		QScopedPointer<JResourceActionGeneratorPrivate> d_ptr;
	};
}

#endif // RESOURCEACTIONGENERATOR_H
