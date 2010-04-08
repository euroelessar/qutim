#ifndef CONTACTINFO_H
#define CONTACTINFO_H

#include "QWidget"
#include "QGroupBox"
#include "QScrollArea"
#include "ui_userinformation.h"
#include <libqutim/buddy.h>
#include <libqutim/inforequest.h>

class QLabel;
class QGridLayout;
class QVBoxLayout;

namespace Core
{
using namespace qutim_sdk_0_3;

class InfoLayout : public QGridLayout
{
public:
	InfoLayout(QWidget *parent = 0);
	~InfoLayout();
	void addItem(const InfoItem &item);
	void addSpacer();
	void addItems(const QList<InfoItem> &items);
protected:
	void addDataWidget(QWidget *widget, const QString &name);
	QLabel *addLabel(const QString &data, const QString &name);
	void addWidget(QWidget *w);
	void addWidget(QWidget *, int row, int column, Qt::Alignment = 0);
	void addWidget(QWidget *, int row, int column, int rowSpan, int columnSpan, Qt::Alignment = 0);
	void addLayout(QLayout *, int row, int column, Qt::Alignment = 0);
	void addLayout(QLayout *, int row, int column, int rowSpan, int columnSpan, Qt::Alignment = 0);
private:
	int m_row;
};

class MainWindow : public QWidget
{
	Q_OBJECT
public:
	MainWindow();
	void setBuddy(Buddy *buddy, InfoRequest *request);
private slots:
	void onRequestStateChanged(InfoRequest::State state);
	void onRequestButton();
private:
	void addItems(const InfoItem &items);
	QString summary(const InfoItem &item);
private:
	Ui::userInformationClass ui;
	InfoRequest *request;
	Buddy *m_buddy;
};

class ContactInfo : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "ContactInfo")
	Q_CLASSINFO("Uses", "IconLoader")
public:
	ContactInfo();
public slots:
	void show(Buddy *buddy);
private slots:
	void onShow();
private:
	QPointer<MainWindow> info;
};

inline void InfoLayout::addWidget(QWidget *w)
{
	QGridLayout::addWidget(w);
}

inline void InfoLayout::addWidget(QWidget *w, int row, int column, Qt::Alignment alignment)
{
	QGridLayout::addWidget(w, row, column, alignment);
}

inline void InfoLayout::addWidget(QWidget *w, int row, int column, int rowSpan, int columnSpan, Qt::Alignment alignment)
{
	QGridLayout::addWidget(w, row, column, rowSpan, columnSpan, alignment);
}

inline void InfoLayout::addLayout(QLayout *w, int row, int column, Qt::Alignment alignment)
{
	QGridLayout::addLayout(w, row, column, alignment);
}

inline void InfoLayout::addLayout(QLayout *w, int row, int column, int rowSpan, int columnSpan, Qt::Alignment alignment)
{
	QGridLayout::addLayout(w, row, column, rowSpan, columnSpan, alignment);
}

}

#endif // CONTACTINFO_H
