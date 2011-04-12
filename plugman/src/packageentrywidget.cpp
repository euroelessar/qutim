/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "packageentrywidget.h"
#include <qutim/icon.h>
#include <QGridLayout>
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
	QGridLayout *layout = new QGridLayout(this);
//	layout->setMargin(4);
	m_previewLabel = new QLabel(this);
//	m_previewLabel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
//	m_previewLabel->resize(PreviewWidth, PreviewHeight);
	m_previewLabel->setMinimumSize(PreviewWidth, PreviewHeight);
	m_previewLabel->setMaximumSize(PreviewWidth, PreviewHeight);
	m_detailsLabel = new QLabel(this);
	m_detailsLabel->setTextFormat(Qt::RichText);
	m_installButton = new QPushButton(this);
	layout->addWidget(m_previewLabel,  0, 0, 1,  1, Qt::AlignCenter);
	layout->addWidget(m_detailsLabel,  0, 1, 1,  1, Qt::AlignLeft | Qt::AlignTop);
	layout->addWidget(m_installButton, 0, 2, 1,  1, Qt::AlignRight | Qt::AlignTop);
	updateData();
}

void PackageEntryWidget::updateData()
{
	const Attica::Content content = m_entry.content();
	const QPixmap * const pixmap = m_previewLabel->pixmap();
	
	if (!m_entry.smallPreview().isNull()) {
		if (!pixmap || pixmap->cacheKey() != m_entry.smallPreview().cacheKey())
			m_previewLabel->setPixmap(m_entry.smallPreview());
	}
	
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
	info += QLatin1Literal("<p>") % content.summary() % QLatin1Literal("</p>");
	m_detailsLabel->setText(info);
	
	QString text;
	QIcon icon;
	bool enabled = true;
	
	switch (m_entry.status()) {
	default:
		// Fall through
		// break;
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
