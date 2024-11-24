#include <windows.h>
#include <string>

class CMySprite {
  private:
    HBITMAP m_hBitmap; // m: member of a class 
    HDC m_hdcMem;
    int m_x, m_y;
    int m_width, m_height;
  public:
    CMySprite();
    virtual ~CMySprite();

    bool LoadBitmapFromResource(HINSTANCE hInstacne, UINT resourceID);
    void SetPosition(int x, int y);
    virtual void Render(HDC hdcDest);

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    int GetX() const { return m_x; }
    int GetY() const { return m_y; }
    HDC GetMemDC() const { return m_hdcMem; }
};