#include "Worker.h"
#include "WindowEventHandler.h"

using namespace FocusedProcessNativeAPI;

Worker::Worker(
        const Napi::Function &callback
) : Napi::AsyncWorker(callback) {

}

void Worker::Execute() {
  WindowEventHandler::getInstance()->launchListener();
  while (WindowEventHandler::getInstance()->poll());
  WindowEventHandler::getInstance()->stopListener();

}

void Worker::OnOK() {

}
