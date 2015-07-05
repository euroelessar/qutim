#ifndef QUICKCHAT_GRADIENTCREATOR_H
#define QUICKCHAT_GRADIENTCREATOR_H

#include <QObject>
#include <QJSValue>
#include <QQmlComponent>

namespace QuickChat {

class GradientCreator : public QObject, public QQmlParserStatus
{
	Q_OBJECT
	Q_INTERFACES(QQmlParserStatus)
	Q_PROPERTY(QObject *object READ object NOTIFY objectChanged)
	Q_PROPERTY(QJSValue data READ data WRITE setData NOTIFY dataChanged)
	Q_PROPERTY(QQmlComponent *gradientComponent READ gradientComponent WRITE setGradientComponent NOTIFY gradientComponentChanged)
	Q_PROPERTY(QQmlComponent *gradientStopComponent READ gradientStopComponent WRITE setGradientStopComponent NOTIFY gradientStopComponentChanged)

public:
	explicit GradientCreator(QObject *parent = 0);

	QObject *object() const;

	QJSValue data() const;
	void setData(QJSValue data);

	QQmlComponent *gradientComponent() const;
	void setGradientComponent(QQmlComponent *gradientComponent);

	QQmlComponent *gradientStopComponent() const;
	void setGradientStopComponent(QQmlComponent *gradientStopComponent);

	void classBegin();
	void componentComplete();

	void update();

signals:
	void objectChanged(QObject *object);
	void dataChanged(QJSValue data);
	void gradientComponentChanged(QQmlComponent *gradientComponent);
	void gradientStopComponentChanged(QQmlComponent *gradientStopComponent);

private:
	bool m_inited = false;
	QObject *m_object = nullptr;
	QJSValue m_data;
	QQmlComponent *m_gradientComponent = nullptr;
	QQmlComponent *m_gradientStopComponent = nullptr;
};

} // namespace QuickChat

#endif // QUICKCHAT_GRADIENTCREATOR_H
