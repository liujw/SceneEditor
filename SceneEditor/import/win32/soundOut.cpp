//DxSoundOut.cpp

#include "../importInclude/soundOut.h"
#ifndef _SOUND_PALY_ONE_

#include "../../GameEngine/sound/wavDispose.h"
//#define DIRECTSOUND_VERSION 0x0400  /* Version 4.0 */
//#include <mmSystem.h> 
#ifdef Set
	#undef Set
#endif
#include <dsound.h> 
#include "../importInclude/mmTimer.h" 
#pragma comment (lib, "dsound.lib")
//#pragma comment (lib, "dxguid.lib")

void*  public_DxSound_hWnd=0;

void CSoundOut::setWinHandle(void* wHandle){
	public_DxSound_hWnd=wHandle;
}


class CDxSoundOutImport;

class CDxSoundBuffer:public ISoundBuffer
{
public:
    CDxSoundOutImport*		m_Owner;
	ISoundDecoder*			m_SoundDecoder;
	LPDIRECTSOUNDBUFFER		m_pDsbuf;
	bool                    m_IsPlaying;
	long                    m_PlayStartTime_ms;
	long					m_DxSoundBufSize;
	long					m_DxSoundBufTimeSpace;
	long                    m_WriteOffset;
	long                    m_curPlayLoopCount;
    long                    m_curPcmDataBytesCount;
    long                    m_sysDelayPcmDataBytesCount;
	bool                    m_is_BufLost;
	void*                   m_ID;
    static void CreateDSBuffer(LPDIRECTSOUNDBUFFER* ppDsbuf,long samples_rate,long Channels,long BitsPerSample,LPDIRECTSOUND lpDxSound,long * pDxSoundBufSize,long* pDxSoundBufTimeSpace);
	static void WriteDSBuffer(LPDIRECTSOUNDBUFFER lpDsb,DWORD DxSoundBufSize,long& WriteOffset,ISoundDecoder* SoundDecoder,
			bool& isBufLost,long PlayStartTime_ms,long& m_curPlayLoopCount,long& m_curPcmDataBytesCount,long& m_DxSoundBufTimeSpace);
	void  UpdateBufData();
   void   private_ToUpdateBufData();
public:
	CDxSoundBuffer(CDxSoundOutImport* Owner);
    ~CDxSoundBuffer();
	void init(ISoundDecoder* SoundDecoder);
	void setID(void* ID);
	void* getID();
	bool is_paly_finished();
	void play(bool is_loop_play,long loopCount,long play_time_ms);
	void stop();
};



class CDxSoundOutImport
{
public:
	LPDIRECTSOUND m_lpDxSound; 
public:
	CDxSoundOutImport();
    ~CDxSoundOutImport();
    bool init(); 
	CDxSoundBuffer* new_ISoundBuffer();
	void  UpdateBufData(CDxSoundBuffer* SoundBuffer);
	void delete_ISoundBuffer(CDxSoundBuffer* SoundBuffer);
	void*  get_DirectSound_handle() { return m_lpDxSound; }
};

CDxSoundBuffer::CDxSoundBuffer(CDxSoundOutImport* Owner)
:m_Owner(0),m_SoundDecoder(0),m_IsPlaying(false),m_pDsbuf(0),m_WriteOffset(0),m_DxSoundBufTimeSpace(0),
m_curPlayLoopCount(-1),m_is_BufLost(false),m_ID(0)
{
	m_Owner=Owner;
}

CDxSoundBuffer::~CDxSoundBuffer()
{
	stop();
	if (m_pDsbuf!=0)
	{
		m_pDsbuf->Release();
		m_pDsbuf=0;
	}
	if (m_SoundDecoder!=0)
	{
		delete m_SoundDecoder;
		m_SoundDecoder=0;
	}
}
void CDxSoundBuffer::init(ISoundDecoder* SoundDecoder)
{
	if (m_pDsbuf!=0)
	{
		m_pDsbuf->Release();
		m_pDsbuf=0;
	}
	m_SoundDecoder=SoundDecoder;
	CreateDSBuffer(&m_pDsbuf,m_SoundDecoder->get_sound_rate(),m_SoundDecoder->get_sound_Channels(),m_SoundDecoder->get_sound_BitsPerSample(),
		m_Owner->m_lpDxSound,&m_DxSoundBufSize,&m_DxSoundBufTimeSpace);
}

void* CDxSoundBuffer::getID(){
	return m_ID;
}
void CDxSoundBuffer::setID(void* ID){
	m_ID=ID;
}


bool CDxSoundBuffer::is_paly_finished()
{
	if (m_pDsbuf==0)
		return false;
    return ((m_curPcmDataBytesCount+m_DxSoundBufSize)<0);
	//long nowTime_ms=getMMTimerCount();
	//return (nowTime_ms>=m_PlayStartTime_ms+m_try_play_time_ms);
}

