'use strict'

let native = require('bindings')('native')
const expect = require('chai').expect

describe('hello World function', () => {
  describe('"helloworld"', () => {
    it('should export a function', () => {
      expect(native.helloworld).to.be.a('function')
    })
  })
})
