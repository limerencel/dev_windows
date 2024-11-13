#include "CMySprite.h"

CMySprite::CMySprite() : m_hBitmap(NULL), m_hdcMem(NULL), m_x(0), m_y(0), m_width(0), m_height(0) {}

CMySprite::~CMySprite() {
  if (m_hBitmap) DeleteObject(m_hBitmap);
  if (m_hdcMem) DeleteDC(m_hdcMem);
}

bool CMySprite::LoadBitmap(HINSTANCE hInstance, const std::string& path) {
  m_hBitmap = (HBITMAP)LoadImage(hInstance, path.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
  if (!m_hBitmap) return false;
  
  BITMAP bm;
  GetObject(m_hBitmap, sizeof(bm), &bm);
  m_width = bm.bmWidth;
  m_height = bm.bmHeight;

  HDC hdc = GetDC(NULL);
  m_hdcMem = CreateCompatibleDC(hdc);
  SelectObject(m_hdcMem, m_hBitmap);
  //SelectObject doesn't directly modify the m_hdcMem variable itself, but it changes the state of the DC that m_hdcMem represents.

// The DC (m_hdcMem) is like an artist's easel with all their tools.
// The bitmap (m_hBitmap) is the canvas.
// SelectObject is the act of putting the canvas on the easel.
// Now, when you use this easel (DC), you're working on that specific canvas (bitmap).


  ReleaseDC(NULL, hdc);

  return true;
}

void CMySprite::SetPosition(int x, int y) {
  m_x = x;
  m_y = y;
}

void CMySprite::Render(HDC hdcDest) {
  BitBlt(
    hdcDest,
    m_x,
    m_y,
    m_width,
    m_height,
    m_hdcMem,
    0,
    0,
    SRCCOPY
  );
}