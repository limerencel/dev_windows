# PushBox Game

## Game Overview

![image-20241208162140877](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20241208162140877.png)

玩家操纵“红点”来推箱子，当所有的箱子都抵达带有蓝色边框的矩形里时，算作成功。

每次打开游戏或按'R'键时能刷新所有物体的位置。

## Technical Implementation

1. 使用Game.h将`WndProc`和`CreateWindow`分离出main.cpp

2. `Initialize` ： 配置游戏窗口，配置双缓冲，创建画笔画刷

3. `gen.seed(static_cast<unsigned int>(time(nullptr)))`： 根据当前的时间来创建随机数

4. ```c++
   	std::uniform_int_distribution<int> disX(1, BOARD_X - 2);  // 1 to 12 instead of 0 to 13
       std::uniform_int_distribution<int> disY(1, BOARD_Y - 2);  // 1 to 6 instead of 0 to 7
       
       Position pos;
       do {
           pos.x = disX(gen);
           pos.y = disY(gen);
       } while (IsPositionOccupied(pos, checkDestinations));
   ```

   `BOARD_X - 2`： 避免box出现在最外一层

​	while循环确保新的 boxes生成在没有被占领的位置上



## Q&A

*1. 为什么在推箱子过程中窗口会间歇性的闪烁？*

首先我想到的是*双缓冲*，为了实现我按照以下步骤

1. 在`Game.h`里创建`BufferDC`和`bufferBitmap`
2. 在`Game::Initialize`里初始化这些变量
3. 在`Game::Render`里，clear buffer, select object, drawing on bufferDC and copy buffer

但是这之后仍然会闪烁，最后我又突然想到可能是因为`InvalidateRect` ，查阅资料后我了解到

Windows程序中会产生一个``的消息，在以下几种情况中他会被触发：

- ```c++
  // Will trigger WM_ERASEBKGND
  InvalidateRect(hwnd, NULL, TRUE);  
  
  // Won't trigger WM_ERASEBKGND
  InvalidateRect(hwnd, NULL, FALSE);
  ```

- Window is first created
- Window is resized
- Window is uncovered (another window moved away)
- Window is restored from minimized state
- System theme changes
- BeginPaint is called without prior InvalidateRect

所以在我的代码中，我将`InvalidateRect`的最后一个参数设置为`FALSE`并处理`WM_ERASEBKGND`消息

```c++
case WM_ERASEBKGND:
    return 1;  // Tell Windows not to erase background
```

这下才算是解决了。



*2. 如何将WndProc作为一个类的成员函数？*

1. 首先在对应的类中声明`WndProc`函数，但必须是static。这是因为WIN API中的`WndProc`期望`function pointer`，更具体地说：

   - A **non-static member function** of a class has an implicit `this` pointer as its first parameter. Its signature is essentially:

     ```c++
     LRESULT CALLBACK WndProc(Game* this, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
     ```

     This doesn't match the required signature.

   - A **static member function** or a free function does not have the `this` pointer and matches the required signature, making it suitable for the Windows API.

   - The Windows API is written in C, not C++. It has no knowledge of C++ class structures or member functions.

   - As a result, it cannot call a non-static member function directly.

2. 现在有了`static WndProc`，下一步在对应的Cpp中实现它，大致如下：
   ```c++
   Game* Game::instance = nullptr;
   Game::Game() : startX(0), startY(0), hwnd(nullptr), blackPen(nullptr), 
                   bluePen(nullptr), hBitmap(nullptr), playerPos{0, 0},
                   bufferDC(nullptr), bufferBitmap(nullptr),
                   windowWidth(0), windowHeight(0),
                   gen(rd()) {
       instance = this;
   }
   ...
   LRESULT CALLBACK Game::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
       switch (message) {
           case WM_CREATE: {
               instance = new Game();
               if (!instance->Initialize(hWnd)) {
                   PostQuitMessage(1);
               }
               return 0;
           }
   ...
   ```

   解释：先把`nullptr`赋值给`instance`当`WndProc`收到`WM_CREATE`消息时实例化对象，此是对应类的`constructor`函数会被执行，在此处`this`会被传进instance，也就代表着instance指向当前的对象

*3. 那为什么还需要instance pointer?*

Windows API 不关心也不知道你的类，`WndProc`现在作为一个static函数，独立于任何实例，它并不会自动地访问类成员函数或实例中的数据。

当消息发送到你的窗口后，Windows API调用`WndProc`，只有在有instance pointer的情况下，把Game instance 和 当前的窗口联系起来，并处理相关的消息。

static `Wndproc`只是一个entry point，并不属于任何具体的实例，使用instance pointer，我们能在实例中实现更具体的功能。

例如：

```c++
case WM_PAINT: {
            if (!instance) return DefWindowProc(hWnd, message, wParam, lParam);
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            instance->Render(hdc);
            EndPaint(hWnd, &ps);
            return 0;
        }
```

此处我们就能调用当前对象的Render函数。

