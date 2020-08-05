#include <napi.h>
#include <windows.h>
#include <iostream>
//#include "WindowEventHandler.h"
#include "ThreadWorker.h"

void StartAsyncCallback(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Invalid argument count").ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsFunction()) {
        Napi::TypeError::New(env, "Invalid argument types").ThrowAsJavaScriptException();
        return;
    }

    ThreadWorker::getInstance()->start(info);

    return;
}

void StopAsyncCallback(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    ThreadWorker::getInstance()->stop(info);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "start"), Napi::Function::New(env, StartAsyncCallback));
    exports.Set(Napi::String::New(env, "stop"), Napi::Function::New(env, StopAsyncCallback));
    return exports;
}


NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
