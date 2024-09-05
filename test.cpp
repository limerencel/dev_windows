#include <windows.h>

//1.define a windwow procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    case WM_PAINT:
      {
        PAINTSTRUCT ps; 
        //this variable "ps" is holding informationn about the painting process, by declaring it 
        // within the curly braces, it ensures that "ps" is only accessible within this block.Once the block is exited
        // "ps" is destroyed, and its memory is freed up. preventing any potential conflicts with other parts of the code.
        HDC hdc = BeginPaint(hwnd, &ps);
        // similarly, "hdc" is a handle to the device context, is also limited to this block, 
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        //fill a rectangle with a color, here "hdc" is the device context where the rectangular area will be painted.
        // This line fllls the area of the window that needs to be repainted 9as specified by `rePaint`) with specified color
        EndPaint(hwnd, &ps);
        // this line signals the Windows that painting is complete, allowing it to perfrom any necssary
        //housekeeping and release resources associated with the painting operation.
      }
      return 0;
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


// the entry point of the application
int WINAPI WinMain(HINSTANCE hInstacne, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  const char CLASS_NAME[] = "Smple Window Class";

  WNDCLASS wc = {};
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = hInstacne;
  wc.lpszClassName = CLASS_NAME;

  RegisterClass(&wc);

  // create the Window
  HWND hwnd = CreateWindowEx(
    0,
    CLASS_NAME,
    "My Simple Window",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,
    640,
    480,
    NULL,
    NULL,
    hInstacne,
    NULL
  );

  if (hwnd == NULL) {
    return 0;
  }

  ShowWindow(hwnd, nCmdShow);

  //Run the message loop
  MSG msg = {};
  while (GetMessage(&msg, NULL, 0,0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return 0;
}