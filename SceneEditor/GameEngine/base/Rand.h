//Rand.h

#ifndef _Rand_h_
#define _Rand_h_

//随机数发生器

class VRand{ 
public: 
    static inline long getRandMax(){ return _RAND_MAX; }
private:
    enum {  _RAND_MAX = (1<<16)-1 };
    long m_holdrand;
    inline void init(){
        m_holdrand=1;
    }
public:
    VRand(){ init(); }
    inline long next(){//15bit
        long result=m_holdrand * 214013 + 2531011;
        m_holdrand =result;
        return  ( (result>>16) & _RAND_MAX );
    }
    inline double next_f(){
        static const double max_R=(1.0/VRand::getRandMax());
        return next()*max_R;
    }
    inline void setSeed(long seed){
        m_holdrand=seed;
    }
    inline long getSeed() const{
        return m_holdrand;
    }
    void setSeedByNowTime();
};

class VRandLong{ 
public:
    static inline long getRandMax(){ return (VRand::getRandMax()<<15) | VRand::getRandMax(); }
private:
    VRand  m_rand;
public:
    VRandLong(){ }
    inline long Range(long min, long max)//(value >= min) && (value < max)
    {
        if (min < max)
        {
            long result = next() % (max - min);
            result += min;
            return result;
        }
        return min;
    }
    inline long next(){//30bit
        return (m_rand.next()<<15) | m_rand.next();
    }
    inline double next_f(){
        static const double max_R=(1.0/VRandLong::getRandMax());
        return next()*max_R;
    }
    inline void setSeed(long seed){
        m_rand.setSeed(seed);
    }
    inline long getSeed() const{
        return m_rand.getSeed();
    }
    inline void setSeedByNowTime(){
        m_rand.setSeedByNowTime();
    }
    
    inline unsigned long nextUInt32Value(){ //32bit
        return (unsigned long)( (m_rand.next()<<30) | (m_rand.next()<<15) | m_rand.next() );
    }
};

#endif //_Rand_h_
