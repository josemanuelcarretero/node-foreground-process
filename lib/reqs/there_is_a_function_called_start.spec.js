'use strict';

let native = require('../../index');
const expect = require('chai').expect;

describe('when the function', () => {
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
    describe('"start"', () => {
        it('is called with an invalid argument should throw an exception', () => {
            expect(()=> native.start()).to.throw(Error, "Invalid argument count");
        });
    });
    describe('"start"', () => {
        it('is first called should not throw any exception', (done) => {
            expect(()=> {
                native.start(()=>{native.stop(); done();});
            }).to.not.throw();
        });
    });
    describe('"start"', () => {
        it('is called again should throw an exception', () => {
            expect(()=> {
                try{
                    native.start((a,m)=>{});
                    native.start((a,m)=>{});
                }
                catch (e){
                    throw (e);
                }
                finally{
                  native.stop();
                }

            }).to.throw(Error, "Already running");
        });
    });


});
