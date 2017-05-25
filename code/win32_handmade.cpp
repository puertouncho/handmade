/*
* @Author: Puertouncho
* @Date:   2016-12-17 08:32:31
* @Last Modified by:   Puertouncho
* @Last Modified time: 2016-12-19 11:35:54
*/

#include <windows.h>

int CALLBACK WinMain( HINSTANCE hInstance,
  					  HINSTANCE hPrevInstance,
  					  LPSTR     lpCmdLine,
  					  int       nCmdShow) 
{
	MessageBoxExA(0, "Hand Made Hero","Handmade Title", 
				  MB_OK|MB_ICONINFORMATION, 0 );
	return (0);
}	