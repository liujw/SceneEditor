//wavDispose.cpp

#include "wavDispose.h"

static inline long DEC(long data,double decR){
#ifndef WIN32	  
    return (long)(data*decR);
#else
#ifdef _WIN64
    return (long)(data*decR);
#else
#ifndef asm
#define asm __asm
#endif
    
    long result;
    asm		fild	data
    asm		fld		decR
    asm     fmulp   st(1),st(0)
    asm		fistp	result
    return result;
#endif
#endif
}
template<typename T,long typeSize>
void tm_wavDispose_DEC(void* aWavData,long count,double decR0,double decR1){
    T* wavData=(T*)aWavData;
    double dR=(decR1-decR0)/count;
    if (typeSize==1){
        for (long i=0;i<count;++i){
            wavData[i]=(T)(DEC(wavData[i]-128,decR0)+128);
            decR0+=dR;
        }
    }else if (typeSize==3){
        for (long i=0;i<count;++i){
            long data=wavData[0]|(wavData[1]<<8)|(wavData[2]<<16);
            data=DEC(data,decR0);
            wavData[0]=(T)data;
            wavData[1]=(T)(data>>8);
            wavData[2]=(T)(data>>16);
            wavData+=typeSize;
            decR0+=dR;
        }
    }else{
        for (long i=0;i<count;++i){
            wavData[i]=(T)DEC(wavData[i],decR0);
            decR0+=dR;
        }
    }
}

bool wavDispose_DEC(long bitsPerSample,void* wavData,long byte_size,double decR0,double decR1){
    long count=byte_size*8/bitsPerSample;
    switch (bitsPerSample){
        case  8:{ tm_wavDispose_DEC<unsigned char,1>(wavData,count,decR0,decR1); } return true;
        case 16:{ tm_wavDispose_DEC<short,2>(wavData,count,decR0,decR1); } return true;
        case 24:{ tm_wavDispose_DEC<unsigned char,3>(wavData,count,decR0,decR1); } return true;
        case 32:{ tm_wavDispose_DEC<long,4>(wavData,count,decR0,decR1); } return true;
        default: 
            return false;
    };
}



template<class T,long typeSize>
void tm_wavDispose_volume(void* _dstWavData,const void* _srcWavData,long count,float volume){
    T* dstWavData=(T*)_dstWavData;
    const T* srcWavData=(const T*)_srcWavData;
    if (volume>=1.0f) {
        if (dstWavData==srcWavData) return ;
        for (long i=0;i<count;++i){
            dstWavData[i]=srcWavData[i];
        }
        return;
    } else if (volume<=0){
        T w0;
        if (typeSize==1) 
            w0=128;
        else
            w0=0;
        for (long i=0;i<count;++i)
            dstWavData[i]=w0;
    }else{
        const int shlVolume=12;
        long mulVolume=(long)(volume*volume*(1<<shlVolume));
        if (typeSize==1){
            for (long i=0;i<count;++i)
                dstWavData[i]=(T)( ((srcWavData[i]-128)*mulVolume>>shlVolume)+128 );
        }else{
            for (long i=0;i<count;++i)
                dstWavData[i]=(T)( (srcWavData[i])*mulVolume>>shlVolume);
        }
    }
}

bool wavDispose_volume(void* dstWavData,const void* srcWavData,long byte_size,float volume,long bitsPerSample){
    long count=byte_size*8/bitsPerSample;
    switch (bitsPerSample){
        case  8:{ tm_wavDispose_volume<unsigned char,1>(dstWavData,srcWavData,count,volume); } return true;
        case 16:{ tm_wavDispose_volume<short,2>(dstWavData,srcWavData,count,volume); } return true;
        default: 
            return false;
    };
}


void downmix_2_stereo(int channels, float **pcm, long samples)
{
    /* Channels are in vorbis order NOT WAVEFORMATEXTENSIBLE */
    const double SQRTHALF = 0.70710678118654752440084436210485;
    
    if (channels<=2) return;
    
    long n;
    float *data[2];
    data[0] = pcm[0];
    data[1] = pcm[1];
    
    if (channels >= 6) {
        for (n = 0; n < samples; n++) {
            data[0][n] =(float)( pcm[0][n] + pcm[1][n] * SQRTHALF + pcm[3][n] * SQRTHALF + pcm[5][n]);
            data[1][n] =(float)( pcm[2][n] + pcm[1][n] * SQRTHALF + pcm[4][n] * SQRTHALF + pcm[5][n]);
        }
    }
    else if (channels == 5) {
        for (n = 0; n < samples; n++) {
            data[0][n] =(float)( pcm[0][n] + pcm[1][n] * SQRTHALF + pcm[3][n] * SQRTHALF);
            data[1][n] =(float)( pcm[2][n] + pcm[1][n] * SQRTHALF + pcm[4][n] * SQRTHALF);
        }
    }
    else if (channels == 4) {
        for (n = 0; n < samples; n++) {
            data[0][n] =(float)( pcm[0][n] + pcm[2][n] * SQRTHALF);
            data[1][n] =(float)( pcm[1][n] + pcm[3][n] * SQRTHALF);
        }
    }
    else if (channels == 3) {
        for (n = 0; n < samples; n++) {
            data[0][n] =(float)( pcm[0][n] + pcm[1][n] * SQRTHALF);
            data[1][n] =(float)( pcm[2][n] + pcm[1][n] * SQRTHALF);
        }
    }
}


