#ifndef LAYERSCITY_H
#define LAYERSCITY_H

#include <qutim/layerinterface.h>

namespace qutim_sdk_0_2 {

class LayersCity : public SystemsCity
{
public:
	inline static LayersCity &instance()
	{
		return *reinterpret_cast<LayersCity *>( &SystemsCity::instance() );
	}
	inline void setLayerInterface(LayerType type, LayerInterface *layer_interface)
	{
		if( type < InvalidLayer )
			m_layers[type] = layer_interface;
	}
	inline LayerInterface *layer( LayerType type ) { return type < InvalidLayer ? m_layers[type] : 0; }
	template< typename T >
	inline T *layer( LayerType type ) { return static_cast<T *>( layer( type ) ); }
	inline static LayerInterface *Layer( LayerType type ) { return instance().layer(type); }
	template< typename T >
	inline static T *Layer( LayerType type ) { return instance().layer<T>( type ); }
	/*
	 * Methods for getting pointers for layers' interfaces
	 * Just do for use:
	 * LayersCity &ls = LayersCity::instance();
	 * SoundEngineLayerInterface *sound_engine = ls.soundEngine();
	 * sound_engine->playSound("example.ogg");
	 */
	inline ContactListLayerInterface *contactList() { return static_cast<ContactListLayerInterface *>( m_layers[ContactListLayer] ); }
	inline ChatLayerInterface *chat() { return static_cast<ChatLayerInterface *>( m_layers[ChatLayer] ); }
	inline HistoryLayerInterface *history() { return static_cast<HistoryLayerInterface *>( m_layers[HistoryLayer] ); }
	inline NotificationLayerInterface *notification() { return static_cast<NotificationLayerInterface *>( m_layers[NotificationLayer] ); }
	inline AntiSpamLayerInterface *antiSpam() { return static_cast<AntiSpamLayerInterface *>( m_layers[AntiSpamLayer] ); }
	inline SoundEngineLayerInterface *soundEngine() { return static_cast<SoundEngineLayerInterface *>( m_layers[SoundEngineLayer] ); }
	inline VideoEngineLayerInterface *videoEngine() { return static_cast<VideoEngineLayerInterface *>( m_layers[VideoEngineLayer] ); }
	inline StatusLayerInterface *status() { return static_cast<StatusLayerInterface *>( m_layers[StatusLayer] ); }
	inline TrayLayerInterface *tray() { return static_cast<TrayLayerInterface *>( m_layers[TrayLayer] ); }
	inline EmoticonsLayerInterface *emoticons() { return static_cast<EmoticonsLayerInterface *>( m_layers[EmoticonsLayer] ); }
	inline SpellerLayerInterface *speller() { return static_cast<SpellerLayerInterface *>( m_layers[SpellerLayer] ); }
	/*
	 * And their static analogs
	 * Just do for use:
	 * LayersCity::SoundEngine()->playSound("example.ogg");
	 */
	inline static ContactListLayerInterface *ContactList() { return instance().contactList(); }
	inline static ChatLayerInterface *Chat() { return instance().chat(); }
	inline static HistoryLayerInterface *History() { return instance().history(); }
	inline static NotificationLayerInterface *Notification() { return instance().notification(); }
	inline static AntiSpamLayerInterface *AntiSpam() { return instance().antiSpam(); }
	inline static SoundEngineLayerInterface *SoundEngine() { return instance().soundEngine(); }
	inline static VideoEngineLayerInterface *VideoEngine() { return instance().videoEngine(); }
	inline static StatusLayerInterface *Status() { return instance().status(); }
	inline static TrayLayerInterface *Tray() { return instance().tray(); }
	inline static EmoticonsLayerInterface *Emoticons() { return instance().emoticons(); }
	inline static SpellerLayerInterface *Speller() { return instance().speller(); }
protected:
	inline LayersCity() : SystemsCity() {}
	inline ~LayersCity() {}
};

}

#endif // LAYERSCITY_H