void CDxSoundBuffer::play(bool is_loop_play,long loopCount,long play_time_ms)
{
	if (m_IsPlaying)
		this->stop();

	if (is_loop_play&&(loopCount<0)&&(play_time_ms<0)){
		m_curPlayLoopCount=1<<30;
		m_curPcmDataBytesCount=1<<30;
	}else{
		if ((loopCount<0)&&(play_time_ms<0))
			loopCount=1;
		m_curPlayLoopCount=loopCount;
		if (m_curPlayLoopCount<0) m_curPlayLoopCount=1<<30;
		--m_curPlayLoopCount; //开始播放
		if ((play_time_ms<0)||(play_time_ms>=(1<<28)))
			m_curPcmDataBytesCount=1<<30;
		else
			m_curPcmDataBytesCount=m_SoundDecoder->getPcmDataBytesCount(play_time_ms);
	}
	m_PlayStartTime_ms=getMMTimerCount();
	if (m_pDsbuf!=0)
	{
		m_SoundDecoder->seek_time_ms(0);
		m_pDsbuf->SetCurrentPosition(m_DxSoundBufSize-4); 
		m_WriteOffset=0;
		m_is_BufLost=false;
		private_ToUpdateBufData();
		m_pDsbuf->SetCurrentPosition(0); 
		m_pDsbuf->Play(0,0,DSBPLAY_LOOPING);
	}
	m_IsPlaying=true;
}


void  CDxSoundBuffer::private_ToUpdateBufData()
{ 
	WriteDSBuffer(m_pDsbuf,m_DxSoundBufSize,m_WriteOffset,m_SoundDecoder,m_is_BufLost,m_PlayStartTime_ms,m_curPlayLoopCount,m_curPcmDataBytesCount,m_DxSoundBufTimeSpace);
}

void CDxSoundBuffer::stop()
{
	if (m_IsPlaying)
	{
		if (m_pDsbuf!=0)
			m_pDsbuf->Stop();
		m_IsPlaying=false;
	}
}

void  CDxSoundBuffer::UpdateBufData()
{ 
	if (m_IsPlaying)
		private_ToUpdateBufData();
}


void CDxSoundBuffer::CreateDSBuffer(LPDIRECTSOUNDBUFFER* ppDsbuf,long samples_rate,long Channels,long BitsPerSample,LPDIRECTSOUND lpDxSound,long * pDxSoundBufSize,long* pDxSoundBufTimeSpace) 
{ 
	*ppDsbuf=0;
	if (lpDxSound==0) return;

    WAVEFORMATEX wfx; 
    DSBUFFERDESC dsbdesc; 

    // Set up WAV format structure. 
    memset(&wfx, 0, sizeof(WAVEFORMATEX)); 
    wfx.wFormatTag = WAVE_FORMAT_PCM; 
    wfx.nChannels = (WORD)Channels; 
    wfx.nSamplesPerSec = samples_rate; 
    wfx.nBlockAlign =(WORD)( Channels*(BitsPerSample/8)); 
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign; 
    wfx.wBitsPerSample =(WORD) BitsPerSample; 

	*pDxSoundBufTimeSpace=200;  //ms 数据 
	*pDxSoundBufSize=((long)(wfx.nAvgBytesPerSec*0.001*(*pDxSoundBufTimeSpace)) +3) /4 * 4; 

    // Set up DSBUFFERDESC structure. 
    memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); 
    dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
    dsbdesc.dwFlags = DSBCAPS_GLOBALFOCUS; //0; //DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY; //  流buffer 
    dsbdesc.dwBufferBytes = *pDxSoundBufSize;
    dsbdesc.lpwfxFormat = &wfx; 
    // Create buffer. 
    HRESULT hr = lpDxSound->CreateSoundBuffer(&dsbdesc, ppDsbuf, 0); 
} 


//////////////////////////////////

