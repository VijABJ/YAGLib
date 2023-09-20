
#include "GjNativeApp.h"
#include "GjPoints.h"
#include "GjWin32Window.h"
#include <cstdlib>
using namespace yaglib;

#pragma comment (lib, "YAGSupport.lib")

// Global Variables:
Point mouseLocation(0, 0);

void callback(NativeApplicationEvent event, NATIVE_WINDOW_HANDLE hWindow, void* params)
{
  if(params != NULL)
  {
    Win32Params* appMessage = (Win32Params*)params;
    switch(appMessage->msg)
    {
      case WM_PAINT:
        {
          PAINTSTRUCT ps;
          BeginPaint(hWindow, &ps);

          RECT rect;
          GetClientRect(hWindow, &rect);

          HPEN hOldPen = (HPEN) SelectObject (ps.hdc, GetStockObject (NULL_PEN));
          HBRUSH hBrush = CreateSolidBrush (RGB (128, 128, 128));
          FillRect (ps.hdc, &rect, hBrush);

          TCHAR cBuffer [128];
          wsprintf(cBuffer, L"Window Size (%d, %d), Mouse Position (%d, %d)", 
            rect.right, rect.bottom, mouseLocation.x, mouseLocation.y);
          WideString text(cBuffer);
          DrawText(ps.hdc, text.c_str(), (int)text.size(), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

          SelectObject(ps.hdc, hOldPen);
          DeleteObject(hBrush);

          EndPaint(hWindow, &ps);
        }
        break;
      case WM_MOUSEMOVE:
        {
          mouseLocation = Point((int)LOWORD(appMessage->lParam), (int)HIWORD(appMessage->lParam));
          InvalidateRect(hWindow, NULL, TRUE);
        }
        break;
    }
  }

  return;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  NativeApplication app(hInstance);

  app.subscribe(yaglib::InterfaceIntercept(callback), L"Main", true);
  app.addWindow(new yaglib::Win32WindowWrapper(
    L"Default", L"TApplication", hInstance, true, false, 800, 600));
  app.run();

  return (0);
}



