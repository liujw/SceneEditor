class CGameSprite
{
public:
	CGameSprite(CWnd* pWnd, HDC hDC);
	virtual ~CGameSprite();

public:
	
	
 private:
	 Bitmap* CreateBitmap(Bitmap* pBitmap,int nWidth,int nHeight)
	 {
		 if(pBitmap) 
		 	delete pBitmap;
		
		 pBitmap = new Bitmap(nWidth, nHeight);

		 return pBitmap;
	 }
	 
protected:
	BOOL CreateMemBitmap();
	void Initial();
	void DrawMemBitmap(Graphics* g);
	
// Attributes
private:
	RectF m_drawRect;
	CRect m_borderRect;
	CRect m_clientRect;
	HDC	  m_hDC;
	CWnd* m_pMainWnd; 

	Bitmap* m_pCoBmp;   //memory

	int m_nClientWidth;
	int m_nClientHeight;

	BOOL m_bReDraw;
	BOOL m_bInitialed;

	CDC m_memDC;
	CDC m_gDC;
	CBitmap m_memBitmap;
	CBitmap* m_pOldBitmap;
};

