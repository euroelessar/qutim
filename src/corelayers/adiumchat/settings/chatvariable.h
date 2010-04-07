#ifndef CHATVARIABLE_H
#define CHATVARIABLE_H

#include <QWidget>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QFontDialog>
#include <QColorDialog>
#include <QColor>
#include <QDoubleSpinBox>

namespace Core
{
	struct CustomChatStyle
	{
		QString selectors;
		QString parameter;
		QString value;
	};

	class ChatVariable
	{
		public:
			virtual const CustomChatStyle &style() = 0;
	};
}
Q_DECLARE_INTERFACE(Core::ChatVariable, "org.qutim.core.ChatVariable")

namespace Core
{
	class ChatFont : public QWidget, ChatVariable
	{
		Q_OBJECT
		Q_INTERFACES(Core::ChatVariable)
		public:
			ChatFont(const CustomChatStyle &style, QWidget *parent = 0);
			const CustomChatStyle &style();
		signals:
			void changeValue();
		private slots:
			void changeCurrentFont();
		private:
			QLabel *fontLabel;
			QToolButton *changeButton;
			CustomChatStyle m_style;
	};

	class ChatColor : public QToolButton, ChatVariable
	{
		Q_OBJECT
		Q_INTERFACES(Core::ChatVariable)
		public:
			ChatColor(const CustomChatStyle &style, QWidget *parent = 0);
			const CustomChatStyle &style();
		signals:
			void changeValue();
		private slots:
			void changeCurrentColor();
		private:
			QColor color;
			CustomChatStyle m_style;
	};

	class ChatNumeric : public QDoubleSpinBox, ChatVariable
	{
		Q_OBJECT
		Q_INTERFACES(Core::ChatVariable)
		public:
			ChatNumeric(const CustomChatStyle &style,
					double min, double max, double step, QWidget *parent = 0);
			const CustomChatStyle &style();
		signals:
			void changeValue();
		private slots:
			void onChangeValue();
		private:
			CustomChatStyle m_style;
	};
}

#endif // CHATVARIABLE_H
