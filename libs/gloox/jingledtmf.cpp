/*
  Copyright (c) 2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "jingledtmf.h"
#include "gloox.h"
#include "tag.h"

#include <cstdlib>

namespace gloox
{

  namespace Jingle
  {

    DTMF::DTMF( const std::string& code, int volume, int duration )
      : m_code( code ), m_volume( volume ), m_duration( duration )
    {
    }

    DTMF::DTMF( const Tag* tag )
      : m_volume( 63 ), m_duration( 0 )
    {
      if( !tag || tag->name() != "dtmf" || tag->xmlns() != XMLNS_JINGLE_DTMF )
        return;

      m_code = tag->findAttribute( "code" );
      m_volume = strtol( tag->findAttribute( "volume" ).c_str(), 0, 10 );
      m_duration = strtol( tag->findAttribute( "duration" ).c_str(), 0, 10 );
    }

    const std::string& DTMF::filterString() const
    {
      static const std::string filter = "dtmf[@xmlns='" + XMLNS_JINGLE_DTMF + "']";
      return filter;
    }

    Tag* DTMF::tag() const
    {
      if( m_code.empty() )
        return 0;

      Tag* t = new Tag( "dtmf" );
      t->setXmlns( XMLNS_JINGLE_DTMF );
      t->addAttribute( "code", m_code.substr( 0, 1 ) );
      t->addAttribute( "volume", m_volume );
      t->addAttribute( "duration", m_duration );

      return t;
    }

  }

}
