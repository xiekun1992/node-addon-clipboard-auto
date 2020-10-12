#pragma once

#ifndef _NODE_ADDON_CLIPBOARD_AUTO
#define _NODE_ADDON_CLIPBOARD_AUTO

#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <functional>
#include <shlobj.h>
#include <windows.h>

namespace clipboard_auto {
  class Clipboard {
    public:
      Clipboard(std::function<void()> const& lambda);
      ~Clipboard();
      bool write_text(std::string text);
      std::string read_text();
      bool write_files(std::vector<std::wstring> files);
      std::vector<std::wstring> read_files();
      void capture();
      void release();
  };
}

#endif