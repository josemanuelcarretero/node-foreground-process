#include <iostream>
#include "WindowEventHandler.h"

FocusedProcessNativeAPI::WindowEventHandler *FocusedProcessNativeAPI::WindowEventHandler::instance = NULL;

// Callback function that handles events.
//
void CALLBACK HandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread,
               DWORD dwmsEventTime) {
    static char nameProgram[256] = {0};
    if (event == EVENT_SYSTEM_FOREGROUND && hwnd != NULL) {
        int length = GetWindowTextLength(hwnd); //Get the name process length
        GetWindowText(hwnd, nameProgram, length); //Get the name process
        nameProgram[length] = 0;
        std::string name(nameProgram);
        FocusedProcessNativeAPI::WindowEventHandler::getInstance()->notifyFocus(name);
    }
}

void FocusedProcessNativeAPI::WindowEventHandler::launchListener() {
  CoInitialize(NULL);
  g_hook = SetWinEventHook(
          EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,  // Range of events.
          NULL,                                          // Handle to DLL.
          HandleWinEvent,                                // The callback.
          0, 0,              // Process and thread IDs of interest (0 = all)
          WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS); // Flags.

  notifyStart();
  forceNotifyCurrentFocus();

}

void FocusedProcessNativeAPI::WindowEventHandler::stopListener() {
  notifyStop();
  UnhookWinEvent(g_hook);
  CoUninitialize();
  tsfn.Release(); // Free safe function
}


FocusedProcessNativeAPI::WindowEventHandler::WindowEventHandler() {
    running = false;
    info = NULL;
    worker = NULL;
}



// Auxiliary functions that allow sending events to the nodejs' main thread
//
void FocusedProcessNativeAPI::WindowEventHandler::notifyFocus(std::string str) {
    if (!info) return;
    std::string * str2 = new std::string(str);
    auto callback = [](Napi::Env env, Napi::Function jsCallback, std::string * str2) {
      jsCallback.Call({Napi::String::New(env, "focused_process"), Napi::String::New(env, *str2)});
      delete str2;
    };
    tsfn.BlockingCall(str2, callback);
}

void FocusedProcessNativeAPI::WindowEventHandler::notifyInterrupt() {
  if (!info) return;
  std::string * str2 = new std::string("");
  auto callback = [](Napi::Env env, Napi::Function jsCallback, std::string * str2) {
    jsCallback.Call({Napi::String::New(env, "interrupt"),});
    delete str2;
  };
  tsfn.BlockingCall(str2, callback);
}

void FocusedProcessNativeAPI::WindowEventHandler::notifyError(std::string str) {
    if (!info) return;
    std::string * str2 = new std::string(str);
    auto callback = [](Napi::Env env, Napi::Function jsCallback, std::string * str2) {
      jsCallback.Call({Napi::String::New(env, "error"), Napi::String::New(env, *str2)});
      delete str2;
    };
    tsfn.BlockingCall(str2, callback);

}

void FocusedProcessNativeAPI::WindowEventHandler::notifyStart() {
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


void FocusedProcessNativeAPI::WindowEventHandler::notifyStop() {
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

bool FocusedProcessNativeAPI::WindowEventHandler::start(const Napi::CallbackInfo &info) {
    if (!running) {
        running = true;
        this->info = const_cast<Napi::CallbackInfo*>(&info);
        tsfn = Napi::ThreadSafeFunction::New(
                      info.Env(),
                      info[0].As<Napi::Function>(),  // JavaScript function called asynchronously
                      "Resource Name",         // Name
                      0,                       // Unlimited queue
                      1,                       // Only one thread will use this initially
                      []( Napi::Env ) {        // Finalizer used to clean threads up

                      } );
        worker = new Worker(info[0].As<Napi::Function>());
        worker->Queue();
        return true;
    }
    return false;
}

bool FocusedProcessNativeAPI::WindowEventHandler::stop() {
    if (running) {
        running = false;
        return true;
    }
    return false;
}

bool FocusedProcessNativeAPI::WindowEventHandler::poll() {
    BOOL bRet;
    MSG msg;

    if (!running) {
        return false;
    }
    UINT_PTR timerId = SetTimer(NULL, NULL, 50, NULL); // Generate a event and poll is stopped
    bRet = GetMessage(&msg, 0, 0, 0);
    KillTimer(NULL, timerId);
    if (bRet != 0) {
        if (bRet == -1) {
            // handle the error and possibly exit
            notifyError("Fatal error");
            return false;
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            return true;
        }
    }
    notifyInterrupt();
    return false;
}

bool FocusedProcessNativeAPI::WindowEventHandler::isRunning() {
    return running;
}

void FocusedProcessNativeAPI::WindowEventHandler::forceNotifyCurrentFocus(){
    HandleWinEvent(g_hook, EVENT_SYSTEM_FOREGROUND, GetForegroundWindow(), 0 , 0, 0, 0);
}


FocusedProcessNativeAPI::WindowEventHandler *FocusedProcessNativeAPI::WindowEventHandler::getInstance() {
    if (!instance) {
        instance = new WindowEventHandler;
    }
    return instance;
}
