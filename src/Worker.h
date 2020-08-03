#ifndef __WORKER_EMITTER_H__
#define __WORKER_EMITTER_H__

#include <napi.h>
#include <windows.h>
#include <iostream>


class Worker : public Napi::AsyncWorker {
public:
    Worker(const Napi::Function &callback);

protected:

    void Execute();

    void OnOK();
};

#endif
