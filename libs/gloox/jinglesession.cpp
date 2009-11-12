/*
  Copyright (c) 2007-2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "jinglesession.h"

#include "clientbase.h"
#include "error.h"
#include "jinglecontent.h"
#include "jingledescription.h"
#include "jingledtmf.h"
#include "jingletransport.h"
#include "jinglesessionhandler.h"
#include "tag.h"
#include "util.h"

namespace gloox
{

  namespace Jingle
  {

    static const char* actionValues [] = {
      "content-accept",
      "content-add",
      "content-modify",
      "content-reject",
      "content-remove",
      "description-info",
      "session-accept",
      "session-info",
      "session-initiate",
      "session-terminate",
      "transport-accept",
      "transport-info",
      "transport-reject",
      "transport-replace"
    };

    static inline Action actionType( const std::string& type )
    {
      return (Action)util::lookup( type, actionValues );
    }

    // ---- Session::Reason ----
    static const char* reasonValues [] = {
      "alternative-session",
      "busy",
      "cancel",
      "connectivity-error",
      "decline",
      "expired",
      "general-error",
      "gone",
      "media-error",
      "security-error",
      "success",
      "timeout",
      "unsupported-applications",
      "unsupported-transports"
    };

    static inline Session::Reason::Reasons reasonType( const std::string& type )
    {
      return (Session::Reason::Reasons)util::lookup( type, reasonValues );
    }

    Session::Reason::Reason( Reasons reason,
                             const std::string& sid,
                             const std::string& text)
      : m_reason( reason ), m_sid( sid ), m_text( text )
    {
    }

    Session::Reason::Reason( const Tag* tag )
    {
      if( !tag || tag->name() != "reason" )
        return;

      const TagList& l = tag->children();
      TagList::const_iterator it = l.begin();
      for( ; it != l.end(); ++it )
      {
        if( (*it)->name() == "text" )
          m_text = (*it)->cdata();
        else if( (*it)->xmlns() == XMLNS_JINGLE )
          m_reason = reasonType( (*it)->name() );
      }
    }

    const std::string& Session::Reason::filterString() const
    {
      static const std::string filter = "reason";
      return filter;
    }

    Tag* Session::Reason::tag() const
    {
      if( m_reason == InvalidReason )
        return 0;

      Tag* t = new Tag( "reason" );
      Tag* r = new Tag( t, util::lookup( m_reason, reasonValues ) );
      if( m_reason == AlternativeSession && !m_sid.empty() )
        new Tag( r, "sid", m_sid );

      if( !m_text.empty() )
        new Tag( t, "text", m_text );

      return t;
    }

    Plugin* Session::Reason::clone() const
    {
      return new Reason( *this );
    }
    // ---- ~Session::Reason ----

    // ---- Session::Jingle ----
    Session::Jingle::Jingle( Action action, const JID& initiator,
                             const PluginList& plugins, const std::string& sid )
      : StanzaExtension( ExtJingle ), m_action( action ), m_sid( sid ),
        m_initiator( initiator ), m_plugins( plugins )
    {
    }

    Session::Jingle::Jingle( Action action, const JID& initiator,
                             const Plugin* plugin, const std::string& sid )
      : StanzaExtension( ExtJingle ), m_action( action ), m_sid( sid ),
        m_initiator( initiator )
    {
      if( plugin )
        m_plugins.push_back( plugin );
    }

    Session::Jingle::Jingle( const Tag* tag )
      : StanzaExtension( ExtJingle ), m_action( InvalidAction )
    {
      if( !tag || tag->name() != "jingle" )
        return;

      m_action = actionType( tag->findAttribute( "action" ) );
      m_initiator.setJID( tag->findAttribute( "initiator" ) );
      m_responder.setJID( tag->findAttribute( "responder" ) );
      m_sid = tag->findAttribute( "sid" );

      const TagList& l = tag->children();
      TagList::const_iterator it = l.begin();
      for( ; it != l.end(); ++it )
      {
        const std::string& name = (*it)->name();
        if( name == "content" )
          m_plugins.push_back( new Content( (*it) ) );
        else if( name == "dtmf" && (*it)->xmlns() == XMLNS_JINGLE_DTMF )
          m_plugins.push_back( new DTMF( (*it) ) );
      }
    }

    Session::Jingle::Jingle( const Jingle& right )
      : StanzaExtension( ExtJingle ), m_action( right.m_action ),
        m_sid( right.m_sid ), m_initiator( right.m_initiator ),
        m_responder( right.m_responder )
    {
      PluginList::const_iterator it = right.m_plugins.begin();
      for( ; it != right.m_plugins.end(); ++it )
        m_plugins.push_back( (*it)->clone() );
    }

    Session::Jingle::~Jingle()
    {
      util::clearList( m_plugins );
    }

    const std::string& Session::Jingle::filterString() const
    {
      static const std::string filter = "/iq/jingle[@xmlns='" + XMLNS_JINGLE + "']";
      return filter;
    }

    Tag* Session::Jingle::tag() const
    {
      if( m_action == InvalidAction || m_sid.empty() || !m_initiator )
        return 0;

      Tag* t = new Tag( "jingle" );
      t->setXmlns( XMLNS_JINGLE );
      t->addAttribute( "action", util::lookup( m_action, actionValues ) );
      t->addAttribute( "initiator", m_initiator );
      if( m_responder )
        t->addAttribute( "responder", m_responder );
      t->addAttribute( "sid", m_sid );

      PluginList::const_iterator it = m_plugins.begin();
      for( ; it != m_plugins.end(); ++it )
        t->addChild( (*it)->tag() );

      return t;
    }

    StanzaExtension* Session::Jingle::clone() const
    {
      return new Jingle( *this );
    }
    // ---- ~Session::Jingle ----

    // ---- Session ----
    Session::Session( ClientBase* parent, const JID& callee, SessionHandler* jsh )
      : m_parent( parent ), m_state( Ended ), m_callee( callee ),
        m_handler( jsh ), m_valid( false )
    {
      if( !m_parent || !m_handler || !m_callee )
        return;

      m_initiator = m_parent->jid();

      m_parent->registerStanzaExtension( new Jingle() );

      m_valid = true;
    }

    Session::Session( ClientBase* parent, const Session::Jingle* jingle, SessionHandler* jsh )
      : m_parent( parent ), m_state( Ended ), m_handler( jsh ), m_valid( false )
    {
      if( !m_parent || !m_handler || !jingle || jingle->action() != SessionInitiate )
        return;

      m_callee = m_parent->jid();
      m_initiator = jingle->initiator();
      m_state = Pending;
      m_sid = jingle->sid();

      m_valid = true;
    }

    Session::~Session()
    {
      if( m_parent )
        m_parent->removeIDHandler( this );
    }

    bool Session::initiate( const PluginList& plugins )
    {
      if( !m_valid || !m_parent || !plugins.empty() || !m_initiator
          || m_state >= Pending )
        return false;

      m_state = Pending;
      IQ init( IQ::Set, m_callee, m_parent->getID() );
      init.addExtension( new Jingle( SessionInitiate, m_initiator, plugins, m_sid ) );
      m_parent->send( init, this, SessionInitiate );

      return true;
    }

    bool Session::accept( const Content* content )
    {
      if( !m_valid || !m_parent || !content || !m_initiator
          || m_state > Pending )
        return false;

      m_state = Active;
      IQ init( IQ::Set, m_callee, m_parent->getID() );
      init.addExtension( new Jingle( SessionAccept, m_initiator, content, m_sid ) );
      m_parent->send( init, this, SessionAccept );

      return true;
    }

    bool Session::inform( Action action, const Plugin* plugin )
    {
      if( !m_valid || !m_parent || m_state < Pending || !m_initiator
          || action != DescriptionInfo || action != SessionInfo
          || action != TransportInfo )
        return false;

      IQ init( IQ::Set, m_callee, m_parent->getID() );
      init.addExtension( new Jingle( action, m_initiator, plugin, m_sid ) );
      m_parent->send( init, this, action );

      return true;
    }

    bool Session::terminate( Session::Reason* reason )
    {
      if( !m_valid || !m_parent || m_state < Pending || !m_initiator )
        return false;

      m_state = Ended;

      IQ init( IQ::Set, m_callee, m_parent->getID() );
      init.addExtension( new Jingle( SessionTerminate, m_initiator, reason, m_sid ) );
      m_parent->send( init, this, SessionTerminate );

      return true;
    }

    bool Session::handleIq( const IQ& iq )
    {
      const Jingle* j = iq.findExtension<Jingle>( ExtJingle );
      if( !j || j->sid() != m_sid || !m_handler )
        return false;

      switch( j->action() )
      {
        case ContentAccept:
          break;
        case ContentAdd:
          break;
        case ContentModify:
          break;
        case ContentReject:
          break;
        case ContentRemove:
          break;
        case DescriptionInfo:
          break;
        case SessionAccept:
        {
          m_state = Active;
          m_handler->handleSessionStateChange( this, j );
          IQ re( IQ::Result, iq.from(), iq.id() );
          m_parent->send( re );
          break;
        }
        case SessionInfo:
        {
          m_handler->handleSessionInfo( this, j );
          IQ re( IQ::Result, iq.from(), iq.id() );
          m_parent->send( re );
          break;
        }
        case SessionInitiate:
        {
          IQ re( IQ::Error, iq.from(), iq.id() );
          Tag* e = new Tag( "unknown-session" );
          e->setXmlns( XMLNS_JINGLE_ERROR );
          re.addExtension( new Error( StanzaErrorTypeCancel, StanzaErrorItemNotFound, e) );
          m_parent->send( re );
          break;
        }
        case SessionTerminate:
        {
          m_state = Ended;
          m_handler->handleSessionStateChange( this, j );
          IQ re( IQ::Result, iq.from(), iq.id() );
          m_parent->send( re );
          break;
        }
        case TransportAccept:
          break;
        case TransportInfo:
        {
          m_handler->handleTransportInfo( this, j );
          IQ re( IQ::Result, iq.from(), iq.id() );
          m_parent->send( re );
          break;
        }
        case TransportReject:
          break;
        case TransportReplace:
          break;
        case InvalidAction:
          break;
      }

      return true;
    }

    void Session::handleIqID( const IQ& iq, int context )
    {
      if( iq.subtype() == IQ::Error )
      {
        switch( context )
        {
          case ContentAccept:
            break;
          case ContentAdd:
            break;
          case ContentModify:
            break;
          case ContentReject:
            break;
          case ContentRemove:
            break;
          case DescriptionInfo:
            break;
          case SessionAccept:
            break;
          case SessionInfo:
            break;
          case SessionInitiate:
            break;
          case SessionTerminate:
            break;
          case TransportAccept:
            break;
          case TransportInfo:
            break;
          case TransportReject:
            break;
          case TransportReplace:
            break;
          case InvalidAction:
            break;
          default:
            break;
        }
      }
    }

  }

}