void CDxSoundBuffer::WriteDSBuffer(LPDIRECTSOUNDBUFFER lpDsb,DWORD DxSoundBufSize,long& WriteOffset,ISoundDecoder* SoundDecoder,
	bool& isBufLost,long PlayStartTime_ms,long& m_curPlayLoopCount,long& m_curPcmDataBytesCount,long& m_DxSoundBufTimeSpace)
{ 
	if (lpDsb==0) return;

	long canWiteSize;
	if (isBufLost)
	{
		//处理缓冲数据失效
		WriteOffset=0;
		canWiteSize=DxSoundBufSize-4;
		lpDsb->SetCurrentPosition(canWiteSize);

		//按时间轴跳过一段数据，不放到缓冲中
		long nowTime_ms=getMMTimerCount();
		long pos_time=nowTime_ms-PlayStartTime_ms;
        m_curPcmDataBytesCount-=SoundDecoder->getPcmDataBytesCount(pos_time);
		if ((m_curPlayLoopCount>0)&&(m_curPcmDataBytesCount>0)){
			long sound_time=SoundDecoder->get_sound_time_ms(); //!!! 不准确
			if (sound_time<=0)
				pos_time=0;
			else{
				m_curPlayLoopCount-=pos_time / sound_time;
				pos_time=pos_time % sound_time;
			}
		}
		SoundDecoder->seek_time_ms(pos_time);
	}else {
		DWORD CurPos=0;
		lpDsb->GetCurrentPosition(&CurPos,0);
		canWiteSize=CurPos-WriteOffset;
	}

	if (canWiteSize<0)
		canWiteSize+=DxSoundBufSize;
	if (canWiteSize<(long)DxSoundBufSize/4) return;

    HRESULT hr;
    LPVOID lpvPtr1=0;
    DWORD dwBytes1=0;
    LPVOID lpvPtr2=0;
    DWORD dwBytes2=0;

	long wantWiteSize=(canWiteSize-4)/4*4;
    //获得将要写的块的地址
    hr = lpDsb->Lock(WriteOffset, wantWiteSize, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);
    if (DSERR_BUFFERLOST == hr)//恢复然后重新锁定
    {
        lpDsb->Restore();
        hr = lpDsb->Lock(WriteOffset, wantWiteSize, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);
    }

    if SUCCEEDED(hr)
    {
        //拷贝数据
		decodePCMData(lpvPtr1,dwBytes1,SoundDecoder,m_curPcmDataBytesCount,m_curPlayLoopCount);
		decodePCMData(lpvPtr2,dwBytes2,SoundDecoder,m_curPcmDataBytesCount,m_curPlayLoopCount);
        //解锁
        hr = lpDsb->Unlock(lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);

		if ((m_curPlayLoopCount<0)){
			if (m_curPcmDataBytesCount>(long)DxSoundBufSize){
				m_curPcmDataBytesCount=DxSoundBufSize;
			}
		}

		WriteOffset+=(dwBytes1+dwBytes2);
		if (WriteOffset>=(long)DxSoundBufSize)
			WriteOffset-=DxSoundBufSize;
		if (isBufLost)
			lpDsb->SetCurrentPosition(0);
		isBufLost=false;
    }
	else
	{
		isBufLost=true;// 标记缓冲数据失效
	}
}



/////////////////


CDxSoundOutImport::CDxSoundOutImport()
:m_lpDxSound(0)
{
}

CDxSoundOutImport::~CDxSoundOutImport()
{
	if (m_lpDxSound!=0)
	{
		m_lpDxSound->Release();
		m_lpDxSound=0;
	}
}
bool CDxSoundOutImport::init()
{
	HRESULT hr=DirectSoundCreate(0, &m_lpDxSound,0);
	if (m_lpDxSound!=0)
	{
		HRESULT hr= m_lpDxSound->SetCooperativeLevel((HWND)public_DxSound_hWnd, DSSCL_PRIORITY); //DSSCL_NORMAL, DSSCL_PRIORITY, DSSCL_WRITEPRIMARY 
		return true;
	}
	return false;
}
CDxSoundBuffer* CDxSoundOutImport::new_ISoundBuffer()
{
	return new CDxSoundBuffer(this);
}
void CDxSoundOutImport::delete_ISoundBuffer(CDxSoundBuffer* SoundBuffer)
{
	if (SoundBuffer!=0)
		delete SoundBuffer;
}

void  CDxSoundOutImport::UpdateBufData(CDxSoundBuffer* SoundBuffer)
{
	if (SoundBuffer!=0)
		SoundBuffer->UpdateBufData();
}


///////////////////////////////////////////////////////////////////////////////////

CSoundOut::CSoundOut():m_SoundOutImport(0){
}

CSoundOut::~CSoundOut(){
	if (m_SoundOutImport!=0)
		delete (CDxSoundOutImport*)m_SoundOutImport;
}

bool CSoundOut::init(){
	if (m_SoundOutImport!=0){
		CDxSoundOutImport* tmp_SoundOutImport=(CDxSoundOutImport*)m_SoundOutImport;
		m_SoundOutImport=0;
		delete tmp_SoundOutImport;
	}
	m_SoundOutImport=new CDxSoundOutImport();
	return ((CDxSoundOutImport*)m_SoundOutImport)->init();
}

ISoundBuffer* CSoundOut::getNewSoundBuffer(){
	return ((CDxSoundOutImport*)m_SoundOutImport)->new_ISoundBuffer();
}

void CSoundOut::deleteSoundBuffer(ISoundBuffer* SoundBuffer){
	((CDxSoundOutImport*)m_SoundOutImport)->delete_ISoundBuffer((CDxSoundBuffer*)SoundBuffer);
}

void  CSoundOut::updateBufData(ISoundBuffer* SoundBuffer){
	((CDxSoundOutImport*)m_SoundOutImport)->UpdateBufData((CDxSoundBuffer*)SoundBuffer);
}

void*  CSoundOut::getSoundOutHandle(){
	return ((CDxSoundOutImport*)m_SoundOutImport)->get_DirectSound_handle();
}

#endif