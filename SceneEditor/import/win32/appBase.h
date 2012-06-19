//appBase.h
#ifndef _appBase_h_
#define _appBase_h_

#include "../importInclude/IUpdateListenerBase.h"

class TFormBase{
private:
    void* m_data;
protected:
    inline void* getPrivateData(){ return m_data; }
public:
    TFormBase(const char* caption,long clientWidth,long clientHeight,long nCmdShow,const char* icon=0,const char* cursor=0);
	void setUpdateListener(const IUpdateListenerBase& updateListener);
    virtual ~TFormBase();
    virtual void formCreate(){}
    virtual void formDestroy(){}

    const char* getCaption()const;
    void setCaption(const char* caption);

    void getClientRect(long& x0,long& y0,long& x1,long& y1) const;
    void setClientRect(long x0,long y0,long x1,long y1);

    long getLeft()const{
        long x0,y0,x1,y1;
        getClientRect(x0,y0,x1,y1);
        return x0;
    }
    void setLeft(long left){
        long x0,y0,x1,y1;
        getClientRect(x0,y0,x1,y1);
        setClientRect(left,y0,left+(x1-x0),y1);
    }

    long getTop()const{
        long x0,y0,x1,y1;
        getClientRect(x0,y0,x1,y1);
        return y0;
    }
    void setTop(long top){
        long x0,y0,x1,y1;
        getClientRect(x0,y0,x1,y1);
        setClientRect(x0,top,x1,top+(y1-y0));
    }

    long getWidth()const{
        long x0,y0,x1,y1;
        getClientRect(x0,y0,x1,y1);
        return x1-x0;
    }
    void setWidth(long width){
        long x0,y0,x1,y1;
        getClientRect(x0,y0,x1,y1);
        setClientRect(x0,y0,x0+width,y1);
    }

    long getHeight()const{
        long x0,y0,x1,y1;
        getClientRect(x0,y0,x1,y1);
        return y1-y0;
    }
    void setHeight(long height){
        long x0,y0,x1,y1;
        getClientRect(x0,y0,x1,y1);
        setClientRect(x0,y0,x1,y0+height);
    }

	void setIconRes(const char* resID);
	void setCursorRes(const char* resID);
};

class TApplicationBase{
private:
    void* m_data;
    virtual void registForm(TFormBase* aForm);
    virtual void unRegistForm(TFormBase* aForm);
    friend TFormBase;
protected:
    inline void* getPrivateData(){ return m_data; }
public:
    TApplicationBase();
    virtual ~TApplicationBase();

    virtual void applicationCreate(){}
    virtual void applicationDestroy(){}

    static TFormBase* getMainForm();
    static TApplicationBase* getApplication();
    static const char* getCmdLine();
	static void getFullScreenSize(long& out_screenWidth,long& out_screenHeight);
};

#endif  //#define _appBase_h_
