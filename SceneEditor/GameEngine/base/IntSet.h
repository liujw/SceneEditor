//IntSet.h

#ifndef _IntSet_h_
#define _IntSet_h_

//用整数实现的比特Set

template<class TUInt>
struct TBitSet{
private:
    inline static TUInt valueCvs(long value) { return (((TUInt)1)<<value); }
    inline static TUInt valueCvs(long valueBegin,long valueEnd) { 
        TUInt result=!(TUInt)0;
        result<<=(sizeof(TUInt)*8-valueEnd);
        result>>=(sizeof(TUInt)*8-valueEnd+valueBegin);
        result<<=valueBegin;
        return result;
    }
public:
    TUInt values;
    
    inline TBitSet():values(0){}
    inline TBitSet(const TBitSet& bitSet):values(bitSet.values){}
    inline TBitSet(long valueBegin,long valueEnd):values(0){ insert(valueBegin,valueEnd); }
    
    inline bool empty()const			{ return (values==0); }
    inline void swap(TBitSet& bitSet)	{ TUInt tmp(values); values=bitSet.values; bitSet.values=tmp; }
    inline void clear()					{ values=0; }
    inline bool operator==(const TBitSet& bitSet)const	{ return values==bitSet.values;  }
    inline bool operator!=(const TBitSet& bitSet)const	{ return !(*this==bitSet);  }
    inline static bool find(const TUInt set,long value) { return (set&valueCvs(value))!=0; }
    inline bool find(long value)const	{ return find(values,value); }
    inline void insert(long value)						{ values|=valueCvs(value); }
    inline void insert(long valueBegin,long valueEnd)	{ values|=valueCvs(valueBegin,valueEnd); }
    inline void insert(const TBitSet& bitSet)			{ values|=bitSet.values; }
    inline void erase(long value)						{ values&=(!valueCvs(value)); }
    inline void erase(long valueBegin,long valueEnd)	{ values&=!valueCvs(valueBegin,valueEnd); }
    inline void erase(const TBitSet& bitSet)			{ values&=!bitSet.values; }
};

#endif //_IntSet_h_
