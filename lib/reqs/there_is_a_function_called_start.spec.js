'use strict';

let native = require('../../index');
const expect = require('chai').expect;

describe('when the function', () => {
    const safeCallbackForDone = (done)=>{
      return (event)=>{
          if(event == "stop")done();
      }
    }
    describe('"start"', () => {
        it('should export a function', () => {
            expect(native.start).to.be.a('function')
        });
    });
    describe('"start"', () => {
        it('is called without argument should throw an exception', () => {
            expect(()=> native.start()).to.throw(Error, "Invalid argument count");
        });
    });
    describe('"start"', (done) => {
        it('is called with an invalid argument should throw an exception', () => {
            expect(()=> native.start()).to.throw(Error, "Invalid argument count");
        });
    });
    describe('"start"', () => {
        it('is first called should not throw any exception', (done) => {
            expect(()=> {
              native.start(safeCallbackForDone(done));
                native.stop();
            }).to.not.throw();
        });
    });
    describe('"start"', () => {
        it('is called again should throw an exception', (done) => {
            try{
              native.start(safeCallbackForDone(done));
              native.start(safeCallbackForDone(done));
            }
            catch (e){
                expect(()=> {
                    throw (e);
                }).to.throw(Error, "Already running");
            }
            native.stop();


        });
    });


});
