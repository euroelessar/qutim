#include "chatvariable.h"

namespace Core
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
		int sec = 0;
		QString cvalue = style.value.section(" ", sec, sec);
		QFont fvalue;
		QRegExp rxSize("(\\d+)(pt|px)");
		while (!cvalue.isEmpty())
		{
			if (cvalue == "bold")
				fvalue.setBold(true);
			else if (cvalue == "italic")
				fvalue.setItalic(true);
			else if (cvalue == "small-caps")
				fvalue.setCapitalization(QFont::SmallCaps);
			else if (cvalue.contains(rxSize))
			{
				if (rxSize.cap(2) == "pt")
					fvalue.setPointSize(rxSize.cap(1).toInt());
				else
					fvalue.setPixelSize(rxSize.cap(1).toInt());
				break;
			}
			sec++;
			cvalue = style.value.section(" ", sec, sec);
		}
		fvalue.setFamily(style.value.section(" ", ++sec));
		fontLabel->setFont(fvalue);
		fontLabel->setText(QString("%1 %2").arg(fvalue.pointSize() != -1
				? QString("%1pt").arg(fvalue.pointSize())
				: QString("%1px").arg(fvalue.pixelSize())).arg(fvalue.family()));
		m_style.selectors = style.selectors;
		m_style.parameter = style.parameter;
	}

	const CustomChatStyle &ChatFont::style()
	{
		QFont fvalue(fontLabel->font());
		QString fontSize(fvalue.pointSize() != -1
				? QString("%1pt ").arg(fvalue.pointSize())
				: QString("%1px ").arg(fvalue.pixelSize()));
		m_style.value = QString("%1 %2 %3 %4 %5; }")
				.arg(fvalue.bold() ? "bold" : "")
				.arg(fvalue.italic() ? "italic " : "")
				.arg(fvalue.capitalization() ? "small-caps " : "")
				.arg(fontSize)
				.arg(fvalue.family());
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
		m_style.selectors = style.selectors;
		m_style.parameter = style.parameter;
	}

	const CustomChatStyle &ChatColor::style()
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
		//int dec = m_style.range.section(";", 2, 2).section(".", 1).length();
		setMinimum(min);
		setMaximum(max);
		setSingleStep(step);
		//setDecimals(dec);
		setValue(style.value.toDouble());
		m_style.selectors = style.selectors;
		m_style.parameter = style.parameter;
	}

	const CustomChatStyle &ChatNumeric::style()
	{
		m_style.value = QString::number(value());
		return m_style;
	}

	void ChatNumeric::onChangeValue()
	{
		emit changeValue();
	}
}
