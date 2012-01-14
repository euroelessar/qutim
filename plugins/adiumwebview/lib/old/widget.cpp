/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "widget.h"
#include "ui_widget.h"
#include "webkitnetworkaccessmanager.h"
#include "webkitdeclarativeview.h"
#include <QDebug>
#include <QFile>
#include <QTimer>
#include <QWebInspector>
#include <QWebFrame>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QElapsedTimer>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
	QStringList arguments = qApp->arguments();
	Q_ASSERT_X(arguments.size() > 1, "AdiumWebView", "Application must be provide by single argument - path to theme");
	m_style.setStylePath(arguments.at(1));
	ui->comboBox->blockSignals(true);
	ui->comboBox->addItems(m_style.variants());
	ui->comboBox->setCurrentIndex(ui->comboBox->findText(m_style.activeVariant()));
	ui->comboBox->blockSignals(false);
	
	m_preview = WebKitPreviewLoader().loadPreview(SHARE_PATH + QLatin1String("Preview.plist"));
	
	{
		// QWebView
		fillPage(ui->webView->page());
//		QTimer::singleShot(1000, this, SLOT(onTimer()));
	}
	{
		// QDeclarativeView
//		QWebPage *page = new QWebPage(this);
//		qmlRegisterType<WebKitDeclarativeView>("org.adium", 0, 1, "WebChatView");
		QString baseHtml = m_style.baseTemplateForChat(m_preview->chat);
		ui->declarativeView->setResizeMode(QDeclarativeView::SizeRootObjectToView);
		QDeclarativeEngine *engine = ui->declarativeView->engine();
		QDeclarativeContext *context = engine->rootContext();
		QVariantMap data;
		data.insert("baseHtml", baseHtml);
		data.insert("scripts", contentScripts());
		context->setContextProperty("adiumData", data);
//		context->setContextProperty("adiumData", qVariantFromValue<QObject*>(page));
//		ui->declarativeView->setSource(SHARE_PATH + QLatin1String("qml/ChatView.qml"));
//		fillPage(page);
	}
//	fillPage(ui->webView->page());
}

Widget::~Widget()
{
    delete ui;
}

void Widget::onLoad()
{
	QWebPage *page = qobject_cast<QWebPage*>(sender());
	QElapsedTimer timer;
	timer.start();
	QStringList scripts = contentScripts();
	qDebug() << timer.elapsed(); timer.start();
	foreach (const QString &script, scripts) {
		page->mainFrame()->evaluateJavaScript(script);
	};
	qDebug() << timer.elapsed();
	if (page != ui->webView->page()) {
		fillPage(ui->webView->page());
//		qDebug() << ui->webView->page()->mainFrame()->toHtml();
	}
//	QTimer::singleShot(2000, this, SLOT(onTimer()));
}

void Widget::onTimer()
{
	fillPage(ui->webView->page());
//	QWebPage *page = ui->webView->page();
//	qDebug() << page << contentScripts().count();
//	foreach (const QString &script, contentScripts())
//		page->mainFrame()->evaluateJavaScript(script);
//	QFile file(qApp->applicationDirPath() + "/index.html");
//	file.open(QFile::WriteOnly);
//	file.write(ui->webView->page()->mainFrame()->toHtml().toUtf8());
}

void Widget::on_comboBox_currentIndexChanged(const QString &variant)
{
    m_style.setActiveVariant(variant);
	ui->webView->page()->mainFrame()->evaluateJavaScript(m_style.scriptForChangingVariant());
}

void Widget::fillPage(QWebPage *page)
{
	QString baseHtml = m_style.baseTemplateForChat(m_preview->chat);
	page->setNetworkAccessManager(new WebKitNetworkAccessManager(page));
	
	page->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
	QWebInspector *inspector = new QWebInspector;
	inspector->setPage(page);
//	inspector->show();
	
//	QTimer::singleShot(10000, this, SLOT(onLoad()));
//	connect(page, SIGNAL(loadFinished(bool)), SLOT(onLoad()));
	connect(page, SIGNAL(loadFinished(bool)), SLOT(onLoad()));
	page->mainFrame()->setHtml(baseHtml);
}

static bool isContentSimiliar(const IContent &a, const IContent &b)
{
	if (a.source == b.source
	        && a.type == b.type
	        && a.isHistory == b.isHistory
	        && a.displayClasses.contains("mention") == b.displayClasses.contains("mention")) {
		return qAbs(a.date.secsTo(b.date)) < 300;
	}
	return false;
}

QStringList Widget::contentScripts()
{
	QStringList result;
	for (int i = 0; i < m_preview->contents.size(); ++i) {
		const IContent &content = m_preview->contents.at(i);
		bool similiar = i > 0 && isContentSimiliar(m_preview->contents.at(i - 1), content);
		result << m_style.scriptForAppendingContent(content, similiar, true, false);
	}
	return result;
}
