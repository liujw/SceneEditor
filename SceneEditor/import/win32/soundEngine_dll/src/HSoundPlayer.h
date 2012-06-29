//HSoundPlayer.h
#ifndef _HSoundPlayer_H_
#define _HSoundPlayer_H_

#define EXTCALL __cdecl

#ifdef HSOUNDPLAYER_EXPORTS
	#define DECLDIR  __declspec(dllexport)
#else 
	#define DECLDIR
#endif

#ifdef __cplusplus
extern "C" {
#endif

	/*
	typedef struct 
	{
		void*                        handle;
		long            (__stdcall * get_size )(void* StreamHandle);
		unsigned char*  (__stdcall * read )(void* StreamHandle,  long PosIndex,long* bReadCount);
		unsigned char*  (__stdcall * read_to )(void* StreamHandle,unsigned char* dst_buf,  long PosIndex,long* bReadCount);
		void            (__stdcall * read_finish )(void* StreamHandle);
	} IReadStream ;
	*/

	DECLDIR void*  EXTCALL HSoundPlayer_new(const void* hWND);
	DECLDIR void   EXTCALL HSoundPlayer_delete(void* HSoundPlayer);
				   
	DECLDIR void*  EXTCALL HSoundPlayer_play(void* HSoundPlayer,void* ReadStream,const long sf);
	DECLDIR void*  EXTCALL HSoundPlayer_play_by_name(void* HSoundPlayer,const char* fileName,const long sf);
    DECLDIR void*  EXTCALL HSoundPlayer_loop_play_by_time(void* HSoundPlayer,void* ReadStream,const long sf,const long msTime);
    DECLDIR void*  EXTCALL HSoundPlayer_loop_play_by_name_by_time(void* HSoundPlayer,const char* fileName,const long sf,const long msTime);
    DECLDIR void*  EXTCALL HSoundPlayer_loop_play_with_loopCount(void* HSoundPlayer,void* ReadStream,const long sf,const long loopCount,const long msTime);
    DECLDIR void*  EXTCALL HSoundPlayer_loop_play_by_name_with_loopCount(void* HSoundPlayer,const char* fileName,const long sf,const long loopCount,const long msTime);
    DECLDIR void*  EXTCALL HSoundPlayer_loop_play(void* HSoundPlayer,void* ReadStream,const long sf);
    DECLDIR void*  EXTCALL HSoundPlayer_loop_play_by_name(void* HSoundPlayer,const char* fileName,const long sf);
	DECLDIR long   EXTCALL HSoundPlayer_stop(void* HSoundPlayer,const void* sound_handle);
	DECLDIR void   EXTCALL HSoundPlayer_stop_all(void* HSoundPlayer);

#ifdef __cplusplus
}
#endif 

#endif  //_HSoundPlayer_H_