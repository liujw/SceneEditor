//MIDIOut.cpp

#include "MIDIOut.h"
#ifndef WINCE
#ifdef IS_NEED_CODE_MIDI

#include <Windows.h>
#include <dmusici.h>
#include "../importInclude/mmTimer.h" 


void*  public_MIDIOut_hWnd=0;

void CMIDIOut::setWinHandle(void* wHandle){
	public_MIDIOut_hWnd=wHandle;
}


////////////

#define MIDI_NULL     0   // this midi object is not loaded
#define MIDI_LOADED   1   // this midi object is loaded
#define MIDI_TRY_PLAY 2   // this midi object is loaded and to playing
#define MIDI_PLAYING  3   // this midi object is playing
#define MIDI_STOPPED  4   // this midi object is loaded, but stopped

class CImportMIDIOut;
class CMidiBuf:public ISoundMIDIBuffer
{
private:
	CImportMIDIOut*             m_Owner;
	IDirectMusicLoader*			m_dm_loader;
	IDirectMusicPerformance*	m_dm_perf;
	IDirectMusicSegment*        dm_segment; 
	IDirectMusicSegmentState*   dm_segstate;
	int                         state; 
	IRandInputStream*			m_inputStream;
	long                        m_streamPos0;
	void*						m_ID;       

    bool				m_is_loop;
	long                m_loopCount;
	long                m_PlayStartTime_ms;
	long				m_try_play_time_ms;
	void MIDI_Stop();
	bool MIDI_Play();
	bool MIDI_Load(unsigned char* MIDI_data,unsigned int MIDI_Data_size);
	void MIDI_Free();
public:
	CMidiBuf(CImportMIDIOut* Owner);
	virtual ~CMidiBuf();
	virtual void init(RandInputStream* inputStream);
	virtual void* getID() { return m_ID; }
	virtual void setID(void* ID) { m_ID=ID; };
	virtual void play(bool is_loop_play,long loopCount,long play_time_ms);
	virtual void stop() { MIDI_Stop(); }
	virtual bool is_paly_finished();


	inline unsigned char* streamRead(unsigned long pos,unsigned long& readCount){
		m_inputStream->setPos(pos+m_streamPos0);
		return (unsigned char*)m_inputStream->read(readCount);
	}

	inline void streamReadTo(void* dst,unsigned long pos,unsigned long& readCount){
		void* data=streamRead(pos,readCount);
		memcpy(dst,data,readCount);
	}
	inline unsigned long getStreamSize()const{
		return m_inputStream->getDataSize()-m_streamPos0;
	}
};


class CImportMIDIOut
{
public:
	LPDIRECTSOUND     m_pDirectSound;
    static bool DMusic_Init();
    static bool DMusic_Free();
public:
    CImportMIDIOut(LPDIRECTSOUND lpds);
    virtual ~CImportMIDIOut();

	static bool checkHeadType(const RandInputStream& aInputStream){
		unsigned char tag[4];
		if (!aInputStream.test_read(&tag[0],4)) return false;

		if ( (tag[0]=='M') && (tag[1]=='T')&&(tag[2]=='h')&&(tag[3]=='d') ) return true;  //'MThd'
		return false;
	}
};

////////////////////////////////

void CMidiBuf::play(bool is_loop_play,long loopCount,long play_time_ms)
{
	MIDI_Free();

    long MIDI_data_size=getStreamSize();
	unsigned char* pMIDI_data=(unsigned char*)(m_inputStream->read_trust(MIDI_data_size));

	m_is_loop=is_loop_play;
	m_loopCount=loopCount;
	m_try_play_time_ms=play_time_ms;
	if (!MIDI_Load(pMIDI_data,MIDI_data_size))
	{
		MIDI_Free();
		return ;
	}
	MIDI_Play();
}

bool CMidiBuf::is_paly_finished()
{
	if ((state!=MIDI_TRY_PLAY)&&(state!=MIDI_PLAYING))
		return true;
	if (m_is_loop)
	{
		if (m_try_play_time_ms<0) 
			return false;
		long now_time=getMMTimerCount();
		return (now_time>=m_try_play_time_ms+m_PlayStartTime_ms);
	}
	else
	{
		HRESULT hr=m_dm_perf->IsPlaying(dm_segment,dm_segstate);
		if (hr == S_OK)
		{
			state=MIDI_PLAYING;
			return false;
		}
		else if (state==MIDI_TRY_PLAY)
		{
			long now_time=getMMTimerCount();
			return (now_time>=m_try_play_time_ms+m_PlayStartTime_ms);
		}
		else
			return true; 
	}
}

CMidiBuf::CMidiBuf(CImportMIDIOut* Owner)
:dm_segment(0),m_Owner(0),m_ID(0),dm_segstate(0),state(MIDI_NULL),m_try_play_time_ms(-1),
 m_dm_loader(0),m_dm_perf(0)
{
	m_Owner=Owner;
	m_inputStream=0;
	m_streamPos0=0;

	// create the performance
	CoCreateInstance(CLSID_DirectMusicPerformance, NULL,CLSCTX_INPROC,IID_IDirectMusicPerformance, (void**)&m_dm_perf);  
	m_dm_perf->Init(NULL,m_Owner->m_pDirectSound,(HWND)public_MIDIOut_hWnd);
	m_dm_perf->AddPort(NULL);
	// create the loader to load object(s) such as midi file
	CoCreateInstance( CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC, IID_IDirectMusicLoader, (void**)&m_dm_loader);
}

