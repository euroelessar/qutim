#ifndef CHATVARIABLE_H
#define CHATVARIABLE_H

#include <QWidget>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QFontDialog>
#include <QColorDialog>
#include <QDoubleSpinBox>
#include <QLocale>

namespace Core
{
	class ChatVariable
	{
		public:
			virtual const QString &style() = 0;
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
			ChatFont(const QString &selectors, const QString &parameter, const QString &value, QWidget *parent = 0);
			const QString &style();
		signals:
			void changeValue();
		private slots:
			void changeCurrentFont();
		private:
			QLabel *fontLabel;
			QToolButton *changeButton;
			QString m_style;
	};

	class ChatColor : public QToolButton, ChatVariable
	{
		Q_OBJECT
		Q_INTERFACES(Core::ChatVariable)
		public:
			ChatColor(const QString &selectors, const QString &parameter, const QString &value, QWidget *parent = 0);
			const QString &style();
		signals:
			void changeValue();
		private slots:
			void changeCurrentColor();
		private:
			QColor color;
			QString m_style;
	};

	class ChatNumeric : public QDoubleSpinBox, ChatVariable
	{
		Q_OBJECT
		Q_INTERFACES(Core::ChatVariable)
		public:
			ChatNumeric(const QString &selectors, const QString &parameter, double value,
					double min, double max, double step, QWidget *parent = 0);
			const QString &style();
		signals:
			void changeValue();
		private slots:
			void onChangeValue();
		protected:
			void fill();
		private:
			QString m_style;
	};
}

#endif // CHATVARIABLE_H
