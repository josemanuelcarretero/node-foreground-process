#ifndef __WINDOWS_EVENT_HANDLER_H__
#define __WINDOWS_EVENT_HANDLER_H__

#include <Windows.h>
#include <napi.h>
#include "Worker.h"

namespace FocusedProcessNativeAPI {

    class WindowEventHandler {
        // Singleton class
    private:
        static WindowEventHandler *instance;
        WindowEventHandler();
    public:
        static WindowEventHandler *getInstance();
    private:
        // Safe nodejs' callback to be called from a separate thread
        Napi::ThreadSafeFunction tsfn;
        // Context from which it was executed
        Napi::CallbackInfo * info;
        Worker * worker;
        HWINEVENTHOOK g_hook;
        bool running;
    public:
        // This function enables the listening of events. It must be executed in a worker
        void launchListener();
        // This function disable the listening of events. It must be executed in a worker
        void stopListener();

        // Auxiliary functions that allow sending events to the nodejs' main thread
        //
        void notifyFocus(std::string str);
        void notifyInterrupt();
        void notifyError(std::string std);
        void notifyStart();
        void notifyStop();

        bool start(const Napi::CallbackInfo &info);
        bool stop();

        bool poll();
        bool isRunning();

        // This function fakes an foreground process change to immediately get the foreground process event
        //
        void forceNotifyCurrentFocus();

    };
};
#endif // __WINDOWS_EVENT_HANDLER_H__
