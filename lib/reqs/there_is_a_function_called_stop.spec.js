'use strict';

let native = require('../../index');
const expect = require('chai').expect;

describe('when the function', () => {
    describe('"stop"', () => {
        it('should export a function', () => {
            expect(native.stop).to.be.a('function')
        });
    });

    describe('"stop"', () => {
        it('is first called should throw a exception', () => {
            expect(() => native.stop()).to.throw(Error, "Not running");
        });
    });

    describe('"stop"', () => {
        it('is called after start function should throw a exception', (done) => {
                expect(()=> {
                    native.start(()=>{native.stop(); done();});
                }).to.not.throw();
        });
    });
});
