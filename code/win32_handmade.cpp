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

struct win32_offscreen_buffer
{
	BITMAPINFO Info;
	void *Memory;
	int Width;
	int Height;
	int Pitch;
};

struct win32_window_dimension
{
	int Width;
	int Height;
};

// TODO: Global for now. Will go out eventually
global_variable bool Running;
global_variable win32_offscreen_buffer GlobalBackbuffer;
 
win32_window_dimension 
Win32GetWindowDimension(HWND Window)
{
	win32_window_dimension WindowDimension;
	RECT ClientRect;
	GetClientRect(Window, &ClientRect);
	WindowDimension.Width   = ClientRect.right - ClientRect.left;
	WindowDimension.Height  = ClientRect.bottom - ClientRect.top;

	return (WindowDimension);
}

internal void 
RenderWeirdGradient(win32_offscreen_buffer Buffer, int XOffset, int YOffset)
{
	uint8 *Row = (uint8 *)Buffer.Memory;
	for( int Y = 0; Y < Buffer.Height; ++Y)
	{
		uint32 *Pixel = (uint32 *)Row;
		for( int X = 0; X < Buffer.Width; ++X)
		{	
			// Pixel in memry : 00 00 00 00 
			// 					BB GG RR xx 
			uint8 Blue  = (uint8) (X+XOffset);
			uint8 Green = (uint8) (Y+YOffset);
			uint8 Red   = (uint8) 0;

			*Pixel++ = (( Red << 16) | (Green << 8) | Blue);
		}
		Row += Buffer.Pitch;
	}
}  

internal void 
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height) 
{
	// TODO: Bulletproof this;
	// Maybe dont free first, free after, and then free if that doenst fail;
	if(Buffer->Memory)
	{
		VirtualFree(Buffer->Memory,0,MEM_RELEASE);
	}	

	Buffer->Width  = Width;
	Buffer->Height = Height;
	int BytesPerPixel = 4;

	Buffer->Info.bmiHeader.biSize   		 = sizeof(Buffer->Info.bmiHeader);
	Buffer->Info.bmiHeader.biWidth  		 = Buffer->Width;
	Buffer->Info.bmiHeader.biHeight 		 = -Buffer->Height;
	Buffer->Info.bmiHeader.biPlanes 		 = 1;
	Buffer->Info.bmiHeader.biBitCount 	     = 32;
	Buffer->Info.bmiHeader.biCompression 	 = BI_RGB;

	// Allocating Memory
	
	int BitmapMemorySize = (Buffer->Width*Buffer->Height) * BytesPerPixel;
	Buffer->Memory = VirtualAlloc(0,BitmapMemorySize,MEM_COMMIT,PAGE_READWRITE);
	
	Buffer->Pitch = Width*BytesPerPixel;
	// Probably clear to black
}

internal void
Win32DisplayBufferInWindow(HDC DeviceContext, int WindowWidth, int WindowHeight, 
						   win32_offscreen_buffer Buffer)
{
	// TODO: Aspect Ratio correction
	// TODO: Play with stretch modes
	StretchDIBits(DeviceContext, 
				  /*X, Y, Width, Height,
				  X, Y, Width, Height,*/
				  0, 0, WindowWidth, WindowHeight,
				  0, 0, Buffer.Width, Buffer.Height,
 				  Buffer.Memory,
  				  &Buffer.Info,
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

			win32_window_dimension WindowDimension = Win32GetWindowDimension(Window);
			Win32DisplayBufferInWindow(DeviceContext, WindowDimension.Width, WindowDimension.Height, 
									   GlobalBackbuffer);

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

	Win32ResizeDIBSection(&GlobalBackbuffer, 1280, 720);

	WindowClass.style 			= CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
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
			// NOTE: Since we specifies CS_OWNDC we can just get one device context
			// and keep it forever
			HDC DeviceContext = GetDC(Window);

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

				RenderWeirdGradient(GlobalBackbuffer, XOffset, YOffset);
				
				win32_window_dimension WindowDimension = Win32GetWindowDimension(Window);
				Win32DisplayBufferInWindow(DeviceContext, WindowDimension.Width, WindowDimension.Height, 
										   GlobalBackbuffer);

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