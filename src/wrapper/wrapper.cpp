#include <napi.h>
#include "clipboard_wrapper.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  Clipboard::initMethods(env, exports);
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll);