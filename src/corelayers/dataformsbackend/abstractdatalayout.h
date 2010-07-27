#ifndef ABSTRACTDATALAYOUT_H
#define ABSTRACTDATALAYOUT_H

#include <QWidget>
#include <QGridLayout>
#include <QHostAddress>
#include <QPointer>
#include <libqutim/dataforms.h>

class QLabel;
Q_DECLARE_METATYPE(QHostAddress);

namespace Core
{
using namespace qutim_sdk_0_3;

class AbstractDataLayout : public QGridLayout
{
public:
	AbstractDataLayout(QWidget *parent = 0);
	~AbstractDataLayout();
	virtual bool addItem(const DataItem &item) = 0;
	bool addItems(const QList<DataItem> &items);
	void addSpacer();
	void addRow(QWidget *widget) { addRow(0, widget); }
	void addRow(QWidget *title, QWidget *widget, Qt::Alignment widgetAligment = 0);
protected:
	QStyle* getStyle() const;
	Qt::Alignment labelAlignment() { return m_labelAlignment; };
private:
	Qt::Alignment m_labelAlignment;
	mutable QPointer<QStyle> m_style;
	int m_row;
};

QPixmap variantToPixmap(const QVariant &data, const QSize &size);

}

#endif // ABSTRACTDATALAYOUT_H
