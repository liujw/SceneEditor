// HSoundPlayer.cpp

#include "../../../../GameEngine/sound/SoundPlayer.h"

/*
#ifdef _WIN64
	#if _DEBUG
		#pragma comment (lib, "engineD_x64.lib")
	#else
		#pragma comment (lib, "engine_x64.lib")
	#endif
#else
	#if _DEBUG
		#pragma comment (lib, "engineD.lib")
	#else
		#pragma comment (lib, "engine.lib")
	#endif
#endif
*/

	typedef struct 
	{
		void*                        handle;
		long            (__stdcall * get_size )(void* StreamHandle);
		unsigned char*  (__stdcall * read )(void* StreamHandle,  long PosIndex,long* bReadCount);
		unsigned char*  (__stdcall * read_to )(void* StreamHandle,unsigned char* dst_buf,  long PosIndex,long* bReadCount);
		void            (__stdcall * read_finish )(void* StreamHandle);
	} IReadStream ;

	class CImportInputStream:public IRandInputStream{
	private:
		IReadStream m_stream;
		long        m_pos;
	public:
		CImportInputStream(IReadStream* stream):m_pos(0){
			m_stream=*stream;
		}
		virtual ~CImportInputStream(){
			if (m_stream.handle!=0){
				m_stream.read_finish(m_stream.handle);
				m_stream.handle=0;
			}
		}

		virtual void* read(unsigned long& bCount){
			void* result=test_read(bCount);
			m_pos+=bCount;
			return result;
		}

		virtual void* test_read(unsigned long& bCount)const{
			return m_stream.read(m_stream.handle,m_pos,(long*)&m_pos);
		}

		virtual unsigned long getPos()const{
			return m_pos;
		}
		virtual bool setPos(unsigned long pos){
			m_pos=pos;
			return true;
		}
		virtual unsigned long getDataSize()const{
			return m_stream.get_size(m_stream.handle);
		}
	};

#include "HSoundPlayer.h"

/*
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
*/


//extern "C" {
void* EXTCALL  HSoundPlayer_new(const void* hWND)
{
	CSoundPlayer::setWinHandle((void*)hWND);
	return new CSoundPlayer();
}

void EXTCALL  HSoundPlayer_delete(void* HSoundPlayer)
{
	delete (CSoundPlayer*)HSoundPlayer;
}


void* EXTCALL  HSoundPlayer_play(void* HSoundPlayer,void* ReadStream,const long sf)
{
	return ((CSoundPlayer*)HSoundPlayer)->play(new CImportInputStream((IReadStream*)ReadStream));
}

void* EXTCALL  HSoundPlayer_play_by_name(void* HSoundPlayer,const char* fileName,const long sf)
{
	return ((CSoundPlayer*)HSoundPlayer)->play(fileName);
}

void* EXTCALL  HSoundPlayer_loop_play_by_time(void* HSoundPlayer,void* ReadStream,const long sf,const long msTime)
{
	return ((CSoundPlayer*)HSoundPlayer)->loop_play(new CImportInputStream((IReadStream*)ReadStream),1<<30,msTime);
}

void* EXTCALL  HSoundPlayer_loop_play_by_name_by_time(void* HSoundPlayer,const char* fileName,const long sf,const long msTime)
{
	return ((CSoundPlayer*)HSoundPlayer)->loop_play(fileName,1<<30,msTime);
}

void* EXTCALL  HSoundPlayer_loop_play_with_loopCount(void* HSoundPlayer,void* ReadStream,const long sf,const long loopCount,const long msTime)
{
	return ((CSoundPlayer*)HSoundPlayer)->loop_play(new CImportInputStream((IReadStream*)ReadStream),loopCount,msTime);
}
void* EXTCALL  HSoundPlayer_loop_play_by_name_with_loopCount(void* HSoundPlayer,const char* fileName,const long sf,const long loopCount,const long msTime)
{
	return ((CSoundPlayer*)HSoundPlayer)->loop_play(fileName,loopCount,msTime);
}


void* EXTCALL  HSoundPlayer_loop_play(void* HSoundPlayer,void* ReadStream,const long sf)
{
	return ((CSoundPlayer*)HSoundPlayer)->loop_play(new CImportInputStream((IReadStream*)ReadStream));
}

void* EXTCALL  HSoundPlayer_loop_play_by_name(void* HSoundPlayer,const char* fileName,const long sf)
{
	return ((CSoundPlayer*)HSoundPlayer)->loop_play(fileName);
}

long EXTCALL   HSoundPlayer_stop(void* HSoundPlayer,const void* sound_handle)
{
	return  (long) ( ((CSoundPlayer*)HSoundPlayer)->stop(sound_handle) );
}

void EXTCALL   HSoundPlayer_stop_all(void* HSoundPlayer)
{
	((CSoundPlayer*)HSoundPlayer)->stop_all();
}



//}