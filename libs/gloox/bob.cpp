/*
  Copyright (c) 2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#include "bob.h"
#include "base64.h"
#include "tag.h"

#include <cstdlib>

namespace gloox
{

  BOB::BOB( const Tag* tag )
    : StanzaExtension( ExtBOB )
  {
    if( !tag || tag->name() != "data" || tag->xmlns() != XMLNS_BOB )
      return;

    m_cid = tag->findAttribute( "cid" );
    m_maxage = strtol( tag->findAttribute( "max-age" ).c_str(), 0, 10 );
    m_type = tag->findAttribute( "type" );
    m_data = tag->cdata();
  }

  BOB::BOB( const std::string& cid, const std::string& type,
            const std::string& data, int maxage )
    : StanzaExtension( ExtBOB ), m_cid( cid ), m_type( type ),
      m_data( Base64::encode64( data ) ), m_maxage( maxage )
  {
  }

  const std::string BOB::data() const
  {
    return Base64::decode64( m_data );
  }

  const std::string& BOB::filterString() const
  {
    static const std::string filter = "/iq/data[@xmlns='" + XMLNS_BOB + "']"
        "|/presence/data[@xmlns='" + XMLNS_BOB + "']"
        "|/message/data[@xmlns='" + XMLNS_BOB + "']";
    return filter;
  }

  Tag* BOB::tag() const
  {
    if( m_cid.empty() || m_type.empty() )
      return 0;

    Tag* t = new Tag( "data" );
    t->setXmlns( XMLNS_BOB );
    t->addAttribute( "cid", m_cid );
    t->addAttribute( "type", m_type );
    t->addAttribute( "max-age", m_maxage );
    t->setCData( m_data );

    return t;
  }

}
