{
  "targets": [
    {
      "target_name": "clipboard_auto",
      "cflags!": ["-fno-exceptions"],
      "cflags_cc!": ["-fno-exceptions"],
      "sources": [
        "src/clipboard.cpp",
        "src/wrapper/wrapper.cpp",
        "src/wrapper/clipboard_wrapper.cpp"
      ],
      "include_dirs": ["<!@(node -p \"require('node-addon-api').include\")"],
      "dependencies": ["<!(node -p \"require('node-addon-api').gyp\")"],
      "defines": ["NAPI_DISABLE_CPP_EXCEPTIONS"],
      "conditions": []
    }
  ]
}