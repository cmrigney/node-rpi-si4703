#include "FMTuner.h"

namespace rpisi4703 {

using v8::Function;
using v8::FunctionTemplate;
using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Persistent;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Number;
using v8::Value;
using v8::Array;
using v8::HandleScope;
using v8::Undefined;

typedef struct _CallbackData
{
  Persistent<Function> callback;
  Isolate *isolate;
  int resultStatus;
  FMTuner *tuner;
  char data[32];
} CallbackData;

FMTuner::FMTuner(int resetPin, int sdioPin) {
  radio = new Si4703_Breakout(resetPin, sdioPin);
}

FMTuner::~FMTuner() {
  delete radio;
}

Si4703_Breakout *FMTuner::GetRadio() {
  return radio;
}

#define DEFAULTCALLBACK(ERRMSG) \
      [](CallbackData *cd, Local<Value> *argv) { \
      if(cd->resultStatus) { \ 
        argv[0] = Null(cd->isolate); \ 
        argv[1] = Null(Number::New(cd->isolate, cd->resultStatus)); \ 
      } \ 
      else { \ 
        argv[0] = Local<Value>::New(cd->isolate, String::NewFromUtf8(cd->isolate, ERRMSG)); \ 
        argv[1] = Null(cd->isolate); \ 
      } \
      } \


#define BEGINASYNC() \
  uv_work_t *work = new uv_work_t(); \
  CallbackData *cd = new CallbackData(); \
  cd->isolate = isolate; \
  cd->callback.Reset(isolate, cb); \
  cd->tuner = this; \


#define ASYNCBODY(NativeFunc,SetResultArgs)  \
  work->data = cd; \
 \
  uv_queue_work(uv_default_loop(), work, \ 
  [](uv_work_t *work) { \
    CallbackData *cd = (CallbackData *)work->data; \
    NativeFunc(cd);\
  }, \
  [](uv_work_t *work, int status) { \ \
      CallbackData *cd = (CallbackData *)work->data; \
      HandleScope handleScope(cd->isolate); \
 \
      const int argc = 2; \
 \
      Local<Value> argv[argc]; \ 
       \
      SetResultArgs(cd, argv); \
 \
      Local<Function>::New(cd->isolate, cd->callback)->Call(cd->isolate->GetCurrentContext()->Global(), argc, argv); \
      cd->callback.Reset(); \
 \
      delete work; \
      delete cd; \
  }); \


#define DEFAULTWRAP(METHODNAME) \
void FMTuner::METHODNAME##Wrap(const FunctionCallbackInfo<Value> &args) { \
  Isolate *isolate = args.GetIsolate(); \
   \
  if(args.Length() != 1) { \
    isolate->ThrowException(Exception::TypeError( \
      String::NewFromUtf8(isolate, "Wrong number of arguments, need a callback function"))); \
    return; \
  } \
   \
  if(!args[0]->IsFunction()) { \
    isolate->ThrowException(Exception::TypeError( \
      String::NewFromUtf8(isolate, "Argument should be function"))); \
    return; \
  } \
   \
  FMTuner *tuner = ObjectWrap::Unwrap<FMTuner>(args.Holder()); \
  tuner->METHODNAME(isolate, Local<Function>::Cast(args[0])); \
} \


void FMTuner::PowerOn(Isolate *isolate, v8::Local<v8::Function> cb) {
  BEGINASYNC();
  ASYNCBODY(
    [](CallbackData *cd) {
      cd->resultStatus = cd->tuner->powerOn();
    },
    DEFAULTCALLBACK("Power on failed")
  );
}

DEFAULTWRAP(PowerOn);

void FMTuner::PowerOff(Isolate *isolate, v8::Local<v8::Function> cb) {
  BEGINASYNC();
  ASYNCBODY(
    [](CallbackData *cd) {
      cd->resultStatus = cd->tuner->powerOff();
    },
    DEFAULTCALLBACK("Power off failed")
  );
}

DEFAULTWRAP(PowerOff);

