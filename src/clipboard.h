#pragma once

#ifndef _NODE_ADDON_CLIPBOARD_AUTO
#define _NODE_ADDON_CLIPBOARD_AUTO

#if _WIN32 == 1

#define UNICODE

#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <shlobj.h>
#include <windows.h>

#elif __linux == 1

#include <X11/extensions/Xfixes.h>

#endif
namespace clipboard_auto {
  class Clipboard {
    public:
      Clipboard();
      ~Clipboard();
      bool write_text(std::string text);
      std::string read_text();
      bool write_files(std::vector<std::u16string> files);
      std::vector<std::u16string> read_files();
      void capture(std::function<void()> const& lambda);
      void release();
  };
}

#endif