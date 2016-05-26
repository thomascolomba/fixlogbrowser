//this file is part of notepad++
//Copyright (C)2003 Don HO <donho@altern.org>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include <stdio.h>
#include "PluginDefinition.h"

extern NppData				nppData;
extern FuncItem			funcItem[1];
HANDLE				g_hModule			= NULL;

void toggleView(void){
	printf("---TCO toggleView");
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  reasonForCall, 
                       LPVOID lpReserved )
{
    g_hModule = hModule;
    switch (reasonForCall)
    {
      case DLL_PROCESS_ATTACH:
      	// Code to run when the DLL is loaded

        /*pluginInit(hModule);
        
        funcItem[0]._pFunc = toggleView;
        funcItem[0]._pShKey = new ShortcutKey;
	funcItem[0]._pShKey->_isAlt	= true;
	funcItem[0]._pShKey->_isCtrl	= false;
	funcItem[0]._pShKey->_isShift	= true;
	funcItem[0]._pShKey->_key	= TEXT('F');
        lstrcpy( funcItem[0]._itemName, TEXT("&toggleView menu name") );*/
        
        break;

      case DLL_PROCESS_DETACH:
      // Code to run when the DLL is freed
        printf ("Unload working...\n");
        pluginCleanUp();
        break;

      case DLL_THREAD_ATTACH:
      // Code to run when a thread is created during the DLL's lifetime
        printf ("ThreadLoad working...\n");
        break;

      case DLL_THREAD_DETACH:
      // Code to run when a thread ends normally.
        printf ("ThreadUnload working...\n");
        break;
    }

    return TRUE;
}


extern "C" __declspec(dllexport) void setInfo(NppData notpadPlusData)
{
	nppData = notpadPlusData;
	commandMenuInit();
}

extern "C" __declspec(dllexport) const TCHAR * getName()
{
	return NPP_PLUGIN_NAME;
}

extern "C" __declspec(dllexport) FuncItem * getFuncsArray(int *nbF)
{
	*nbF = nbFunc;
	return funcItem;
}


extern "C" __declspec(dllexport) void beNotified(SCNotification *notifyCode)
{
	switch (notifyCode->nmhdr.code) 
	{
	
		case NPPN_READY:
			printf("---TCO debug\n");
		break;
	
		case NPPN_SHUTDOWN:
		{
			commandMenuCleanUp();
		}
		break;

		default:
			return;
	}
}



// Here you can process the Npp Messages 
// I will make the messages accessible little by little, according to the need of plugin development.
// Please let me know if you need to access to some messages :
// http://sourceforge.net/forum/forum.php?forum_id=482781
//
extern "C" __declspec(dllexport) LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam)
{
	printf("messageProc() Message = %i, wParam = %i, lParam = %li \n", Message, wParam, lParam);
	return TRUE;
}

#ifdef UNICODE
extern "C" __declspec(dllexport) BOOL isUnicode()
{
    return TRUE;
}
#endif //UNICODE