void FMTuner::SeekUp(Isolate *isolate, v8::Local<v8::Function> cb) {
  BEGINASYNC();
  ASYNCBODY(
    [](CallbackData *cd) {
      cd->resultStatus = cd->tuner->seekUp();
    },
    DEFAULTCALLBACK("Seek up failed")
  );
}

DEFAULTWRAP(SeekUp);

void FMTuner::SeekDown(Isolate *isolate, v8::Local<v8::Function> cb) {
  BEGINASYNC();
  ASYNCBODY(
    [](CallbackData *cd) {
      cd->resultStatus = cd->tuner->seekDown();
    },
    DEFAULTCALLBACK("Seek down failed")
  );
}

DEFAULTWRAP(SeekDown);

void FMTuner::GetChannel(Isolate *isolate, v8::Local<v8::Function> cb) {
  BEGINASYNC();
  ASYNCBODY(
    [](CallbackData *cd) {
      cd->resultStatus = cd->tuner->getChannel();
    },
    DEFAULTCALLBACK("Get channel failed")
  );
}

DEFAULTWRAP(GetChannel);

void FMTuner::SetChannel(Isolate *isolate, Local<Number> channel, v8::Local<v8::Function> cb) {
  BEGINASYNC();

  int chan = channel->Value();
  *(int*)(&cd->data[0]) = chan;

  ASYNCBODY(
    [](CallbackData *cd) {
      cd->tuner->setChannel(*(int*)(&cd->data[0]));
      cd->resultStatus = 1;
    },
    DEFAULTCALLBACK("") //doesn't fail
  );
}

void FMTuner::SetChannelWrap(const FunctionCallbackInfo<Value> &args) { 
  Isolate *isolate = args.GetIsolate(); 
   
  if(args.Length() != 2) { 
    isolate->ThrowException(Exception::TypeError( 
      String::NewFromUtf8(isolate, "Wrong number of arguments, need a channel and a callback function"))); 
    return; 
  } 
   
  if(!args[0]->IsNumber()) { 
    isolate->ThrowException(Exception::TypeError( 
      String::NewFromUtf8(isolate, "Argument should be number"))); 
    return; 
  } 

  if(!args[1]->IsFunction()) { 
    isolate->ThrowException(Exception::TypeError( 
      String::NewFromUtf8(isolate, "Argument should be function"))); 
    return; 
  } 
   
  FMTuner *tuner = ObjectWrap::Unwrap<FMTuner>(args.Holder()); 
  tuner->SetChannel(isolate, Local<Number>::Cast(args[0]), Local<Function>::Cast(args[1])); 
}

void FMTuner::SetVolume(Isolate *isolate, Local<Number> volume, v8::Local<v8::Function> cb) {
  BEGINASYNC();

  int vol = volume->Value();
  if(vol > 15)
    vol = 15;
  else if(vol < 0)
    vol = 0;
    
  *(int*)(&cd->data[0]) = vol;

  ASYNCBODY(
    [](CallbackData *cd) {
      cd->tuner->setVolume(*(int*)(&cd->data[0]));
      cd->resultStatus = 1;
    },
    DEFAULTCALLBACK("") //doesn't fail
  );
}

void FMTuner::SetVolumeWrap(const FunctionCallbackInfo<Value> &args) { 
  Isolate *isolate = args.GetIsolate(); 
   
  if(args.Length() != 2) { 
    isolate->ThrowException(Exception::TypeError( 
      String::NewFromUtf8(isolate, "Wrong number of arguments, need a channel and a callback function"))); 
    return; 
  } 
   
  if(!args[0]->IsNumber()) { 
    isolate->ThrowException(Exception::TypeError( 
      String::NewFromUtf8(isolate, "Argument should be number"))); 
    return; 
  } 

  if(!args[1]->IsFunction()) { 
    isolate->ThrowException(Exception::TypeError( 
      String::NewFromUtf8(isolate, "Argument should be function"))); 
    return; 
  } 
   
  FMTuner *tuner = ObjectWrap::Unwrap<FMTuner>(args.Holder()); 
  tuner->SetVolume(isolate, Local<Number>::Cast(args[0]), Local<Function>::Cast(args[1])); 
}


