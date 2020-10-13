#pragma once

#ifndef _NODE_ADDON_CLIPBOARD_AUTO
#define _NODE_ADDON_CLIPBOARD_AUTO

#define UNICODE

#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <shlobj.h>
#include <windows.h>

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