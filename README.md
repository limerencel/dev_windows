# Cheatsheet

## WinMain

- `LPSTR`: **(It's a type)** is a pointer to a string (_string of ANSI characters_), in the WinMain function, this parameter typically represents he command line argumenst passed to the application, if you run your application from the command line or with parameters, the string contains those parameters.
- `cmdShow`: is an `int` **(the type of this parameter)** that specifies how the window should be shown when the application starts. It's value can be one of serval predefined constants, such as:
- `lmCmdLine`: Type is `LPSTR`, which contains the command line arguments passed to the application when it is launched. For example, if user runs the application with the additional parameters like `myapp.exe -opstion vale`,`lpCmdLine` wold contain the string "-option value".

1. `SW_SHOW`: Activates and displays the window.
   2, `SW_HIDE`: Hides the window.
2. `SW_MINIMIZE`: Minimizes the window.
3. `SW_MAXIMIZE`: Maximizes the window.
   This parameter is used ton control the initial state of the main application window.

- `HINSTANCE`: is a handle to an insatnce of the application. It is a unique identifier for the application instance and is used by Windows operating system to manage resources allocated to that instance. Ths `hInstance` parameter in `WinMain` allows you to access resources(like icons, menus, etc) and manage application's lifecycle. For example, you can use it to load resources from the application's executable file.
- `hPrevInstance`: is a handle to the previous instance of the application, this parameter is always `NULL`, as Windwos does not allow multiple instances of the same application to run in the same session.

## WindowProc

## Steps of creating windows

1. Include necessary headers (windows.h).
2. Define a window procedure (WindowProc).
   _Handle messages (e.g., WM_PAINT, WM_DESTROY)_
3. Register the window class (RegisterWindowClass).
   _Define a WNDCLASS structure and register it using RegisterClass. This specifies the properties of the window (like the procedure to handle messages)._
4. Create the window (CreateMainWindow).
   _Use CreateWindowEx to instantiate the window based on the registered class. This defines its style, dimensions, and other attributes._
5. Implement the WinMain function to run the application.
   _The entry point for the application. Register the window class, create the window, show it, and enter the main message loop to process events._

---

## Handling Messages

In the Windows operating system, messages are sent and received as part of the **event-driven programming model**. Here's how it works:

### Who Sends the Messages

1. Windows OS:
   1. The Windows operating system itself generates messages based on user actions (like mouse clicks, keyboard input, etc.) and system events (like resizing, minimizing, etc.).
   2. For example, when a user clicks the close button on a window, the OS sends a WM_CLOSE message to the window.
2. Other Windows:
   1. Messages can also be sent from one window to another using functions like SendMessage or PostMessage.

### Who Receives the Messages

1. Window Procedure:
   1. Each window has an associated window procedure (defined by the developer), which is a callback function that processes the messages.
   2. The window procedure is responsible for handling the messages it receives, such as WM_PAINT, WM_KEYDOWN, and WM_DESTROY.

### Message Flow

1. **Message Generation**: The OS generates messages based on events (user actions or system changes).
2. **Message Dispatching**: The OS places these messages in the message queue for the specific window.
3. **Message Loop**: The application retrieves messages from the queue using functions like GetMessage.
4. **Message Handling**: The retrieved messages are passed to the window procedure for processing, where the developer defines how the application should respond to each type of message.

### Purpose of Message Loop

1. Message Retrieval:
   1. GetMessage(&msg, NULL, 0, 0) retrieves messages from the application's message queue. It waits for messages to arrive if the queue is empty.
   2. The function returns TRUE as long as there are messages to process. When it encounters a WM_QUIT message (indicating that the application should exit), it returns FALSE, breaking the loop.
2. Message Translation:
   1. TranslateMessage(&msg) translates virtual-key messages (like keyboard input) into character messages (like typing letters). This step is necessary for handling keyboard input correctly, as it converts key events into characters.
3. Message Dispatching:
   1. DispatchMessage(&msg) sends the message to the appropriate window procedure (the WindowProc function you defined). This is where the actual handling of the message takes place, allowing your application to respond to events like painting, resizing, or closing.

in a typical windows application, the `WinMain` function requires at least the `HINSTANCE` and `int` parameters, as per the standerd signatures.

---
Functions

### WndProc

`WndProc` function receives the messages from the operating system and the application itself. Each message corresponds to a specific event or action, such as mosue clicks, keyboard input, window resizing, etc.
You need to define how to handle the these messages by implementing the conditional statements within `WndProc`.
If you do not handle a specific message, you can call `DefWindowProc`, which provides default processing for unhandled messages.

### Explain _GetMessage_ function

1. The `GetMessage` function will return a non-zero value if a message is successfully retrieved from the queue.
2. if a message retrieved is `WM_QUIT`, `GetMessage` will return zero, Thid indicates that the application should terminate, and the message loop should exit.
3. Rare case: If there is an error or if the function fails, `GetMessage` will return a negative value.

```c++
BOOL GetMessage(
  LPMSG lpMsg,
  HWND   hWnd,
  UINT   wMsgFilterMin,
  UINT   wMsgFilterMax
);
```

- `LPMSG`: A pointer to an `MSG` structure that will receive message information.
- `HWND`: A handle to the window whose message you want to retrieve. If this parameter is `NULL`, it retrieves message for any window associated with the <u>current thread</u>.
- `wMsgFilterMin` and `wMsgFilterMax`: These parameters allow filtering of the message that are retrieved.



Brushes

```bash
g++ your_file.cpp -o your_program -mwindows -lgdi32
```

## Display Text
the outlines of best practice
1. Create a function to draw the text, including necessary parameters like text to display, the font, and the device context(HDC), which contains the actual text drawing code like `TextOut`or `DrawText`.
2. Call the function from `WM_PAINT`.
3. Update the text only when necessary.
4. Use a back buffer(optional). 