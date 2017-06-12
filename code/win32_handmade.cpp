/*
* @Author: Puertouncho
* @Date:   2016-12-17 08:32:31
* @Last Modified by:   Puertouncho
* @Last Modified time: 2016-12-19 11:35:54
*/

#include <windows.h>
#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

// TODO: Global for now. Will go out eventually
global_variable bool Running;
global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;
global_variable int BytesPerPixel = 4;

internal void 
RenderWeirdGradient(int XOffset, int YOffset)
{
	int Pitch = BitmapWidth*BytesPerPixel;
	uint8 *Row = (uint8 *)BitmapMemory;
	for( int Y = 0; Y < BitmapHeight; ++Y)
	{
		uint32 *Pixel = (uint32 *)Row;
		for( int X = 0; X < BitmapWidth; ++X)
		{	
			// Pixel in memry : 00 00 00 00 
			// 					BB GG RR xx 
			uint8 Blue  = (uint8) (X+XOffset);
			uint8 Green = (uint8) (Y+YOffset);

			*Pixel++ = ((Green << 8) | Blue);
		}
		Row += Pitch;
	}
}  

internal void 
Win32ResizeDIBSection(int Width, int Height) 
{
	// TODO: Bulletproof this;
	// Maybe dont free first, free after, and then free if that doenst fail;
	if(BitmapMemory)
	{
		VirtualFree(BitmapMemory,0,MEM_RELEASE);
	}	

	BitmapWidth  = Width;
	BitmapHeight = Height;

	BitmapInfo.bmiHeader.biSize   		 = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth  		 = BitmapWidth;
	BitmapInfo.bmiHeader.biHeight 		 = -BitmapHeight;
	BitmapInfo.bmiHeader.biPlanes 		 = 1;
	BitmapInfo.bmiHeader.biBitCount 	 = 32;
	BitmapInfo.bmiHeader.biCompression 	 = BI_RGB;

	// Allocating Memory
	
	int BitmapMemorySize = (BitmapWidth*BitmapHeight) * BytesPerPixel;
	BitmapMemory = VirtualAlloc(0,BitmapMemorySize,MEM_COMMIT,PAGE_READWRITE);
	
	// Probably clear to black
}

internal void
Win32UpdateWindow(HDC DeviceContext, RECT *WindowRect, int X, int Y, int Width, int Height)
{
	int WindowWidth  = WindowRect->right - WindowRect->left;
	int WidnowHeight = WindowRect->bottom - WindowRect->top;
	StretchDIBits(DeviceContext, 
				  /*X, Y, Width, Height,
				  X, Y, Width, Height,*/
				  0,0,BitmapWidth, BitmapHeight,
				  0,0,WindowWidth, WidnowHeight,
 				  BitmapMemory,
  				  &BitmapInfo,
  				  DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK
Win32MainWindowCallback(HWND   Window,
				   		UINT   Message,
				   		WPARAM WParam,
				   		LPARAM LParam )
{
	LRESULT Result = 0;

	switch(Message)
	{
		case WM_SIZE:
		{
			RECT ClientRect;
			GetClientRect(Window, &ClientRect);
			LONG Height = ClientRect.bottom - ClientRect.top; 
			LONG Width  = ClientRect.right -ClientRect.left; 
			Win32ResizeDIBSection(Width, Height);
			OutputDebugStringA("WM_SIZE\n");
		}
		break;

		case WM_DESTROY:
		{
			// TODO: Handle this as an error - recreate window
			Running = false;
		}
		break;

		case WM_CLOSE:
		{
			// TODO: Handle this with a message to user
			Running = false;
			OutputDebugStringA("WH_CLOSE\n");
		}
		break;

		case WM_PAINT:
		{
			//OutputDebugStringA("WM_PAINT\n");
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);

			int X = Paint.rcPaint.left;
			int Y = Paint.rcPaint.bottom;
			LONG Height = Paint.rcPaint.bottom - Paint.rcPaint.top; 
			LONG Width  = Paint.rcPaint.right - Paint.rcPaint.left; 

			RECT ClientRect;
			GetClientRect(Window, &ClientRect);

			Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);

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
	WindowClass.lpfnWndProc 	= Win32MainWindowCallback;
	WindowClass.hInstance 		= Instance;
	WindowClass.lpszClassName 	= "HandmadeHeroWindowClass"; 

	if(RegisterClassA(&WindowClass))
	{
		HWND Window = CreateWindowExA(0,
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
		if(Window)
		{
			Running = true;
			int XOffset = 0;
			int YOffset = 0;
			while(Running)
			{
				MSG Message;
				while(PeekMessage(&Message,0,0,0,PM_REMOVE))
				{
					if(Message.message == WM_QUIT)
					{
						Running = false;
					}
					TranslateMessage(&Message);
					DispatchMessageA(&Message);
				}

				RenderWeirdGradient(XOffset, YOffset);
				HDC DeviceContext = GetDC(Window);
				RECT ClientRect;
				GetClientRect(Window, &ClientRect);
				int WindowWidth   = ClientRect.right - ClientRect.left;
				int WidnowHeight  = ClientRect.bottom - ClientRect.top;
				Win32UpdateWindow(DeviceContext, &ClientRect, 0, 0, WindowWidth, WidnowHeight);
				ReleaseDC(Window, DeviceContext);

				XOffset++;
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