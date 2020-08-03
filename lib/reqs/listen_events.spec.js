'use strict';

let native = require('../../index');
const expect = require('chai').expect;

describe('when start is called ', () => {
    it('should lauch event into callback', (done) => {
        let events = {};
        native.start(function(action, name){
            events[action] = name;
        });
        setTimeout(() => {
            native.stop();
        }, 1000);
        setTimeout(() => {
            expect(events).to.have.all.keys('start', 'stop', 'focused_process');
            done();
        }, 1500);
    });
});
