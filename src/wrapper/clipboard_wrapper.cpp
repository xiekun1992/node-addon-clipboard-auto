#include "clipboard_wrapper.h"

clipboard_auto::Clipboard clipboard;
std::thread thread;
Napi::ThreadSafeFunction tsfn;
int threadId = 0;

namespace Clipboard {
  Napi::Boolean writeText(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) {
      throw Napi::Error::New(env, "param text required");
    }
    if (!info[0].IsString()) {
      throw Napi::TypeError::New(env, "expect text to be type of string");
    }
    bool result = clipboard.write_text(info[0].As<Napi::String>().Utf8Value());
    return Napi::Boolean::New(env, result);
  }
  Napi::String readText(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), clipboard.read_text());
  }
  Napi::Boolean writeFiles(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) {
      throw Napi::Error::New(env, "param files required");
    }
    if (!info[0].IsArray()) {
      throw Napi::TypeError::New(env, "expect files to be type of string array");
    }
    std::vector<std::u16string> filenames;
    Napi::Array files = info[0].As<Napi::Array>();
    for (unsigned int i = 0; i < files.Length(); i++) {
      Napi::Value path = files[i];
      if (path.IsString()) {
        std::u16string str16 = path.As<Napi::String>().Utf16Value();
        // std::wstring strw(str16.begin(), str16.end());
        filenames.push_back(str16);
      }
    }
    bool result = clipboard.write_files(filenames);
    return Napi::Boolean::New(env, result);
  }
  Napi::Array readFiles(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    std::vector<std::u16string> files = clipboard.read_files();
    Napi::Array fileArray = Napi::Array::New(env, files.size());
    int index = 0;
    for (auto& file : files) {
      fileArray[index++] = Napi::String::New(env, file.c_str());
    }
    return fileArray;
  }
  Napi::Value capture(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) {
      throw Napi::Error::New(env, "param callback required");
    } 
    if (!info[0].IsFunction()) {
      throw Napi::TypeError::New(env, "expect callback to be a function");
    }

    tsfn = Napi::ThreadSafeFunction::New(env, info[0].As<Napi::Function>(), "tsfn", 0, 1, [](Napi::Env) {
      thread.join();
    });
    thread = std::thread([]() {
#if _WIN32 == 1
      threadId = GetCurrentThreadId();
#endif
      auto callback = [](Napi::Env env, Napi::Function jsCallback) {
        jsCallback.Call({});
      };
      auto updateHandler = [=]() {
        tsfn.BlockingCall(callback);
      };
      clipboard.capture(updateHandler);
    });
    return info.Env().Undefined();
  }
  Napi::Value release(const Napi::CallbackInfo& info) {
#if _WIN32 == 1
    PostThreadMessage(threadId, WM_QUIT, 0, 0);
#elif __linux == 1
    clipboard.release();
#endif
    tsfn.Release();
    return info.Env().Undefined();
  }
  Napi::Object initMethods(Napi::Env env, Napi::Object exports) {
    exports.Set("writeText", Napi::Function::New(env, Clipboard::writeText));
    exports.Set("readText", Napi::Function::New(env, Clipboard::readText));
    exports.Set("writeFiles", Napi::Function::New(env, Clipboard::writeFiles));
    exports.Set("readFiles", Napi::Function::New(env, Clipboard::readFiles));
    exports.Set("capture", Napi::Function::New(env, Clipboard::capture));
    exports.Set("release", Napi::Function::New(env, Clipboard::release));
    return exports;
  }
}