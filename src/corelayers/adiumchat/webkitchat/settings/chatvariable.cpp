#include "chatvariable.h"
#include <QStringBuilder>

namespace Core
{
namespace AdiumChat
{

ChatFont::ChatFont(const CustomChatStyle &style, QWidget *parent) : QWidget(parent)
{
	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	setLayout(layout);
	fontLabel = new QLabel();
	QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	fontLabel->setSizePolicy(sizePolicy);
	changeButton = new QToolButton();
	changeButton->setText(tr("Change"));
	connect(changeButton, SIGNAL(clicked()), SLOT(changeCurrentFont()));
	layout->addWidget(fontLabel);
	layout->addWidget(changeButton);
	QString cvalue = style.value;
	QFont fvalue;
	QRegExp rxBold("(bold)");
	QRegExp rxItalic("(italic)");
	QRegExp rxSCaps("(small-caps)");
	QRegExp rxSize("(\\d+)(pt|px)");
	if (cvalue.contains(rxSize)) {
		if (rxSize.cap(2) == "pt")
			fvalue.setPointSize(rxSize.cap(1).toInt());
		else
			fvalue.setPixelSize(rxSize.cap(1).toInt());
		cvalue.remove(rxSize);
	}
	if (cvalue.contains(rxBold)) {
		fvalue.setBold(true);
		cvalue.remove(rxBold);
	}
	if (cvalue.contains(rxItalic)) {
		fvalue.setItalic(true);
		cvalue.remove(rxItalic);
	}
	if (cvalue.contains(rxSCaps)) {
		fvalue.setCapitalization(QFont::SmallCaps);
		cvalue.remove(rxBold);
	}
	cvalue = cvalue.simplified();
	fvalue.setFamily(cvalue);
	fontLabel->setFont(fvalue);
	fontLabel->setText(QString("%1 %2").arg(fvalue.pointSize() != -1
											? QString("%1pt").arg(fvalue.pointSize())
											: QString("%1px").arg(fvalue.pixelSize())).arg(fvalue.family()));
	m_style.selector = style.selector;
	m_style.parameter = style.parameter;
}

const CustomChatStyle &ChatFont::chatStyle()
{
	QFont fvalue(fontLabel->font());
	QString fontSize(fvalue.pointSize() != -1
					 ? QString("%1pt ").arg(fvalue.pointSize())
					 : QString("%1px ").arg(fvalue.pixelSize()));
	m_style.value = QString(fvalue.bold() ? "bold " : "") % QString(fvalue.italic() ? "italic " : "")
			% QString(fvalue.capitalization() ? "small-caps " : "") % fontSize % fvalue.family();
	return m_style;
}

void ChatFont::changeCurrentFont()
{
	QFontDialog *dialog = new QFontDialog();
	bool ok;
	QFont newFont = dialog->getFont(&ok, fontLabel->font(), this);
	if (ok && newFont != fontLabel->font())
	{
		fontLabel->setFont(newFont);
		fontLabel->setText(QString("%1 %2").arg(newFont.pointSize() != -1
												? QString("%1pt").arg(newFont.pointSize())
												: QString("%1px").arg(newFont.pixelSize())).arg(newFont.family()));
		emit changeValue();
	}
}

ChatColor::ChatColor(const CustomChatStyle &style, QWidget *parent) : QToolButton(parent)
{
	color = QColor(style.value);
	connect(this, SIGNAL(clicked()), SLOT(changeCurrentColor()));
	setStyleSheet("background: "+color.name());
	m_style.selector = style.selector;
	m_style.parameter = style.parameter;
}

const CustomChatStyle &ChatColor::chatStyle()
{
	m_style.value = color.name();
	return m_style;
}

void ChatColor::changeCurrentColor()
{
	QColorDialog *dialog = new QColorDialog();
	QColor newColor = dialog->getColor(color);
	if (newColor.isValid() && newColor != color)
	{
		color = newColor;
		setStyleSheet("background: "+color.name());
		emit changeValue();
	}
}

ChatNumeric::ChatNumeric(const CustomChatStyle &style,
						 double min, double max, double step, QWidget *parent) : QDoubleSpinBox(parent)
{
	connect(this, SIGNAL(valueChanged(double)), SLOT(onChangeValue()));
	int dec = QString::number(step).section(".", 1).length();
	setMinimum(min);
	setMaximum(max);
	setSingleStep(step);
	setDecimals(dec);
	setValue(style.value.toDouble());
	m_style.selector = style.selector;
	m_style.parameter = style.parameter;
}

const CustomChatStyle &ChatNumeric::chatStyle()
{
	m_style.value = QString::number(value());
	return m_style;
}

void ChatNumeric::onChangeValue()
{
	emit changeValue();
}

ChatBoolean::ChatBoolean(const CustomChatStyle &style,
						 const QString &trueValue, const QString &falseValue, QWidget *parent) : QCheckBox(parent)
{
	connect(this, SIGNAL(toggled(bool)), SLOT(onChangeValue()));
	m_trueValue = trueValue;
	m_falseValue = falseValue;
	if (style.value == m_trueValue)
		setChecked(true);
	else
		setChecked(false);
	m_style.selector = style.selector;
	m_style.parameter = style.parameter;
}

const CustomChatStyle &ChatBoolean::chatStyle()
{
	m_style.value = isChecked() ? m_trueValue : m_falseValue;
	return m_style;
}

void ChatBoolean::onChangeValue()
{
	emit changeValue();
}

}
}
