#ifndef __THREADWORKER_H__
#define __THREADWORKER_H__

#include <napi.h>
#include <mutex>

class ThreadWorker{
private:
  Napi::ThreadSafeFunction tsfn;
  Napi::CallbackInfo *info;
  HWINEVENTHOOK g_hook;
  std::thread * thread;
  bool running;
  std::mutex muterRunning;
  void launchListener();
  void stopListener();
protected:
  void notifyError(std::string str);
  void notifyStop();
  void notifyStart();
  void notifyCurrentFocus();
  static void throwAlreadyStart(const Napi::Env &env);
  static void throwNotRunning(const Napi::Env &env);
public:
  void notifyFocus(std::string str);
  ThreadWorker();
  void start(const Napi::CallbackInfo &info);
  void stop(const Napi::CallbackInfo &info);
  static void run(ThreadWorker * runnable);
  static ThreadWorker * getInstance();
};

#endif
