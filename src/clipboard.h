#pragma once

#ifndef _NODE_ADDON_CLIPBOARD_AUTO
#define _NODE_ADDON_CLIPBOARD_AUTO

#include <Windows.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <shlobj.h>

namespace clipboard_auto {
  class Clipboard {
    public:
      Clipboard();
      ~Clipboard();
      bool write_text(std::string text);
      std::string read_text();
      bool write_files(std::string files[]);
      std::string read_files();
  };
}

#endif