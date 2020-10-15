const assert = require('assert');
const os = require('os')
const clipboardAuto = require('../index')

if (os.platform() === 'linux' && !process.env.DISPLAY) {
  console.log('npm test will not run on linux server') // 没有GUI环境导致SegmentFault错误
  process.exit(0)
}

describe('clipboardAuto', () => {
  describe('text', () => {
    it('given a text when copy to clipboard should return true', () => {
      assert.equal(clipboardAuto.writeText('this is a test for clipboard'), true)
    })
    it('given a text when copy to clipboard should be available to read', () => {
      const text = 'this is a test for clipboard'
      if (clipboardAuto.writeText(text)) {
        assert.equal(clipboardAuto.readText(), text)
      } else {
        assert.fail()
      }
    })
    it('given a text include chinese when copy to clipboard should be available to read', () => {
      const text = 'this is a test for nodejs 的剪切板扩展'
      if (clipboardAuto.writeText(text)) {
        assert.equal(clipboardAuto.readText(), text)
      } else {
        assert.fail()
      }
    })
  })
  describe('files', () => {
    it('given an array of file path when copy to clipboard should return true', () => {
      const files = [
        "C:\\Users\\test\\Desktop\\test.crt" ,
        "C:\\Users\\test\\Desktop\\player",
        "C:\\Users\\test\\Desktop\\test.png"
      ]
      assert.equal(clipboardAuto.writeFiles(files), true)
    })
    it('given an array of file path when copy to clipboard should be available to read', () => {
      const files = [
        "C:\\Users\\test\\Desktop\\test.crt" ,
        "C:\\Users\\test\\Desktop\\player",
        "C:\\Users\\test\\Desktop\\test.png"
      ]
      if (clipboardAuto.writeFiles(files)) {
        assert.deepEqual(clipboardAuto.readFiles(), files)
      } else {
        assert.fail()
      }
    })
    it('given an array of file path include chinese when copy to clipboard should be available to read', () => {
      const files = [
        "C:\\Users\\test\\Desktop\\test测试.crt" ,
        "C:\\Users\\test\\Desktop\\player玩家",
        "C:\\Users\\test\\Desktop\\测试图片.png"
      ]
      if (clipboardAuto.writeFiles(files)) {
        assert.deepEqual(clipboardAuto.readFiles(), files)
      } else {
        assert.fail()
      }
    })
  })
  describe('capture', () => {
    it('when start to capture clipboard changes should be able to relase', (done) => {
      clipboardAuto.capture(function() {})
      setTimeout(() => {
        clipboardAuto.release()
        done()
      }, 50)
    })
    it('when start to capture clipboard changes should capture text writed', (done) => {
      const text = 'this is a test for nodejs 的剪切板扩展'
      clipboardAuto.capture(function() {
        assert.equal(clipboardAuto.readText(), text)
        clipboardAuto.release()
        done()
      })
      setTimeout(() => {
        clipboardAuto.writeText(text)
      }, 50)
    })
    it('when start to capture clipboard changes should capture files writed', (done) => {
      const files = [
        "C:\\Users\\test\\Desktop\\test测试.crt" ,
        "C:\\Users\\test\\Desktop\\player玩家",
        "C:\\Users\\test\\Desktop\\测试图片.png"
      ]
      clipboardAuto.capture(function() {
        assert.deepEqual(clipboardAuto.readFiles(), files)
        clipboardAuto.release()
        done()
      })
      setTimeout(() => {
        clipboardAuto.writeFiles(files)
      }, 50)
    })
  })
})
