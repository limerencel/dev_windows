# Cheatsheet

## WinMain
- `LPSTR`: **(It's a type)** is a pointer to a string (*string of ANSI characters*), in the WinMain function, this parameter typically represents he command line argumenst passed to the application, if you run your application from the command line or with parameters, the string contains those parameters.
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
1. Define a Window Procedure 
*the window procedure is a function that process messages sent to your window. You'll need to define it before you create the window*
2. Create the `WinMain` Funcction


---
in a typical windows application, the `WinMain` function requires at least the `HINSTANCE` and `int` parameters, as per the standerd signatures.