/*
  Copyright (c) 2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef JINGLEDTMF_H__
#define JINGLEDTMF_H__

#include "jingleplugin.h"
#include "macros.h"

#include <string>

namespace gloox
{

  class Tag;

  namespace Jingle
  {

    /**
     * @brief An abstraction of a Jingle DTMF element, of XEP-0181.
     *
     * @author Jakob Schroeter <js@camaya.net>
     * @since 1.0
     */
    class GLOOX_API DTMF : public Plugin
    {
      public:
        /**
         * Constructs a new DTMF object.
         * @param code The DTMF code to generate.
         * @param volume The volume, ranges from 0 to -63 dBm0.
         * Thus, a larger value denotes a lower volume.
         * @param duration The duration if the tone, in ms, defaults to 100ms.
         */
        DTMF( const std::string& code, int volume, int duration = 100 );

        /**
         * Constructs a new DTMF object from the given Tag.
         * @param tag The Tag to parse.
         */
        DTMF( const Tag* tag );

        /**
         * Virtual destructor.
         */
        virtual ~DTMF() {}

        /**
         * Returns the DTMF tone to generate.
         * @return The DTMF tone to generate.
         */
        const std::string& code() const { return m_code; }

        /**
         * Returns the volume, ranges from 0 to -63 dBm0. Thus, a larger value denotes a lower volume.
         * @return The volume.
         */
        int volume() const { return m_volume; }

        /**
         * Returns the duration of the tone.
         * @return The duration of the tone.
         */
        int duration() const { return m_duration; }

        // reimplemented from Plugin
        virtual const std::string& filterString() const;

        // reimplemented from Plugin
        virtual Tag* tag() const;

        // reimplemented from Plugin
        virtual DTMF* clone() const
        {
          return new DTMF( *this );
        }

      private:
        std::string m_code;
        int m_volume;
        int m_duration;

    };

  }

}

#endif // JINGLEDTMF_H__