const int          conv_factor = 65536;               // Default convert to 16 bit PCM
const long         wrap_prev_pos_i = 32767,           // Default for 16 bit PCM
wrap_prev_neg_i = -32768;          // Default for 16 bit PCM
const double       wrap_prev_pos = wrap_prev_pos_i,           
wrap_prev_neg = wrap_prev_neg_i;          


inline long FixedToInt(float x)
{
    double val = (x*(2147483647.0/conv_factor)) ;
    if (val >= wrap_prev_pos) 
        return  wrap_prev_pos_i;
    else if (val <=wrap_prev_neg)
        return wrap_prev_neg_i;
#ifndef WIN32	  
    return (long)val;
#else
#define asm __asm
#ifdef _WIN64
    return (long)val;
#else
#ifndef asm
#define asm __asm
#endif
    
    long result;
    asm		fld		val
    asm		fistp	result;
    return result;
#endif
#endif
}

inline float mixFloat(float a,float b){
    return (a+b-a*b);
}

bool FixedToInt16(int channels, float **pcm, long count,int dst_channels,short* dst_pcm){
    
    if(channels>=2) {
        if (dst_channels==2){
            for(long i=0;i<count;i++) {
                dst_pcm[i*2  ]=(short)FixedToInt(pcm[0][i]);
                dst_pcm[i*2+1]=(short)FixedToInt(pcm[1][i]);
            }
        }else if(dst_channels==1){
            for(long i=0;i<count;i++) {
                dst_pcm[i]=(short)FixedToInt(mixFloat(pcm[0][i],pcm[1][i]));
            }
        }else
            return false;
    }
    else if (channels==1)
    {
        if (dst_channels==2){
            for(long i=0;i<count;i++) {
                short data=(short)FixedToInt(pcm[0][i]);
                dst_pcm[i*2  ]=data;
                dst_pcm[i*2+1]=data;
            }
        }else if(dst_channels==1){
            for(long i=0;i<count;i++) {
                dst_pcm[i]=(short)FixedToInt(pcm[0][i]);
            }
        }else
            return false;
    }else
        return false;
    return true;
}

#include "soundDecoder/ISoundDecoder.h"
void decodePCMData(void* lpvPtr,long dwBytes,ISoundDecoder* SoundDecoder,long& curPcmDataBytesCount,long& curPlayLoopCount)
{
    if (lpvPtr==0) return;
    
    while (dwBytes>0) {
        TSoundDataBuf* pcm_data=SoundDecoder->decode();
        if ((curPcmDataBytesCount>0)&&(pcm_data->pcm_count == pcm_data->pcm_readed)) {
            if (curPlayLoopCount>0){
                SoundDecoder->seek_time_ms(0);
                pcm_data=SoundDecoder->decode();
            }
            --curPlayLoopCount;
        }
        
        long pcm_size=(pcm_data->pcm_count - pcm_data->pcm_readed);
        if ((curPcmDataBytesCount<=0)||(pcm_size<=0)) {
            if (SoundDecoder->get_sound_BitsPerSample()==8)
                memset(lpvPtr, 128, dwBytes);					
            else
                memset(lpvPtr, 0, dwBytes);
            curPcmDataBytesCount-=dwBytes;
            dwBytes=0;
            return;
        }else{
            long writeSize=dwBytes;
            if (writeSize>pcm_size) 
                writeSize=pcm_size;
            if (writeSize>curPcmDataBytesCount) 
                writeSize=curPcmDataBytesCount;
            
            memcpy(lpvPtr,(void*) &pcm_data->pcm_buf[pcm_data->pcm_readed], writeSize);
            dwBytes-=writeSize;
            ((char*&)lpvPtr)+=writeSize;
            pcm_data->pcm_readed+=writeSize;
            curPcmDataBytesCount-=writeSize;
        }
    }
}