void FMTuner::ReadRDS(Isolate *isolate, Local<Number> timeout, v8::Local<v8::Function> cb) {
  BEGINASYNC();

  int tmout = timeout->Value();

  *(int*)(&cd->data[0]) = tmout;

  ASYNCBODY(
    [](CallbackData *cd) {
      int timeout = *(int*)(&cd->data[0]);
      char msg[16];
      msg[0] = '\0';
      cd->tuner->readRDS(msg, timeout);
      strcpy(cd->data, msg);
      cd->resultStatus = 1;
    },
    [](CallbackData *cd, Local<Value> *argv) {
      argv[0] = Null(cd->isolate);
      argv[1] = String::NewFromUtf8(cd->isolate, cd->data);
    }
  );
}

void FMTuner::ReadRDSWrap(const FunctionCallbackInfo<Value> &args) { 
  Isolate *isolate = args.GetIsolate(); 
   
  if(args.Length() != 2) { 
    isolate->ThrowException(Exception::TypeError( 
      String::NewFromUtf8(isolate, "Wrong number of arguments, need a channel and a callback function"))); 
    return; 
  } 
   
  if(!args[0]->IsNumber()) { 
    isolate->ThrowException(Exception::TypeError( 
      String::NewFromUtf8(isolate, "Argument should be number"))); 
    return; 
  } 

  if(!args[1]->IsFunction()) { 
    isolate->ThrowException(Exception::TypeError( 
      String::NewFromUtf8(isolate, "Argument should be function"))); 
    return; 
  } 
   
  FMTuner *tuner = ObjectWrap::Unwrap<FMTuner>(args.Holder()); 
  tuner->ReadRDS(isolate, Local<Number>::Cast(args[0]), Local<Function>::Cast(args[1])); 
}



void FMTuner::Init(Isolate *isolate, Local<Object> &exports) {
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "FMTuner"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  
  //Prototype
  NODE_SET_PROTOTYPE_METHOD(tpl, "powerOn", PowerOnWrap);
  NODE_SET_PROTOTYPE_METHOD(tpl, "powerOff", PowerOffWrap);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setChannel", SetChannelWrap);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getChannel", GetChannelWrap);
  NODE_SET_PROTOTYPE_METHOD(tpl, "seekUp", SeekUpWrap);
  NODE_SET_PROTOTYPE_METHOD(tpl, "seekDown", SeekDownWrap);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setVolume", SetVolumeWrap);
  NODE_SET_PROTOTYPE_METHOD(tpl, "readRDS", ReadRDSWrap);
  
  constructor.Reset(isolate, tpl->GetFunction());
  
  exports->Set(String::NewFromUtf8(isolate, "FMTuner"), tpl->GetFunction());
}

void FMTuner::New(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  
  if(args.IsConstructCall()) {
    
    //called as `new FMTuner(...)`
    
    if(args.Length() != 2)
    {
      isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, "Wrong number of arguments, need a host and a port")));
      return;
    }
    
    if(!args[0]->IsNumber())
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "First argument must be reset pin number")));
      return;
    }
    if(!args[1]->IsNumber())
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Second argument must be sdio pin number")));
      return;
    }
    
    int resetPin = (int)args[0]->ToInteger()->Value();
    int sdioPin = (int)args[1]->ToInteger()->Value();
    
    
    FMTuner *tuner = new FMTuner(resetPin, sdioPin);
    tuner->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
    
  }
  else {
    //called as plain function `FMTuner(...)`, turn into construct call
    const int argc = 2;
    Local<Value> argv[argc] = { args[0], args[1] };
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    args.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void FMTuner::NewInstance(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  
  const int argc = 2;
  Local<Value> argv[argc] = { args[0], args[1] };
  Local<Function> cons = Local<Function>::New(isolate, constructor);
  Local<Object> instance = cons->NewInstance(argc, argv);
  
  args.GetReturnValue().Set(instance);
}

}