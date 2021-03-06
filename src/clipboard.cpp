#include "clipboard.h"

std::function<void()> lambda_update_handler;

#if _WIN32 == 1
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
#endif

namespace clipboard_auto {
  Clipboard::Clipboard() {}
  Clipboard::~Clipboard() {}
  bool Clipboard::write_text(std::string text) {
#if _WIN32 == 1
    OpenClipboard(0);

    HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, text.size() + 1);
    if (clipbuffer != NULL) {
      char* buffer = (char*)GlobalLock(clipbuffer);
      if (buffer != NULL) {
        memcpy(buffer, text.c_str(), text.size() + 1);
        EmptyClipboard();
        SetClipboardData(CF_TEXT, clipbuffer);
        GlobalUnlock(clipbuffer);

        CloseClipboard();
        return true;
      }
    }
    CloseClipboard();
#endif
    return false;
  }
  std::string Clipboard::read_text() {
    std::string empty = "";
#if _WIN32 == 1
    OpenClipboard(0);

    if (!IsClipboardFormatAvailable(CF_UNICODETEXT) && !IsClipboardFormatAvailable(CF_TEXT)) return empty;

    HANDLE handle = (HANDLE)GetClipboardData(CF_TEXT);
    if (handle != NULL) {
      if (GlobalLock(handle) != NULL) {
        char* buffer = (char*)GetClipboardData(CF_TEXT);
        std::string text(buffer, buffer + strlen(buffer));
        GlobalUnlock(handle);

        CloseClipboard();
        return text;
      }
    }
    CloseClipboard();
#endif
    return empty;
  }
  bool Clipboard::write_files(std::vector<std::u16string> files) {
#if _WIN32 == 1
    OpenClipboard(0);

    std::u16string sFiles; // 对文件名以双字节存放
    for (std::u16string file : files) {
      sFiles += file;
      sFiles.push_back(0); // 追加'\0'
    }

    DROPFILES dobj = { 20, { 0, 0 }, 0, 1 };
    int nLen = (sFiles.size() + 1) * sizeof(char16_t);
    int nGblLen = sizeof(dobj) + nLen;
    
    HGLOBAL hGbl = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, nGblLen);
    if (hGbl != NULL) {
      char* buffer = (char*)GlobalLock(hGbl);
      if (buffer != NULL) {
        memcpy(buffer, &dobj, sizeof(dobj));
        memcpy(buffer + sizeof(dobj), sFiles.c_str(), nLen);
        EmptyClipboard();
        SetClipboardData(CF_HDROP, hGbl);
        GlobalUnlock(hGbl);

        CloseClipboard();
        return true;
      }
    }
    CloseClipboard();
#endif
    return false;
  }
  std::vector<std::u16string> Clipboard::read_files() {
    std::vector<std::basic_string<char16_t>> filenames;
#if _WIN32 == 1
    OpenClipboard(0);

    if (!IsClipboardFormatAvailable(CF_HDROP)) return filenames;

    HGLOBAL hClipboardText = (HGLOBAL)GetClipboardData(CF_HDROP);
    if (hClipboardText != NULL) {
      HDROP  h = (HDROP)GlobalLock(hClipboardText);
      if (h != NULL) {
        int fileCount = DragQueryFile(h, 0xFFFFFFFF, nullptr, 0);
        for (int i = 0; i < fileCount; i++) {
          std::basic_string<char16_t> str;
          filenames.push_back(str);
          int filenameLength = DragQueryFile(h, i, nullptr, 0);
          filenames[i].reserve(filenameLength);
          DragQueryFile(h, i, (LPWSTR)&(filenames[i][0]), filenameLength + 1);
        }
        GlobalUnlock(hClipboardText);

        CloseClipboard();
        return filenames;
      }
    }
    CloseClipboard();
#endif
    return filenames;
  }
  void Clipboard::capture(std::function<void()> const& lambda) {
    lambda_update_handler = lambda;

#if _WIN32 == 1
    const wchar_t CLASS_NAME[] = L"Clipboard Window Class";

    HINSTANCE hInstance = GetModuleHandle(0);
    WNDCLASS wc = { };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);
    HWND hwnd = CreateWindowEx(
      0,                              // Optional window styles.
      CLASS_NAME,                     // Window class
      L"Clipboard Listener Windows",    // Window text
      WS_OVERLAPPEDWINDOW,            // Window style
      // Size and position
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
      NULL,       // Parent window    
      NULL,       // Menu
      hInstance,  // Instance handle
      NULL        // Additional application data
    );
    if (hwnd != NULL) {
      AddClipboardFormatListener(hwnd);
      MSG msg;
      while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
#elif __linux == 1
    notify = true;
    Display *display = XOpenDisplay(NULL);
    unsigned long color = BlackPixel(display, DefaultScreen(display));
    Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0,0, 1,1, 0, color, color);
    
    const char* bufname_clipboard = "CLIPBOARD";
    const char* bufname_primary = "PRIMARY";
    int event_base, error_base;
    XEvent event;
    Atom bufid_clip = XInternAtom(display, bufname_clipboard, False);
    Atom bufid_pri = XInternAtom(display, bufname_primary, False);

    XFixesQueryExtension(display, &event_base, &error_base);
    XFixesSelectSelectionInput(display, DefaultRootWindow(display), bufid_clip, XFixesSetSelectionOwnerNotifyMask);
    XFixesSelectSelectionInput(display, DefaultRootWindow(display), bufid_pri, XFixesSetSelectionOwnerNotifyMask);

    while (notify) {
      if (XPending(display)) {
        XNextEvent(display, &event);
        if (
          event.type == event_base + XFixesSelectionNotify && 
          (((XFixesSelectionNotifyEvent*)&event)->selection == bufid_clip || ((XFixesSelectionNotifyEvent*)&event)->selection == bufid_pri)
        ) {
          // printf("update\n");
          lambda_update_handler();
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1)); // prevent high cpu usage
    }

    XDestroyWindow(display, window);
    XCloseDisplay(display);
#endif
  }
  void Clipboard::release() {
#if __linux == 1
    notify = false;
#endif
  }
}