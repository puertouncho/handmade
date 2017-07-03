/*
* @Author: Puertouncho
* @Date:   2016-12-17 08:32:31
* @Last Modified by:   skala
* @Last Modified time: 2017-07-03 21:08:52
*/

#include <windows.h>
#include <stdint.h>
#include <xinput.h>

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
global_variable bool GlobalRunning;
global_variable win32_offscreen_buffer GlobalBackbuffer;

// NOTE: XInputSetState
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
	return(0);
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

// NOTE: XInputGetState
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
	return(0);
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_


internal void
Win32LoadXInput(void)
{
	HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");
	if(XInputLibrary)
	{
		XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
		XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
	}
	else 
	{
		XInputLibrary = LoadLibraryA("xinput1_3.dll");
		if(XInputLibrary)
		{
			XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
			XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
		}
	}
} 

internal win32_window_dimension 
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
RenderWeirdGradient(win32_offscreen_buffer *Buffer, int XOffset, int YOffset)
{
	uint8 *Row = (uint8 *)Buffer->Memory;
	for( int Y = 0; Y < Buffer->Height; ++Y)
	{
		uint32 *Pixel = (uint32 *)Row;
		for( int X = 0; X < Buffer->Width; ++X)
		{	
			// Pixel in memry : 00 00 00 00 
			// 					BB GG RR xx 
			uint8 Blue  = (uint8) (X+XOffset);
			uint8 Green = (uint8) (Y+YOffset);
			uint8 Red   = (uint8) 0;

			*Pixel++ = (( Red << 16) | (Green << 8) | Blue);
		}
		Row += Buffer->Pitch;
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
						   win32_offscreen_buffer *Buffer)
{
	// TODO: Aspect Ratio correction
	// TODO: Play with stretch modes
	StretchDIBits(DeviceContext, 
				  0, 0, WindowWidth, WindowHeight,
				  0, 0, Buffer->Width, Buffer->Height,
 				  Buffer->Memory,
  				  &Buffer->Info,
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
			GlobalRunning = false;
		}
		break;

		case WM_CLOSE:
		{
			// TODO: Handle this with a message to user
			GlobalRunning = false;
			OutputDebugStringA("WH_CLOSE\n");
		}
		break;

		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			uint32 VKCode = WParam;
			bool WasDown  = ((LParam & (1<<30)) != 0);
			bool IsDown   = ((LParam & (1<<31)) == 0);
			switch(VKCode)
			{
				case 'A':
				{

				}
				break;
				case 'S':
				{

				}
				break;
				case 'D':
				{

				}
				break;
				case 'W':
				{

				}
				break;
				case 'Q':
				{

				}
				break;
				case 'E':
				{

				}
				break;
				case VK_LEFT:
				{

				}
				break;
				case VK_RIGHT:
				{

				}
				break;
				case VK_UP:
				{

				}
				break;
				case VK_DOWN:
				{

				}
				break;
				case VK_SPACE:
				{

				}
				break;
				case VK_ESCAPE:
				{

				}
				break;
			}
		}
		break;

		case WM_PAINT:
		{
			//OutputDebugStringA("WM_PAINT\n");
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);

			win32_window_dimension WindowDimension = Win32GetWindowDimension(Window);
			Win32DisplayBufferInWindow(DeviceContext, WindowDimension.Width, WindowDimension.Height, 
									   &GlobalBackbuffer);

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
	Win32LoadXInput();
	WNDCLASSA WindowClass = {};

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
			// NOTE: Since we specified CS_OWNDC we can just get one device context
			// and keep it forever
			HDC DeviceContext = GetDC(Window);

			GlobalRunning = true;
			int XOffset = 0;
			int YOffset = 0;
			while(GlobalRunning)
			{
				MSG Message;
				while(PeekMessage(&Message,0,0,0,PM_REMOVE))
				{
					if(Message.message == WM_QUIT)
					{
						GlobalRunning = false;
					}
					TranslateMessage(&Message);
					DispatchMessageA(&Message);
				}

				// TODO : Shjould we poll this more frequenly  
				for (DWORD ControllerIndex=0; ControllerIndex< XUSER_MAX_COUNT; ControllerIndex++ )
				{
					XINPUT_STATE ControllerState;
					if( XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS )
					{
						// NOTE: This controller is plugged in
						// TODO: Take a look at ControllerState.dwPacketNumber, see if increments too rapidly
						XINPUT_GAMEPAD *Gamepad = &ControllerState.Gamepad;
						
						bool DPadUp            = (Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
						bool DPadDown          = (Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
						bool DPadLeft          = (Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
						bool DPadRight         = (Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
						bool DPadStart         = (Gamepad->wButtons & XINPUT_GAMEPAD_START);
						bool DPadBack          = (Gamepad->wButtons & XINPUT_GAMEPAD_BACK);
						bool DPadLeftShoulder  = (Gamepad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
						bool DPadRightShoulder = (Gamepad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
						bool DPadA 			   = (Gamepad->wButtons & XINPUT_GAMEPAD_A);
						bool DPadB 			   = (Gamepad->wButtons & XINPUT_GAMEPAD_B);
						bool DPadX 			   = (Gamepad->wButtons & XINPUT_GAMEPAD_X);
						bool DPadY 			   = (Gamepad->wButtons & XINPUT_GAMEPAD_Y);

						int16 StickX = Gamepad->sThumbLX;
						int16 StickY = Gamepad->sThumbLY;

						if(DPadA)
						{
							XOffset++;
						}
						if(DPadY)
						{
							YOffset++;
						}
					}
					else
					{
						// NOTE: The controller is not available
					}
				} 

				RenderWeirdGradient(&GlobalBackbuffer, XOffset, YOffset);
				
				win32_window_dimension WindowDimension = Win32GetWindowDimension(Window);
				Win32DisplayBufferInWindow(DeviceContext, WindowDimension.Width, WindowDimension.Height, 
										   &GlobalBackbuffer);

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