//SoundImport.cpp

#include "../importInclude/SoundImport.h"
#include "SysImportType.h"

#ifdef _SOUND_PALY_ONE_

#include "VThread.h"
#include "hStream.h"
#include "../../GameEngine/sound/soundDecoder/OggDecoder.h"
#include "../../GameEngine/sound/soundDecoder/Mp3Decoder.h"
#include "../../GameEngine/sound/soundDecoder/WavDecoder.h"
#include "../../GameEngine/sound/wavDispose.h"


typedef bool (*TWavOut_close_callBack_getIsWantExit)(void* callBackData);

void* wavOut_create();
bool  wavOut_setParams(void* wavOutHandle,double SampleFreq,long BitsPerSample,long Channels);
void  wavOut_playSamples(void* wavOutHandle,UInt8* pcm_buf,long buf_size);
void  wavOut_close(void* wavOutHandle,TWavOut_close_callBack_getIsWantExit callBack_getIsWantExit=0,void* callBackData=0);

	//音量值
	float useVolume=0.5f;	 
	float volumeIsActive=1.0f;
	const float aIncStep=1.0f/15;
	void setWavOutActive(bool isActive){
		if (isActive)
			volumeIsActive=1;
		else 
			volumeIsActive=0;
	}
	float getWavOutVolumeValue(){
		return useVolume;
	}
	float getWavOutVolume(){
		return useVolume*volumeIsActive;
	}
	void setWavOutVolume(float volume){
		if (volume>1.0f) 
			useVolume=1.0f;
		else if (volume>=0)
			useVolume=volume;
		else if (volume<0)
			useVolume=0;
	}
	void addWavOutVolumeStep(){
		setWavOutVolume(useVolume+aIncStep);
	}
	void subWavOutVolumeStep(){
		setWavOutVolume(useVolume-aIncStep);
	}

class CSoundOutBase{
public:
	virtual void private_disposePlayFinish(const VThread* thread,bool isPalyOk)=0;
};

bool VThread_wavOut_close_callBack_getIsWantExit(void* callBackData){
	return ((VThread*)callBackData)->getIsWantExit();
}

class CSoundOutThread:public VThread{
private:
	String     m_fileName;
	CSoundOutBase* m_owner;
	bool play(){
		if (getIsWantExit()) return false;
		FileInputStream* stream=0;
		try{
			stream=new FileInputStream(m_fileName.c_str());
		}catch(...){
			return false;
		}
		ISoundDecoder* dec=0;
			
		if (COggDecoder::isCanLoadFrom(*stream))
			dec=new COggDecoder();
		else if (CMp3Decoder::isCanLoadFrom(*stream))
			dec=new CMp3Decoder();
		else if (CWavDecoder::isCanLoadFrom(*stream))
			dec=new CWavDecoder();
		else {
			delete stream;
			return false;
		}
		
		if (!(dec->init(stream))){
			delete dec;
			return false;
		}
		dec->seek_time_ms(0);

		void* wavOutHandle=wavOut_create();
		if ( !wavOut_setParams (wavOutHandle,dec->get_sound_rate(),dec->get_sound_BitsPerSample(),dec->get_sound_Channels()) ){
			wavOut_close(wavOutHandle,0,0);
			delete dec;
			return false;
		}

		while (true){
			if (getIsWantExit()){
				wavOut_close(wavOutHandle,0,0);
				delete dec;
				return false;
			}
			TSoundDataBuf* sdb=dec->decode();
			if (sdb->pcm_count==sdb->pcm_readed) break;
			wavOut_playSamples (wavOutHandle,&sdb->pcm_buf[sdb->pcm_readed], (sdb->pcm_count - sdb->pcm_readed));
			sdb->pcm_readed=sdb->pcm_count;
		}
		if (!getIsWantExit())
			wavOut_close(wavOutHandle,VThread_wavOut_close_callBack_getIsWantExit,this);
		else
			wavOut_close(wavOutHandle,0,0);
		delete dec;
		return true;
	}
protected:
	virtual void setIsWantExit(){
		m_owner=0;
		VThread::setIsWantExit();
	}
	virtual void run(){
		bool isPlayOk=play();
		if (!isPlayOk)
			sleep_s(0.01);

		CSoundOutBase* owner=m_owner;
		m_owner=0;

		if ((!getIsWantExit())&&(owner!=0)){
			owner->private_disposePlayFinish(this,isPlayOk);
		}
	}
public:
	CSoundOutThread(const String& fileName,CSoundOutBase* owner)
		:m_fileName(fileName),m_owner(owner){
	}
};


