#include "gradientcreator.h"
#include <qutim/debug.h>

namespace QuickChat {

GradientCreator::GradientCreator(QObject *parent) :
	QObject(parent)
{
}

QObject *GradientCreator::object() const
{
	return m_object;
}

QJSValue GradientCreator::data() const
{
	return m_data;
}

void GradientCreator::setData(QJSValue data)
{
	if (m_data.strictlyEquals(data))
		return;

	m_data = data;
	emit dataChanged(data);

	update();
}

QQmlComponent *GradientCreator::gradientComponent() const
{
	return m_gradientComponent;
}

void GradientCreator::setGradientComponent(QQmlComponent *gradientComponent)
{
	if (m_gradientComponent == gradientComponent)
		return;

	m_gradientComponent = gradientComponent;
	emit gradientComponentChanged(gradientComponent);

	update();
}

QQmlComponent *GradientCreator::gradientStopComponent() const
{
	return m_gradientStopComponent;
}

void GradientCreator::setGradientStopComponent(QQmlComponent *gradientStopComponent)
{
	if (m_gradientStopComponent == gradientStopComponent)
		return;

	m_gradientStopComponent = gradientStopComponent;
	emit gradientStopComponentChanged(gradientStopComponent);

	update();
}

void GradientCreator::classBegin()
{
}

void GradientCreator::componentComplete()
{
	m_inited = true;
	update();
}

void GradientCreator::update()
{
	if (!m_inited)
		return;

	if (!m_gradientComponent || !m_gradientStopComponent) {
		if (m_object) {
			QObject *object = m_object;
			m_object = nullptr;
			emit objectChanged(m_object);
			object->deleteLater();
		}
		return;
	}

	int stopsCount = m_data.property(QStringLiteral("length")).toInt();
	int currentStopsCount = 0;
	if (m_object) {
		QQmlListReference stops(m_object, "stops");
		currentStopsCount = stops.count();
	}

	QObject *object = nullptr;
	if (currentStopsCount == stopsCount) {
		if (stopsCount == 0)
			return;
		object = m_object;
	} else {
		object = m_gradientComponent->create();
	}

	QQmlListReference stops(object, "stops");
	for (int i = 0; i < stopsCount; ++i) {
		QObject *stop = nullptr;
		if (stops.count() == i) {
			stop = m_gradientStopComponent->create();
			stop->setParent(object);
			stops.append(stop);
		} else {
			stop = stops.at(i);
		}

		QJSValue stopData = m_data.property(i);
		auto position = stopData.property(QStringLiteral("position")).toVariant();
		auto color = stopData.property(QStringLiteral("color")).toVariant();

		stop->setProperty("position", position);
		stop->setProperty("color", color);
	}

	if (m_object != object) {
		qSwap(m_object, object);
		emit objectChanged(m_object);
		object->deleteLater();
	}
}

} // namespace QuickChat
