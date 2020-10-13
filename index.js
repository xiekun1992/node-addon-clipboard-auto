const clipboardAuto = require('./build/Release/clipboard_auto.node')

module.exports = {
  writeText(text) {
    return clipboardAuto.writeText(text)
  },
  readText() {
    return clipboardAuto.readText()
  },
  writeFiles(files) {
    return clipboardAuto.writeFiles(files)
  },
  readFiles() {
    return clipboardAuto.readFiles()
  },
  capture(callback) {
    return clipboardAuto.capture(callback)
  },
  release() {
    return clipboardAuto.release()
  }
}