class CSoundOut:public CSoundOutBase{
private:
	String m_fileName;
	TPlayFinishCallBackProc m_playFinishCallBackProc;
	void * m_callBackData;
	VThread* m_thread;
	void stop(){
		VThread::tryExitThread(m_thread);
		m_playFinishCallBackProc=0;
		m_callBackData=0;
	}
public:
	CSoundOut(const String& fileName)
		:m_fileName(fileName),m_playFinishCallBackProc(0),m_callBackData(0),m_thread(0){
	}
	void play(TPlayFinishCallBackProc playFinishCallBackProc,void *callBackData){
		stop();
		m_playFinishCallBackProc=playFinishCallBackProc;
		m_callBackData=callBackData;

		m_thread=new CSoundOutThread(m_fileName,this);
		m_thread->start();
	}
	~CSoundOut(){
		stop();
	}

	virtual void private_disposePlayFinish(const VThread* thread,bool isPalyOk){
		if ((m_thread==0)||(thread!=m_thread)) return;
		m_thread=0;
		TPlayFinishCallBackProc playFinishCallBackProc=m_playFinishCallBackProc;
		void *callBackData=m_callBackData;
		m_playFinishCallBackProc=0;
		m_callBackData=0;

		if (isPalyOk){
			if (playFinishCallBackProc!=0){
				playFinishCallBackProc(this,callBackData);
			}
		}
	}
};


//////////////////////////////////////////////////////////
#ifdef Set
	#undef Set
#endif
#include "Windows.h"


void CALLBACK waveOut_callback ( HWAVEOUT dev, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2 );

const long MaxAlowWaveHeaderCount=16;


class TWavOutData{
private:
	CRITICAL_SECTION  m_cs;
	HWAVEOUT          m_dev;
	bool			  m_isOpening;
	long			  m_usingWaveHeaderCount;
	Vector<WAVEHDR*> m_doneWaveHeaderList;
	long			  m_bitsPerSample;
	void lock(){
		EnterCriticalSection (&m_cs);
	}
	void unlock(){
		LeaveCriticalSection (&m_cs);
	}
	void deleteWaveHeader(WAVEHDR* waveHeader,bool isUnprepareHeader=true){
		if (isUnprepareHeader)
			waveOutUnprepareHeader (m_dev,waveHeader,sizeof(WAVEHDR) );
		delete[](char*)(waveHeader->lpData);
		delete waveHeader;
	}
	WAVEHDR* newWaveHeader(void* pcm_buf,long buf_size){
		LPWAVEHDR  wh=new WAVEHDR();
		wh -> dwBufferLength = buf_size;
		wh -> lpData         = new char[buf_size];
				
		//音量处理
		wavDispose_volume(wh ->lpData,pcm_buf,buf_size,getWavOutVolume(),m_bitsPerSample);
		return wh;
	}
	void clearDoneWaveHeaderList(){
		lock();
		for (long i=0;i<(long)m_doneWaveHeaderList.size();++i)
			deleteWaveHeader(m_doneWaveHeaderList[i]);
		m_doneWaveHeaderList.clear();
		unlock();
	}
	bool getIsCanPushWaveHeader(){
		bool result=false;
		lock();
		result=(m_usingWaveHeaderCount<MaxAlowWaveHeaderCount);
		unlock();
		return result;
	}
	bool getIsCanClose(){
		bool result=false;
		lock();
		result=(m_usingWaveHeaderCount==0);
		unlock();
		return result;
	}

	void waitIsCanPushWaveHeader(){
		while(true) {
			clearDoneWaveHeaderList();  // free used blocks ...			
			if ( getIsCanPushWaveHeader() ) // wait for a free block ...
				break;
			::Sleep (1);
		}
	}
public:
	TWavOutData():m_dev(0),m_usingWaveHeaderCount(0),m_isOpening(false){
		InitializeCriticalSection(&m_cs);
	}
	~TWavOutData(){
		DeleteCriticalSection(&m_cs);
	}

	void doneWaveHeader(WAVEHDR* waveHeader){
		lock();
		--m_usingWaveHeaderCount;
		m_doneWaveHeaderList.push_back(waveHeader);
		unlock();
	}
	
	bool wavOpen(WAVEFORMATEX * waveFormat){
		wavClose(0,0);
		if ( waveOutGetNumDevs () == 0 )
			return false;
		UINT      deviceID =WAVE_MAPPER;
		MMRESULT result=waveOutOpen(&m_dev, deviceID,waveFormat, (DWORD)waveOut_callback,(DWORD)this, CALLBACK_FUNCTION ) ;
		if (result!=MMSYSERR_NOERROR)
			return false;
		m_bitsPerSample=waveFormat->wBitsPerSample;
		waveOutReset(m_dev);
		//SetThreadPriority ( GetCurrentProcess (), THREAD_PRIORITY_NORMAL );
		::SetThreadPriority( GetCurrentProcess (), THREAD_PRIORITY_HIGHEST );
		//SetThreadPriority ( GetCurrentProcess (), THREAD_PRIORITY_TIME_CRITICAL );
		m_isOpening=true;
		return m_isOpening;
	}

