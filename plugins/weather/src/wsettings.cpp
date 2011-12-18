/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Belov Nikita <null@deltaz.org>
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

#include "wsettings.h"

WSettings::WSettings()
{
	ui.setupUi( this );

	m_networkManager = new QNetworkAccessManager();
	connect( m_networkManager, SIGNAL( finished( QNetworkReply * ) ), this, SLOT( searchFinished( QNetworkReply * ) ) );

	ui.cityEdit->installEventFilter( this );
	ui.searchEdit->installEventFilter( this );

	eventFilter( ui.cityEdit, new QFocusEvent( QEvent::FocusOut ) );
	eventFilter( ui.cityEdit, new QFocusEvent( QEvent::FocusOut ) );

	lookForWidgetState( ui.intervalBox );
	lookForWidgetState( ui.showStatusBox );
	lookForWidgetState( ui.useDefaultTheme );
	lookForWidgetState( ui.useCustomTheme );
	lookForWidgetState( ui.themePath );
}

WSettings::~WSettings()
{
}

void WSettings::loadImpl()
{
	Config mainGroup = Config( "weather" ).group( "main" );

	ui.intervalBox->setValue( mainGroup.value( "interval", 25 ) );
	ui.showStatusBox->setCheckState( ( mainGroup.value( "showStatus", true ) ? Qt::Checked : Qt::Unchecked ) );
	ui.useDefaultTheme->setChecked( mainGroup.value( "useDefaultTheme", true ) );
	ui.useCustomTheme->setChecked( !mainGroup.value( "useDefaultTheme", true ) );
	ui.themePath->setText( mainGroup.value( "themePath", QString() ) );

	for ( int i = 0, count = mainGroup.value( "countItems", 0 ); i < count; i++ )
	{
		WListItem *item = new WListItem( mainGroup.value( "item_" + QString::number( i ), QString() ), false );
		connect( item, SIGNAL( buttonClicked() ), this, SLOT( deleteButton_clicked() ) );

		ui.cityList->addItem( QString() );
		ui.cityList->setItemWidget( ui.cityList->item( i ), item );
		item->setItem( ui.cityList->item( i ) );
	}
}

void WSettings::saveImpl()
{
	Config mainGroup = Config( "weather" ).group( "main" );

	mainGroup.setValue( "interval", ui.intervalBox->value() );
	mainGroup.setValue( "showStatus", ( ui.showStatusBox->checkState() == Qt::Checked ? true : false ) );
	mainGroup.setValue( "countItems", ui.cityList->count() );
	mainGroup.setValue( "useDefaultTheme", ui.useDefaultTheme->isChecked() );
	mainGroup.setValue( "themePath", ui.themePath->text() );

	for ( int i = 0; i < ui.cityList->count(); i++ )
		mainGroup.setValue( "item_" + QString::number( i ), ( ( WListItem * )ui.cityList->itemWidget( ui.cityList->item( i ) ) )->title() );

	mainGroup.sync();
}

void WSettings::cancelImpl()
{
}

bool WSettings::eventFilter( QObject *o, QEvent *e )
{
	QLineEdit *l = ( QLineEdit *)o;

	if ( e->type() == QEvent::FocusIn )
	{
		if ( !l->styleSheet().isEmpty() )
		{
			l->setStyleSheet( QString() );
			l->setText( QString() );
		}
	}
	else if ( e->type() == QEvent::FocusOut )
	{
		if ( l->text().isEmpty() )
		{
			l->setStyleSheet( "color: rgb(130, 130, 130); font-style: italic;" );
			if ( l->objectName() == "cityEdit" )
				l->setText( QT_TRANSLATE_NOOP( "Weather", "Enter here city's code" ) );
			else if ( l->objectName() == "searchEdit" )
				l->setText( QT_TRANSLATE_NOOP( "Weather", "Enter here city's name" ) );
		}
	}

	return QObject::eventFilter( o, e );
}

void WSettings::on_addCityButton_clicked()
{
	if ( ui.cityEdit->text().isEmpty() )
		return;

	WListItem *item = new WListItem( ui.cityEdit->text(), false );
	connect( item, SIGNAL( buttonClicked() ), this, SLOT( deleteButton_clicked() ) );
	
	ui.cityList->addItem( QString() );
	ui.cityList->setItemWidget( ui.cityList->item( ui.cityList->count() -1 ), item );
	ui.cityEdit->setText( "" );
	item->setItem( ui.cityList->item( ui.cityList->count() - 1 ) );

	emit modifiedChanged( true );
}

void WSettings::on_searchButton_clicked()
{
	m_networkManager->get( QNetworkRequest( QUrl( QString( "http://xoap.weather.com/search/search?where=%1" ).arg( ui.searchEdit->text() ) ) ) );
	ui.searchResult->setText( QT_TRANSLATE_NOOP( "Weather", "Searching..." ) );
}

void WSettings::on_chooseButton_clicked()
{
	ui.themePath->setText( QFileDialog::getExistingDirectory( this, QT_TRANSLATE_NOOP( "Weather", "Select theme's directory" ), QDesktopServices::storageLocation( QDesktopServices::HomeLocation ), QFileDialog::ShowDirsOnly ) );
}

void WSettings::addButton_clicked()
{
	ui.cityEdit->setText( m_searchResults.value( ui.searchList->row( ( ( WListItem * )sender() )->item() ) ) );
	on_addCityButton_clicked();
	ui.tabWidget->setCurrentIndex( 0 );
}

void WSettings::deleteButton_clicked()
{
	delete ( ( WListItem * )sender() )->item();
	delete sender();

	emit modifiedChanged( true );
}

void WSettings::searchFinished( QNetworkReply *reply )
{
	for ( int i = 0; i < ui.searchList->count(); i++ )
	{
		ui.searchList->takeItem( 0 );
		delete ui.searchList->item( 0 );
	}
	m_searchResults.clear();

	QDomDocument doc;
	if( doc.setContent( reply->readAll() ) )
	{
		QDomElement rootElement = doc.documentElement();
		int itemsCount = rootElement.elementsByTagName( "loc" ).count();
		if ( itemsCount == 0 )
			ui.searchResult->setText( QT_TRANSLATE_NOOP( "Weather", "Not found" ) );
		else
			ui.searchResult->setText( QString( QT_TRANSLATE_NOOP( "Weather", "Found: %1" ) ).arg( QString::number( itemsCount ) ) );

		for ( int i = 0; i < itemsCount; i++ )
		{
			QString cityId = rootElement.elementsByTagName( "loc" ).at( i ).attributes().namedItem( "id" ).nodeValue();
			QString cityName = rootElement.elementsByTagName( "loc" ).at( i ).toElement().text();

			WListItem *item = new WListItem( cityName );
			connect( item, SIGNAL( buttonClicked() ), this, SLOT( addButton_clicked() ) );
			
			ui.searchList->addItem( QString() );
			item->setItem( ui.searchList->item( i ) );
			ui.searchList->setItemWidget( ui.searchList->item( i ), item );
			m_searchResults.insert( i, cityId );
		}
	}
}

