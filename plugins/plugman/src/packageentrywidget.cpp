/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "packageentrywidget.h"
#include <qutim/icon.h>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QStringBuilder>

using namespace qutim_sdk_0_3;

// From KNewStuff
enum {
	PreviewWidth  = 96,
	PreviewHeight = 72
};

PackageEntryWidget::PackageEntryWidget(const PackageEntry &entry)
    : m_entry(entry)
{
//	QGridLayout *layout = new QGridLayout(this);
	QHBoxLayout *layout = new QHBoxLayout(this);
	QVBoxLayout *buttonsLayout = new QVBoxLayout();
	connect(this, SIGNAL(destroyed()), buttonsLayout, SLOT(deleteLater()));
	setLayout(layout);
//	layout->setMargin(4);
	m_previewLabel = new PackagePreviewWidget(this);
//	m_previewLabel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
//	m_previewLabel->resize(PreviewWidth, PreviewHeight);
//	m_previewLabel->setMinimumSize(PreviewWidth, PreviewHeight);
//	m_previewLabel->setMaximumSize(PreviewWidth, PreviewHeight);
	m_previewLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_detailsLabel = new QLabel(this);
	m_detailsLabel->setWordWrap(true);
	m_detailsLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	m_detailsLabel->setTextFormat(Qt::RichText);
	m_installButton = new QPushButton(this);
	m_installButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	connect(m_installButton, SIGNAL(clicked()), SIGNAL(buttonClicked()));
	layout->addWidget(m_previewLabel);
	layout->addWidget(m_detailsLabel, 10);
	layout->addLayout(buttonsLayout);
	buttonsLayout->addWidget(m_installButton);
//	layout->setColumnStretch(1, 10);
//	layout->addWidget(m_previewLabel,  0, 0, 1,  1, Qt::AlignLeft | Qt::AlignTop);
//	layout->addWidget(m_detailsLabel,  0, 1, 1,  1, Qt::AlignLeft | Qt::AlignTop);
//	layout->addWidget(m_installButton, 0, 2, 1,  1, Qt::AlignRight | Qt::AlignTop);
	updateData();
}

void PackageEntryWidget::updateData()
{
	const Attica::Content content = m_entry.content();
//	const QPixmap * const pixmap = m_previewLabel->pixmap();
	
//	if (!m_entry.smallPreview().isNull()) {
//		if (!pixmap || pixmap->cacheKey() != m_entry.smallPreview().cacheKey())
//			m_previewLabel->setPixmap(m_entry.smallPreview());
//	}
	m_previewLabel->setPreview(m_entry.smallPreview());
	
	QString info;
	const QUrl url = content.detailpage();
	if (url.isEmpty()) {
		info += QLatin1Literal("<p><a href=\"") % QLatin1String(url.toEncoded())
		        % QLatin1Literal("\">") % content.name() % QLatin1Literal("</a></p>");
	} else {
		info += QLatin1Literal("<p><b>") % content.name() % QLatin1Literal("</b></p>");
	}
	
	const QString authorName = content.author();
	const QString authorEmail = content.attribute(QLatin1String("email"));
	const QString authorPage = content.attribute(QLatin1String("profilepage"));
	
	info += QLatin1String("<p>");
	if (!authorPage.isEmpty()) {
		info += tr("By <i>%1</i>").arg(QLatin1Literal("<a href=\"") % authorPage % QLatin1Literal("\">")
		                               % authorName % QLatin1Literal("</a>"));
	} else if (!authorEmail.isEmpty()) {
		info += tr("By <i>%1</i>").arg(authorName) % QLatin1Literal("<a href=\"mailto")
		        % authorEmail % QLatin1Literal("\">") % authorEmail % QLatin1Literal("</a>");
	} else {
		info += tr("By <i>%1</i>").arg(QLatin1Literal("<a href=\"") % authorPage % QLatin1Literal("\">")
		                               % authorName % QLatin1Literal("</a>"));
	}
	info += QLatin1String("</p>");
	info += QLatin1Literal("<p>") % content.description() % QLatin1Literal("</p>");
	m_detailsLabel->setText(info);
	
	QString text;
	QIcon icon;
	bool enabled = true;
	
	switch (m_entry.status()) {
	default:
		text = QString("Invalid - %1").arg(m_entry.status());
		break;
	case PackageEntry::Installable:
		text = tr("Install");
		icon = Icon(QLatin1String("dialog-ok"));
		break;
	case PackageEntry::Updateable:
		text = tr("Update");
		icon = Icon(QLatin1String("system-software-update"));
		break;
	case PackageEntry::Installing:
		text = tr("Installing");
		icon = Icon(QLatin1String("dialog-ok"));
		enabled = false;
		break;
	case PackageEntry::Updating:
		text = tr("Updating");
		icon = Icon(QLatin1String("dialog-ok"));
		enabled = false;
		break;
	case PackageEntry::Installed:
		text = tr("Uninstall");
		icon = Icon(QLatin1String("edit-delete"));
		break;
	}
	
	m_installButton->setText(text);
	m_installButton->setIcon(icon);
	m_installButton->setEnabled(enabled);
}

