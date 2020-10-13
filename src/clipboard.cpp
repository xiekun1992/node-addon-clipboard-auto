#include "clipboard.h"

std::function<void()> lambda_update_handler;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_CLIPBOARDUPDATE:
    // handle change 
    lambda_update_handler();
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		RemoveClipboardFormatListener(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

namespace clipboard_auto {
  Clipboard::Clipboard() {}
  Clipboard::~Clipboard() {}
  bool Clipboard::write_text(std::string text) {
    HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, text.size() + 1);
    if (clipbuffer != NULL) {
      char* buffer = (char*)GlobalLock(clipbuffer);
      if (buffer != NULL) {
        memcpy(buffer, text.c_str(), text.size() + 1);
        EmptyClipboard();
        SetClipboardData(CF_TEXT, clipbuffer);
        GlobalUnlock(clipbuffer);
        return true;
      }
    }
    return false;
  }
  std::string Clipboard::read_text() {
    std::string empty = "";
    if (!IsClipboardFormatAvailable(CF_UNICODETEXT) && !IsClipboardFormatAvailable(CF_TEXT)) return empty;

    HANDLE handle = (HANDLE)GetClipboardData(CF_TEXT);
    if (handle != NULL) {
      if (GlobalLock(handle) != NULL) {
        char* buffer = (char*)GetClipboardData(CF_TEXT);
        std::string text(buffer, buffer + strlen(buffer));
        GlobalUnlock(handle);
        return text;
      }
    }
    return empty;
  }
  bool Clipboard::write_files(std::vector<std::wstring> files) {
    std::wstring sFiles; // 对文件名以双字节存放
    for (std::wstring file : files) {
      sFiles += file;
      sFiles.push_back(0); // 追加'\0'
    }

    DROPFILES dobj = { 20, { 0, 0 }, 0, 1 };
    int nLen = sFiles.size()+1;
    int nGblLen = sizeof(dobj) + nLen * sizeof(wchar_t);
    
    HGLOBAL hGbl = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, nGblLen);
    if (hGbl != NULL) {
      char* buffer = (char*)GlobalLock(hGbl);
      if (buffer != NULL) {
        memcpy(buffer, &dobj, sizeof(dobj));
        memcpy(buffer + sizeof(dobj), sFiles.c_str(), nLen * sizeof(wchar_t));
        EmptyClipboard();
        SetClipboardData(CF_HDROP, hGbl);
        GlobalUnlock(hGbl);
        return true;
      }
    }
    return false;
  }
  std::vector<std::wstring> Clipboard::read_files() {
    std::vector<std::basic_string<wchar_t>> filenames;
    if (!IsClipboardFormatAvailable(CF_HDROP)) return filenames;

    HGLOBAL hClipboardText = (HGLOBAL)GetClipboardData(CF_HDROP);
    if (hClipboardText != NULL) {
      HDROP  h = (HDROP)GlobalLock(hClipboardText);
      if (h != NULL) {
        int fileCount = DragQueryFile(h, 0xFFFFFFFF, nullptr, 0);
        for (int i = 0; i < fileCount; i++) {
          std::basic_string<wchar_t> str;
          filenames.push_back(str);
          int filenameLength = DragQueryFile(h, i, nullptr, 0);
          filenames[i].reserve(filenameLength);
          DragQueryFile(h, i, &(filenames[i][0]), filenameLength + 1);
        }
        GlobalUnlock(hClipboardText);
        return filenames;
      }
    }
    return filenames;
  }
  void Clipboard::capture(std::function<void()> const& lambda) {
    lambda_update_handler = lambda;
    
    LPSTR CLASS_NAME = "Clipboard Window Class";

    HINSTANCE hInstance = GetModuleHandle(0);
    WNDCLASS wc = { };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);
    HWND hwnd = CreateWindowEx(
      0,                              // Optional window styles.
      CLASS_NAME,                     // Window class
      "Clipboard Listener Windows",    // Window text
      WS_OVERLAPPEDWINDOW,            // Window style
      // Size and position
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
      NULL,       // Parent window    
      NULL,       // Menu
      hInstance,  // Instance handle
      NULL        // Additional application data
    );
    if (hwnd != NULL)
    {
      AddClipboardFormatListener(hwnd);
      MSG msg;
      while (GetMessage(&msg, nullptr, 0, 0))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
  }
  void Clipboard::release() {

  }
}