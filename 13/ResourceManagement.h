#include <windows.h>
#include <string>

class ResourceManager {
  private:
    HBITMAP hBitmap;
    std::string bitmapPath;

    void loadBitmap(const std::string& path) {
      hBitmap = (HBITMAP)LoadImage(NULL, path.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
      if (hBitmap == NULL) {
        MessageBox(NULL, "Error loading bitmap", "Error", MB_OK);
      }
    }

  public:
    ResourceManager(const std::string& path) : hBitmap(NULL), bitmapPath(path) {}

    ~ResourceManager() {
      if (hBitmap) {
        DeleteObject(hBitmap);
      }
    }

    HBITMAP get() {
      if (hBitmap == NULL) {
        loadBitmap(bitmapPath);
      }
      return hBitmap;
    }

    void draw(HDC hdc, int x, int y, int width, int height) {
        if (hBitmap) {
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
            BitBlt(hdc, x, y, width, height, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, oldBitmap);
            DeleteDC(hdcMem);
        }
    }
};