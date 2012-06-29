//wavDispose.h

#ifndef _wavDispose_h_
#define _wavDispose_h_

//消音处理
bool wavDispose_DEC(long bitsPerSample,void* wavData,long byte_size,double decR0,double decR1);

//音量调节
bool wavDispose_volume(void* dstWavData,const void* srcWavData,long byte_size,float volume,long bitsPerSample);

//多路混合成两路声轨
void downmix_2_stereo(int channels, float **pcm, long samples);

//浮点单或双通道 转成16bit 
bool FixedToInt16(int channels, float **pcm, long count,int dst_channels,short* dst_pcm);

class ISoundDecoder;
void decodePCMData(void* lpvPtr,long dwBytes,ISoundDecoder* SoundDecoder,long& curPcmDataBytesCount,long& curPlayLoopCount);

#endif // _wavDispose_h_