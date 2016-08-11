#ifndef FMTUNER_H
#define FMTUNER_H

#include <node.h>
#include <node_object_wrap.h>
#include <cstring>
#include <cstdlib>
#include <uv.h>
#include "rpi-si4703/Si4703_Breakout.h"

namespace rpisi4703
{

class FMTuner : public node::ObjectWrap {
  public:
    FMTuner(int resetPin, int sdioPin);
    //v8 Object Creation
    static void Init(v8::Isolate* isolate, v8::Local<v8::Object>& exports);
    static void NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);

    void PowerOn(v8::Isolate *isolate, v8::Local<v8::Function> cb);
    void PowerOff(v8::Isolate *isolate, v8::Local<v8::Function> cb);
    void SeekUp(v8::Isolate *isolate, v8::Local<v8::Function> cb);
    void SeekDown(v8::Isolate *isolate, v8::Local<v8::Function> cb);
    void GetChannel(v8::Isolate *isolate, v8::Local<v8::Function> cb);
    void SetChannel(v8::Isolate *isolate, v8::Local<v8::Number> channel, v8::Local<v8::Function> cb);
    void SetVolume(v8::Isolate *isolate, v8::Local<v8::Number> volume, v8::Local<v8::Function> cb);
    void ReadRDS(v8::Isolate *isolate, v8::Local<v8::Number> timeout, v8::Local<v8::Function> cb);

    Si4703_Breakout *GetRadio();

  private:
    ~FMTuner();

    //wrappers
    static void PowerOnWrap(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void PowerOffWrap(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void SetChannelWrap(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void GetChannelWrap(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void SeekUpWrap(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void SeekDownWrap(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void SetVolumeWrap(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void ReadRDSWrap(const v8::FunctionCallbackInfo<v8::Value> &args);

    //v8 function stuff    
    static void New(const v8::FunctionCallbackInfo<v8::Value> &args);
    static v8::Persistent<v8::Function> constructor;

    Si4703_Breakout *radio;
};

}

#endif