void CMidiBuf::init(RandInputStream* inputStream)
{
	MIDI_Free();
	if (m_inputStream!=0){
		delete m_inputStream;
		m_inputStream=0;
	}
	m_inputStream=inputStream;
	m_streamPos0=m_inputStream->getPos();
}

CMidiBuf::~CMidiBuf()
{
	MIDI_Free();
	if (m_inputStream!=0){
		delete m_inputStream;
		m_inputStream=0;
	}
	if (m_dm_perf)
	{
	    m_dm_perf->Stop(NULL, NULL, 0, 0 ); 
	    m_dm_perf->CloseDown();
	    m_dm_perf->Release();     
	}
	if (m_dm_loader)
	   m_dm_loader->Release();     
}

void CMidiBuf::MIDI_Stop()
{
	if ((dm_segment) && ((state==MIDI_TRY_PLAY)||(state==MIDI_PLAYING)))
    {
	   m_dm_perf->Stop(dm_segment, NULL, 0, 0);
	   state = MIDI_STOPPED;
    } 
}


void CMidiBuf::MIDI_Free()
{
    if (dm_segment)
    {
		MIDI_Stop();
        dm_segment->SetParam(GUID_Unload, -1, 0, 0, (void*)(m_dm_perf)); 
        dm_segment->Release(); 
        dm_segment  = 0;
        dm_segstate = 0;
        state       = MIDI_NULL;
    }
}

bool CMidiBuf::MIDI_Play()
{
	if ((dm_segment) && (state!=MIDI_NULL))
	{
		MIDI_Stop();

		if (m_is_loop)
			if(FAILED(dm_segment->SetRepeats(DMUS_SEG_REPEAT_INFINITE))) return false;
		else
			dm_segment->SetRepeats(0);

		if(FAILED(m_dm_perf->PlaySegment(dm_segment, 0 ,0,&dm_segstate))) return false;
		state = MIDI_TRY_PLAY;

		m_PlayStartTime_ms=getMMTimerCount();
		MUSIC_TIME mtLength=0;
		dm_segment->GetLength(&mtLength);
		if (!m_is_loop)
			m_try_play_time_ms=mtLength;
		else{
			long loopCount_time_ms=m_loopCount*mtLength;
			if ((m_try_play_time_ms<0)||(loopCount_time_ms<m_try_play_time_ms))
				m_try_play_time_ms=loopCount_time_ms;
		}
		return true;
	} 
	else
		return false;
}


bool CMidiBuf::MIDI_Load(unsigned char* MIDI_data,unsigned int MIDI_Data_size)
{
	// this function loads a midi segment
	IDirectMusicSegment*& pSegment = dm_segment;
	 
	DMUS_OBJECTDESC ObjDesc; 
	memset(&ObjDesc,0,sizeof(ObjDesc)); 
    ObjDesc.dwSize     =sizeof(ObjDesc);
    ObjDesc.guidClass  =CLSID_DirectMusicSegment;
    ObjDesc.pbMemData  =MIDI_data;
    ObjDesc.llMemLength=MIDI_Data_size;
    ObjDesc.dwValidData=DMUS_OBJ_CLASS|DMUS_OBJ_MEMORY;
  	 
	HRESULT hr;
	hr = m_dm_loader->GetObject(&ObjDesc,IID_IDirectMusicSegment, (void**) &pSegment);
	if (FAILED(hr))
	   return false;
	 

	hr = pSegment->SetParam(GUID_StandardMIDIFile,-1, 0, 0, (void*)(m_dm_perf));
	if (FAILED(hr))
	   return false;
	  
	hr = pSegment->SetParam(GUID_Download, -1, 0, 0, (void*)(m_dm_perf));
	if (FAILED(hr))
	   return false;

	dm_segstate = NULL;
	state       = MIDI_LOADED;
	return true;	
}


/////////////////////////////////////////////////////


CImportMIDIOut::CImportMIDIOut(LPDIRECTSOUND lpds)
:m_pDirectSound(0)
{
	m_pDirectSound=lpds;
	if (!DMusic_Init())
	{
		DMusic_Free();
	}
}

CImportMIDIOut::~CImportMIDIOut()
{
	DMusic_Free();
}


bool CImportMIDIOut::DMusic_Init()
{
	// set up directmusic  initialize COM
	if (FAILED(CoInitialize(NULL)))
	   return false;

	return true;
} 

bool CImportMIDIOut::DMusic_Free()
{
	// Release COM
	CoUninitialize(); 
	return true;
}




//////////////

CMIDIOut::CMIDIOut(void* pDirectSound):m_Import(0){
	m_Import=new CImportMIDIOut((LPDIRECTSOUND)pDirectSound);
}
CMIDIOut::~CMIDIOut(){
	delete (CImportMIDIOut*)m_Import;
}

ISoundMIDIBuffer* CMIDIOut::getNewSoundBuffer(){
	return new CMidiBuf((CImportMIDIOut*)m_Import);
}

void  CMIDIOut::updateBufData(ISoundMIDIBuffer* SoundBuffer){
	//no thing
}

void CMIDIOut::deleteSoundBuffer(ISoundMIDIBuffer* soundBuffer){
	delete soundBuffer;
}

bool CMIDIOut::isCanLoadFrom(const RandInputStream&  aInputStream){
	return CImportMIDIOut::checkHeadType(aInputStream);
}

#endif
#endif //#ifndef WINCE
