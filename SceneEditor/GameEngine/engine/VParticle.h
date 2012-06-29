/*
 *  VParticle.h
 *
 *  Created by housisong on 08-6-26.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

//粒子系统  用以控制大量的小图片的绘制

#ifndef _VParticle_h_
#define _VParticle_h_
//TODO:
/*
 #include "VSprite.h"
 
 struct TParticleBase{
 bool   isLiving;  //是否是活的
 double life_s;    //剩余存活时间(秒)
 inline TParticleBase():isLiving(false){}
 inline TParticleBase(const TParticleBase& value){ *this=value; }
 inline void update(double stepTime_s){
 life_s-=stepTime_s;
 isLiving=(life_s>0);
 }
 };
 
 struct TParticle:public TParticleBase{
 double x;         //位置
 double y;
 double xV_s;      //速度 (/秒)
 double yV_s;
 inline TParticle(){}
 inline TParticle(const TParticle& value){ *this=value; }
 inline void update(double stepTime_s){
 TParticleBase::update(stepTime_s);
 x+=xV_s*stepTime_s;
 y+=yV_s*stepTime_s;
 }
 };
 
 template<class TParticleType>
 class VParticleSys:public VSprite{
 protected:
 typedef Vector<TParticleType> TParticleList;
 TParticleList m_particleList;
 public:
 VParticleSys(long maxParticleCount){ m_particleList.resize(maxParticleCount); }	
 virtual void doUpdate(double stepTime_s){
 long size=m_particleList.size();
 for (long i=0;i<size;++i){
 if (m_particleList[i].isLiving){
 m_particleList[i].update(stepTime_s);
 }
 }						
 }
 };
 
 class VColorParticle:public TParticleBase{
 Color32 color;
 long    left;
 long    top;
 public:
 VColorParticle(){ }
 VColorParticle(const VColorParticle& value){ *this=value; }
 }
 
 */

#endif //_VParticle_h_


