/******************************************************************************/
/*  CRC32.h                                                                   */
/*  侯思松  2003.4.21                                                         */
/*----------------------------------------------------------------------------*/
/* CRC32 bit                                                                  */
/* 使用的本原多项式为 : x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 + x^10 */
/*                      + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1                 */
/******************************************************************************/
//由于CRC校验经常需要移植到其它平台，所以用C语言编码实现

//  更新：侯思松  2003.8.15


#ifndef _CRC32_H_AA9B8699_DDAF_4F52_A35B_9D0BEFB202C1_DEFINE_
#define _CRC32_H_AA9B8699_DDAF_4F52_A35B_9D0BEFB202C1_DEFINE_

#ifdef __cplusplus
extern "C" {
#endif
    
#ifndef  UInt8
#define  UInt8   unsigned char
#endif
#ifndef  UInt32
#define  UInt32  unsigned long
#endif
    
    /* 计算CRC值*/
    UInt32 CRC32_GetCRC(const UInt8 * databuf,UInt32 databuflen);
    
    /* 得到CRC初始值*/
    UInt32 CRC32_Start(void);
    /* CRC计算结束*/
    UInt32 CRC32_Finish(UInt32 CRC);
    /*在原CRC的基础上增加一个字节的数据,并得到新的CRC值*/
    UInt32 CRC32_ProcessByte(UInt32 CRC,UInt8 b);
    /* 在原数据的CRC值的基础上增加数据,并得到新的CRC值 */
    UInt32 CRC32_Process(const UInt32 OldCRC,const UInt8 * databuf,UInt32 databuflen);
    
#ifdef __cplusplus
}
#endif

#endif /* _CRC32_H_AA9B8699_DDAF_4F52_A35B_9D0BEFB202C1_DEFINE_ */

