#include <mutex>
#include <iostream>
#include <windows.h>
#include "ThreadWorker.h"

void CALLBACK HandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread,
               DWORD dwmsEventTime) {
    static char nameProgram[256] = {0};
    if (event == EVENT_SYSTEM_FOREGROUND && hwnd != NULL) {
        int length = GetWindowTextLength(hwnd); //Get the name process length
        GetWindowText(hwnd, nameProgram, length); //Get the name process
        nameProgram[length] = 0;
        std::string name(nameProgram);
        ThreadWorker::getInstance()->notifyFocus(name);
    }
}

ThreadWorker::ThreadWorker(){
  running = false;
  thread = NULL;
}

void ThreadWorker::launchListener() {
  CoInitialize(NULL);
  g_hook = SetWinEventHook(
          EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,  // Range of events.
          NULL,                                          // Handle to DLL.
          HandleWinEvent,                                // The callback.
          0, 0,              // Process and thread IDs of interest (0 = all)
          WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS); // Flags.

}

void ThreadWorker::stopListener() {
  UnhookWinEvent(g_hook);
  CoUninitialize();
}

void ThreadWorker::run(ThreadWorker * runnable){
  BOOL bRet;
  MSG msg;

  runnable->launchListener();
  runnable->notifyStart();
  runnable->notifyCurrentFocus();
  while (runnable->running)
  {
    if((bRet = PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) > 0) //Or use an if statement
    {
      TranslateMessage (&msg);
      DispatchMessage (&msg);
    }
    else if(bRet == -1){
      runnable->notifyError("Fatal error");
      runnable->running = false;
      break;
    }
  }
  runnable->notifyStop();
  runnable->stopListener();
  runnable->info = NULL;
  runnable->tsfn.Release();
}

void ThreadWorker::start(const Napi::CallbackInfo &info){
    muterRunning.lock();
    if(running){
      muterRunning.unlock();
      return throwAlreadyStart(info.Env());
    }
    if(thread){
      thread->join();
      delete thread;
      thread = NULL;
    }
    this->info = const_cast<Napi::CallbackInfo*>(&info);
    tsfn = Napi::ThreadSafeFunction::New(
                  info.Env(),
                  info[0].As<Napi::Function>(),  // JavaScript function called asynchronously
                  "ForegroundListener",         // Name
                  0,                       // Unlimited queue
                  1,                       // Only one thread will use this initially
                  []( Napi::Env ) {        // Finalizer used to clean threads up

                  } );
    running = true;
    thread = new std::thread(ThreadWorker::run, this);
    muterRunning.unlock();

}

void ThreadWorker::stop(const Napi::CallbackInfo &info){
  muterRunning.lock();
  if(!running){
    muterRunning.unlock();
    return throwNotRunning(info.Env());
  }
  running = false;
  muterRunning.unlock();
}

void ThreadWorker::notifyFocus(std::string str) {
    if (!info) return;
    std::string * str2 = new std::string(str);
    auto callback = [](Napi::Env env, Napi::Function jsCallback, std::string * str2) {
      jsCallback.Call({Napi::String::New(env, "focused_process"), Napi::String::New(env, *str2)});
      delete str2;
    };
    tsfn.BlockingCall(str2, callback);
}

void ThreadWorker::notifyError(std::string str) {
    if (!info) return;
    std::string * str2 = new std::string(str);
    auto callback = [](Napi::Env env, Napi::Function jsCallback, std::string * str2) {
      jsCallback.Call({Napi::String::New(env, "error"), Napi::String::New(env, *str2)});
      delete str2;
    };
    tsfn.BlockingCall(str2, callback);
}

void ThreadWorker::notifyStart() {
    if (!info) return;
    std::string * str2 = new std::string("");
    auto callback = [](Napi::Env env, Napi::Function jsCallback, std::string * str2) {
      Napi::HandleScope scope(env);
      Napi::Object obj = Napi::Object::New(env);
      jsCallback.Call({Napi::String::New(env, "start"),});
      delete str2;
    };
    tsfn.BlockingCall(str2, callback);
}


void ThreadWorker::notifyStop() {
    if (!info) return;
    std::string * str2 = new std::string("");
    auto callback = [](Napi::Env env, Napi::Function jsCallback, std::string * str2) {
      Napi::HandleScope scope(env);
      Napi::Object obj = Napi::Object::New(env);
      jsCallback.Call({Napi::String::New(env, "stop"),});
      delete str2;
    };
    tsfn.BlockingCall(str2, callback);
}

void ThreadWorker::throwAlreadyStart(const Napi::Env &env){
  Napi::Error::New(env, "Already running").ThrowAsJavaScriptException();
}

void ThreadWorker::throwNotRunning(const Napi::Env &env){
  Napi::Error::New(env, "Not running").ThrowAsJavaScriptException();
}

void ThreadWorker::notifyCurrentFocus(){
  HandleWinEvent(g_hook, EVENT_SYSTEM_FOREGROUND, GetForegroundWindow(), 0 , 0, 0, 0);
}

ThreadWorker * ThreadWorker::getInstance(){
  static ThreadWorker * instance = NULL;
  if (!instance) {
      instance = new ThreadWorker;
  }
  return instance;
}
