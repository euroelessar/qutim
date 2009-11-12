/*
  Copyright (c) 2007-2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef JINGLESESSION_H__
#define JINGLESESSION_H__

#include "stanzaextension.h"
#include "tag.h"
#include "iqhandler.h"
#include "jingleplugin.h"

#include <string>

namespace gloox
{

  class ClientBase;

  /**
   * @brief The namespace containing Jingle-related (XEP-0166 et. al.) classes.
   *
   * See @link gloox::Jingle::Session Session @endlink for more information
   * about Jingle in gloox.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 1.1
   */
  namespace Jingle
  {

    class Description;
    class Transport;
    class SessionHandler;
    class Content;

    /**
     * A list of Jingle Plugins.
     */
    typedef std::list<const Plugin*> PluginList;

    /**
     * Jingle Session actions.
     */
    enum Action
    {
      ContentAccept,                /**< Accept a content-add action received from another party. */
      ContentAdd,                   /**< Add one or more new content definitions to the session. */
      ContentModify,                /**< Change the directionality of media sending. */
      ContentReject,                /**< Reject a content-add action received from another party. */
      ContentRemove,                /**< Remove one or more content definitions from the session. */
      DescriptionInfo,              /**< Exchange information about parameters for an application type. */
      SessionAccept,                /**< Definitively accept a session negotiation. */
      SessionInfo,                  /**< Send session-level information, such as a ping or a ringing message. */
      SessionInitiate,              /**< Request negotiation of a new Jingle session. */
      SessionTerminate,             /**< End an existing session. */
      TransportAccept,              /**< Accept a transport-replace action received from another party. */
      TransportInfo,                /**< Exchange transport candidates. */
      TransportReject,              /**< Reject a transport-replace action received from another party. */
      TransportReplace,             /**< Redefine a transport method or replace it with a different method. */
      InvalidAction                 /**< Invalid action. */
    };

    /**
     * @brief This is an implementation of a Jingle Session (XEP-0166).
     *
     * Beware! The classes in the Jingle namespace implement the signaling part of Jingle only.
     * At this point, there is no support for actually establishing any connection to a remote entity,
     * nor for transfering any media in any way whatsoever.
     *
     * XEP Version: 0.33
     * @author Jakob Schroeter <js@camaya.net>
     * @since 1.1
     */
    class GLOOX_API Session : public IqHandler
    {

      public:
        /**
         * Session state.
         */
        enum State
        {
          Ended,                    /**< The session has ended or was not active yet. */
          Pending,                  /**< The session has been initiated but has not yet been accepted by the remote party. */
          Active                    /**< The session is empty. */
        };

        /**
         * @brief An abstraction of a terminate reason.
         *
         * XEP Version: 0.33
         * @author Jakob Schroeter <js@camaya.net>
         * @since 1.1
         */
        class GLOOX_API Reason : public Plugin
        {
          public:
            /**
             * Defied reasons for terminating a Jingle Session.
             */
            enum Reasons
            {
              AlternativeSession,   /**< An alternative session exists that should be used. */
              Busy,                 /**< The terminating party is busy. */
              Cancel,               /**< The session has been canceled. */
              ConnectivityError,    /**< Connectivity error. */
              Decline,              /**< The terminating party formally declines the request. */
              Expired,              /**< The session has expired. */
              GeneralError,         /**< General error. */
              Gone,                 /**< Participant went away. */
              MediaError,           /**< Media error. */
              SecurityError,        /**< Security error. */
              Success,              /**< Session terminated after successful call. */
              Timeout,              /**< A timeout occured. */
              UnsupportedApplications,/**< The terminating party does not support any of the offered application formats. */
              UnsupportedTransports,/**< The terminating party does not support any of the offered transport methods. */
              InvalidReason         /**< Invalid reason. */
            };

            /**
             * Constructor.
             * @param reason The reason for the termination of the session.
             * @param sid An optional session ID (only used if reason is AlternativeSession).
             * @param text An optional human-readable text explaining the reason for the session termination.
             */
            Reason( Reasons reason, const std::string& sid = EmptyString,
                    const std::string& text = EmptyString );

            /**
             * Constructs a new element by parsing the given Tag.
             * @param tag A tag to parse.
             */
            Reason( const Tag* tag = 0 );

            /**
             * Virtual destructor.
             */
            virtual ~Reason() {}

            /**
             * Returns the reason for the session termination.
             * @return The reason for the session termination.
             */
            Reasons reason() const { return m_reason; }

            /**
             * Returns the session ID of the alternate session, if given (only applicable
             * if reason() returns AlternativeSession).
             * @return The session ID of the alternative session, or the empty string.
             */
            const std::string& sid() const { return m_sid; }

            /**
             * Returns the content of an optional, human-readable
             * &lt;text&gt; element.
             * @return An optional text describing the reason for the action.
             */
            const std::string& text() const { return m_text; }

            // reimplemented from Jingle::Plugin
            virtual const std::string& filterString() const;

            // reimplemented from Jingle::Plugin
            virtual Tag* tag() const;

            // reimplemented from Jingle::Plugin
            virtual Plugin* clone() const;

          private:
            Reasons m_reason;
            std::string m_sid;
            std::string m_text;

        };

        /**
         * @brief This is an abstraction of the XEP-0166 (Jingle) &lt;jingle&gt; element as a StanzaExtension.
         *
         * XEP Version: 0.33
         * @author Jakob Schroeter <js@camaya.net>
         * @since 1.0
         */
        class Jingle : public StanzaExtension
        {

          friend class Session;

          public:
            /**
             * Constructs a new object from the given Tag.
             * @param tag The Tag to parse.
             */
            Jingle( const Tag* tag = 0 );

            /**
             * Virtual Destructor.
             */
            virtual ~Jingle();

            /**
             * Returns the session ID.
             * @return The session ID.
             */
            const std::string& sid() const { return m_sid; }

            /**
             *
             */
            void setInitiator( const JID& jid ) { m_initiator = jid; }

            /**
             * Returns the initiator.
             * @return The initiator.
             */
            const JID& initiator() const { return m_initiator; }

            /**
             *
             */
            void setResponder( const std::string& responder ) { m_responder = responder; }

            /**
             *
             */
            Action action() const { return m_action; }

            // reimplemented from StanzaExtension
            virtual const std::string& filterString() const;

            // reimplemented from StanzaExtension
            virtual StanzaExtension* newInstance( const Tag* tag ) const
            {
              return new Jingle( tag );
            }

            // reimplemented from StanzaExtension
            virtual Tag* tag() const;

            // reimplemented from StanzaExtension
            virtual StanzaExtension* clone() const;

#ifdef JINGLE_TEST
          public:
#else
          private:
#endif
            /**
             * Constructs a new object and fills it according to the parameters.
             * @param action The Action to carry out.
             * @param initiator The full JID initiator of the session flow.
             * @param plugins A list of contents (plugins) for the &lt;jingle&gt;
             * element. Usually, this will be Content objects.
             * @param sid The session ID:
             */
            Jingle( Action action, const JID& initiator,
                    const PluginList& plugins, const std::string& sid );

            /**
             * Constructs a new object and fills it according to the parameters.
             * @param action The Action to carry out.
             * @param initiator The full JID initiator of the session flow.
             * @param plugin A single content (plugin) for the &lt;jingle&gt;
             * element. This may be Content object, but can be any Plugin-derived object.
             * @param sid The session ID:
             */
            Jingle( Action action, const JID& initiator,
                    const Plugin* plugin, const std::string& sid );

            /**
             * Copy constructor.
             * @param right The instance to copy.
             */
            Jingle( const Jingle& right );

            Action m_action;
            std::string m_sid;
            JID m_initiator;
            JID m_responder;
            PluginList m_plugins;

        };

        /**
         * Creates a new Jingle Session.
         * @param parent The ClientBase to use for communication.
         * @param callee The remote end of the session.
         * @param jsh The handler to receive events and results.
         */
        Session( ClientBase* parent, const JID& callee, SessionHandler* jsh );

        /**
         * Creates a new Session from the incoming Jingle object.
         * This is a NOOP for Jingles that have an action() different from SessionInitiate.
         * @param param The ClientBase to use for communication.
         * @param jingle The Jingle object to init the Session from.
         * @param jsh The handler to receive events and results.
         */
        Session( ClientBase* parent, const Session::Jingle* jingle,
                 SessionHandler* jsh );

        /**
         * Virtual Destructor.
         */
        virtual ~Session();

        /**
         * Initiates a session with a remote entity.
         * @param plugins A list of Content-derived objects.
         * @return @b False if a prerequisite is not met, @b true otherwise.
         */
        bool initiate( const PluginList& plugins );

        /**
         * Accepts an incoming session with the given content.
         * @param content A pair of Description and Transport that describe the accepted session
         * parameters.
         * @return @b False if a prerequisite is not met, @b true otherwise.
         */
        bool accept( const Content* content );

        /**
         * Sends an informational message (DescriptionInfo,
         * TransportInfo, SessionInfo) to the remote party.
         * @param action The type of message to send.
         * @param plugin The payload. May be 0.
         * @return @b False if a prerequisite is not met, @b true otherwise.
         */
        bool inform( Action action, const Plugin* plugin );

        /**
         * Terminates the current session, if it is at least in Pending state, with the given reason. The sid parameter is ignored unless the reason is AlternativeSession.
         * @param reason The reason for terminating the session.
         * @return @b False if a prerequisite is not met, @b true otherwise.
         */
        bool terminate( Reason* reason );

        /**
         * Returns the session's state.
         * @return The session's state.
         */
        State state() const { return m_state; }

        /**
         * Returns the session's ID.
         * @return The session's ID.
         */
        const std::string& sid() const { return m_sid; }

        // reimplemented from IqHandler
        virtual bool handleIq( const IQ& iq );

        // reimplemented from IqHandler
        virtual void handleIqID( const IQ& iq, int context );

#ifdef JINGLE_TEST
      public:
#else
      private:
#endif
        ClientBase* m_parent;
        State m_state;
        JID m_callee;
        JID m_initiator;
        SessionHandler* m_handler;
        std::string m_sid;
        bool m_valid;

    };

  }

}

#endif // JINGLESESSION_H__
