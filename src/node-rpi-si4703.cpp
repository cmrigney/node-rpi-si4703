#include <node.h>
#include "FMTuner.h"

namespace rpisi4703 {

using node::AtExit;
using v8::Function;
using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Number;
using v8::Value;
using v8::Array;

void init(Local<Object> exports) {
  FMTuner::Init(exports->GetIsolate(), exports);
}

NODE_MODULE(rpisi4703, init);

}