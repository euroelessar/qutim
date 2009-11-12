/*
  Copyright (c) 2008-2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "jinglecontent.h"
#include "jingledescription.h"
#include "jingletransport.h"
#include "util.h"

namespace gloox
{

  namespace Jingle
  {

    static const char* creatorValues [] = {
      "initiator",
      "responder"
    };

    static inline Content::Creator creatorType( const std::string& type )
    {
      return (Content::Creator)util::lookup( type, creatorValues );
    }

    static const char* sendersValues [] = {
      "initiator",
      "responder",
      "both"
    };

    static inline Content::Senders sendersType( const std::string& type )
    {
      return (Content::Senders)util::lookup( type, sendersValues );
    }

    Content::Content( Description* desc, Transport* trans,
                      const std::string& name, Creator creator,
                      Senders senders, const std::string& disposition )
      : m_description( desc ), m_transport( trans ),
        m_creator( creator ), m_disposition( disposition ), m_name( name ), m_senders( senders )
    {
    }

    Content::Content( const Tag* tag )
    {

    }

    Content::~Content()
    {
    }

    const std::string& Content::filterString() const
    {
      static const std::string filter = "content";
      return filter;
    }

    Tag* Content::tag() const
    {
      if( m_creator == InvalidCreator || m_name.empty() )
        return 0;

      Tag* t = new Tag( "content" );
      t->addAttribute( "creator", util::lookup( m_creator, creatorValues ) );
      t->addAttribute( "disposition", m_disposition );
      t->addAttribute( "name", m_name );
      t->addAttribute( "senders", util::lookup( m_senders, sendersValues ) );

      if( m_description )
        t->addChild( m_description->tag() );
      if( m_transport )
        t->addChild( m_transport->tag() );

      return t;
    }

    Plugin* Content::clone() const
    {
      return 0;
    }

  }

}
