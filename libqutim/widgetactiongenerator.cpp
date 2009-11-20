#include "widgetactiongenerator.h"

namespace qutim_sdk_0_3
{
	struct WidgetActionGeneratorPrivate
	{
		QList<QByteArray> names;
		QList<QVariant>   values;
		ObjectGenerator *generator;
	};

	WidgetActionGenerator::WidgetActionGenerator(const QIcon &icon, const QString &text, const QObject *receiver, const char *member)
			: ActionGenerator(icon, text, receiver, member)
	{
		p->generator = 0;
	}

	WidgetActionGenerator::~WidgetActionGenerator()
	{
	}

	WidgetActionGenerator *WidgetActionGenerator::setWidget(ObjectGenerator *gen)
	{
		Q_ASSERT(gen);
		if (p->generator)
			delete p->generator;
		p->generator = gen;
		return this;
	}

	WidgetActionGenerator *WidgetActionGenerator::addWidgetProperty(const QByteArray &name, const QVariant &value)
	{
		int index = p->names.indexOf(name);
		if (index != -1) {
			p->values[index] = value;
		} else {
			p->names.append(name);
			p->values.append(value);
		}
		return this;
	}

	QObject *WidgetActionGenerator::generateHelper() const
	{
		QAction *action = prepareAction(new QWidgetAction(NULL));
		if (p->generator && action) {
			QWidget *widget = p->generator->generate<QWidget>();
			for (int i = 0; i < p->names.size(); i++)
				widget->setProperty(p->names.at(i), p->values.at(i));
			static_cast<QWidgetAction *>(action)->setDefaultWidget(widget);
		}
		return action;
	}
}