	bool  wavPlay(UInt8* pcm_buf,long buf_size){
		waitIsCanPushWaveHeader();

		LPWAVEHDR  wh=newWaveHeader(pcm_buf, buf_size);

		if ( waveOutPrepareHeader ( m_dev, wh, sizeof (WAVEHDR)) != MMSYSERR_NOERROR ) {
			deleteWaveHeader(wh,false);
			return false;
		}
		if ( waveOutWrite(m_dev, wh, sizeof (WAVEHDR)) != MMSYSERR_NOERROR ) {
			deleteWaveHeader(wh);
			return false;
		}

		lock();
		++m_usingWaveHeaderCount;
		unlock();
		return true;
	}

	void wavClose(TWavOut_close_callBack_getIsWantExit callBack_getIsWantExit,void* callBackData){
		if ( m_isOpening ) {
			if (callBack_getIsWantExit!=0){
				while(!getIsCanClose()){
					if (callBack_getIsWantExit(callBackData))
						break;
					::Sleep (1);
				}
			}
			waveOutReset(m_dev); // reset the device
			clearDoneWaveHeaderList();  // free used blocks ...			
			waveOutClose (m_dev);      // close the device
		}
		m_isOpening=false;
		m_dev = 0;
		m_usingWaveHeaderCount=0;
	}
};

void CALLBACK waveOut_callback ( HWAVEOUT dev, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2 ){
	if ( uMsg == WOM_DONE ) {
		TWavOutData* wavOutData=(TWavOutData*)dwInstance; 
		wavOutData->doneWaveHeader((WAVEHDR*) dwParam1);
	}
}

void* wavOut_create(){
	return new TWavOutData();
}

bool  wavOut_setParams(void* wavOutHandle,double SampleFreq,long BitsPerSample,long Channels){
	if (wavOutHandle==0) return false;
	WAVEFORMATEX  waveformat;
	memset(&waveformat,0,sizeof(waveformat));
	waveformat.cbSize=sizeof(waveformat);

	waveformat.wFormatTag = WAVE_FORMAT_PCM;
	waveformat.wBitsPerSample    = (WORD)BitsPerSample;
	waveformat.nChannels         = (WORD)Channels;
	waveformat.nSamplesPerSec    = (unsigned long)(SampleFreq + 0.5);
	waveformat.nBlockAlign       = (waveformat.wBitsPerSample + 7) / 8 * waveformat.nChannels;
	waveformat.nAvgBytesPerSec   = waveformat.nSamplesPerSec * waveformat.nBlockAlign;

	TWavOutData* wavOutData=(TWavOutData*)wavOutHandle; 
	return wavOutData->wavOpen(&waveformat);
}

void  wavOut_playSamples(void* wavOutHandle,UInt8* pcm_buf,long buf_size){
	if (wavOutHandle==0) return;
	TWavOutData* wavOutData=(TWavOutData*)wavOutHandle; 
	wavOutData->wavPlay(pcm_buf,buf_size);
}
void wavOut_close(void* wavOutHandle,TWavOut_close_callBack_getIsWantExit callBack_getIsWantExit,void* callBackData){
	if (wavOutHandle==0) return;
	TWavOutData* wavOutData=(TWavOutData*)wavOutHandle; 
	wavOutData->wavClose(callBack_getIsWantExit,callBackData);
	delete wavOutData;	
}

//////////////////////////////////////////////////////////////////////////////////////////

void* sound_create(const char* soundFileName,const char* soundFilePath){
	return new CSoundOut(String(soundFilePath)+"/"+soundFileName);
}


void  sound_play(void* soundHandleImport,TPlayFinishCallBackProc playFinishCallBackProc,void *callBackData){
	if (soundHandleImport!=0)
		((CSoundOut*)soundHandleImport)->play(playFinishCallBackProc,callBackData);
}

void  sound_delete(void* soundHandleImport){
	if (soundHandleImport!=0)
		delete (CSoundOut*)soundHandleImport;
}

#else

	void* sound_create(const char* soundFileName,const char* soundFilePath){
		return 0;
	}
	void  sound_play(void* soundHandleImport,TPlayFinishCallBackProc playFinishCallBackProc,void *callBackData){
	}
	void  sound_delete(void* soundHandleImport){
	}

#endif
		
void  phone_vibrate(){
	//
}

void* gBackgroundMusic=0;
void soundBackgroundMusic_play(const char* soundFileName,const char* soundFilePath,bool isLoop){

}
void soundBackgroundMusic_stop(){
}
