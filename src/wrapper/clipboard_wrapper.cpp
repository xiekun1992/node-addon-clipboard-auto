#include "clipboard_wrapper.h"

clipboard_auto::Clipboard clipboard;
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
    return Napi::String::New(env, clipboard.read_text());
  }
  Napi::Boolean writeFiles(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) {
      throw Napi::Error::New(env, "param files required");
    }
    if (!info[0].IsArray()) {
      throw Napi::TypeError::New(env, "expect files to be type of string array");
    }
    std::vector<std::wstring> filenames;
    Napi::Array files = info[0].As<Napi::Array>();
    for (int i = 0; i < files.Length(); i++) {
      Napi::Value path = files[i];
      if (path.IsString()) {
        std::u16string str16 = path.As<Napi::String>().Utf16Value();
        std::wstring strw(str16.begin(), str16.end());
        filenames.push_back(strw);
      }
    }
    bool result = clipboard.write_files(filenames);
    return Napi::Boolean::New(env, result);
  }
  Napi::Array readFiles(const Napi::CallbackInfo& info) {
    std::vector<std::wstring> files = clipboard.read_files();
    Napi::Array fileArray = Napi::Array::New(env, files.size());
    int index = 0;
    for (auto& file : files) {
      std::u16string str(file.begin(), file.end());
      fileArray[i++] = Napi::String::New(env, str);
    }
    return fileArray;
  }
  Napi::Value capture(const Napi::CallbackInfo& info) {
    clipboard.capture();
  }
  Napi::Object initMethods(Napi::Env env, Napi::Object exports) {
    exports.Set("writeText", Napi::Function::New(env, Clipboard::writeText));
    exports.Set("readText", Napi::Function::New(env, Clipboard::readText));
    exports.Set("writeFiles", Napi::Function::New(env, Clipboard::writeFiles));
    exports.Set("readFiles", Napi::Function::New(env, Clipboard::readFiles));
    exports.Set("capture", Napi::Function::New(env, Clipboard::capture));
    return exports;
  }
}