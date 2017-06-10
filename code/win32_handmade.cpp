/*
* @Author: Puertouncho
* @Date:   2016-12-17 08:32:31
* @Last Modified by:   Puertouncho
* @Last Modified time: 2016-12-19 11:35:54
*/

#include <windows.h>


LRESULT CALLBACK
MainWindowCallback(HWND   Window,
				   UINT   Message,
				   WPARAM WParam,
				   LPARAM LParam )
{
	LRESULT Result = 0;

	switch(Message)
	{
		case WM_SIZE:
		{
			OutputDebugStringA("WM_SIZE\n");
		}
		break;

		case WM_DESTROY:
		{
			OutputDebugStringA("WH_DESTROY\n");
		}
		break;

		case WM_CLOSE:
		{
			OutputDebugStringA("WH_CLOSE\n");
		}
		break;

		case WM_PAINT:
		{
			OutputDebugStringA("WM_PAINT\n");
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);

			int X = Paint.rcPaint.left;
			int Y = Paint.rcPaint.bottom;
			LONG Height = Paint.rcPaint.bottom - Paint.rcPaint.top; 
			LONG Width  = Paint.rcPaint.right - Paint.rcPaint.left; 

			static DWORD Operation = WHITENESS;
			if(Operation == WHITENESS)
			{
				Operation = BLACKNESS;
			}
			else
			{
				Operation = WHITENESS;
			}
			PatBlt(DeviceContext,X, Y, Width, Height, Operation);

			EndPaint(Window, &Paint);
		}
		break;

		case WM_ACTIVATEAPP:
		{
			OutputDebugStringA("WH_ACTIVATEAPP\n");
		}
		break;

		default:
		{
			//OutputDebugStringA("default\n");
			Result = DefWindowProc( Window,Message,WParam,LParam);
		}
		break;
	}
	return Result;
}

int CALLBACK WinMain( HINSTANCE Instance,
  					  HINSTANCE PrevInstance,
  					  LPSTR     CmdLine,
  					  int       CmdShow) 
{
	WNDCLASS WindowClass = {};
	WindowClass.style 			= CS_OWNDC;
	WindowClass.lpfnWndProc 	= MainWindowCallback;
	WindowClass.hInstance 		= Instance;
	WindowClass.lpszClassName 	= "HandmadeHeroWindowClass"; 

	if(RegisterClassA(&WindowClass))
	{
		HWND WindowHandle = CreateWindowExA(0,
								  		    WindowClass.lpszClassName,
								  		    "Handmade Hero",
								  		    WS_OVERLAPPEDWINDOW|WS_VISIBLE,
								  		    CW_USEDEFAULT,
								 		    CW_USEDEFAULT,
								  		    CW_USEDEFAULT,
								  		    CW_USEDEFAULT,
								 		    0,
								  		    0,
								  		    Instance,
								  		    0);
		if(WindowHandle)
		{
			MSG Message;
			while(true)
			{
				BOOL MessageResult = GetMessageA(&Message,0,0,0);
				if(MessageResult >0)
				{
					TranslateMessage(&Message);
					DispatchMessageA(&Message);
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			// TODO: Logging
		}
	}
	else
	{
		// TODO: Logging
	}

	return (0);
}	