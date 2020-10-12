#pragma once

#ifndef _CLIPBOARD_WRAPPER
#define _CLIPBOARD_WRAPPER

#include <napi.h>
#include "../clipboard.h"

namespace Clipboard {
  Napi::Boolean writeText(const Napi::CallbackInfo& info);
  Napi::String readText(const Napi::CallbackInfo& info);
  Napi::Boolean writeFiles(const Napi::CallbackInfo& info);
  Napi::Array readFiles(const Napi::CallbackInfo& info);
  Napi::Value capture(const Napi::CallbackInfo& info);
  Napi::Object initMethods(Napi::Env env, Napi::Object exports);
}

#endif