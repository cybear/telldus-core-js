#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <v8.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <telldus-core.h>

using namespace v8;
using namespace node;

#ifdef _WINDOWS
#define strcasecmp _stricmp
#define DEGREE " "
#else
#define DEGREE "°"
#endif



namespace telldus_v8 {

    const int SUPPORTED_METHODS =
    TELLSTICK_TURNON    
    | TELLSTICK_TURNOFF   
    | TELLSTICK_BELL      
    | TELLSTICK_TOGGLE    
    | TELLSTICK_DIM       
    | TELLSTICK_LEARN     
    | TELLSTICK_EXECUTE   
    | TELLSTICK_UP        
    | TELLSTICK_DOWN      
    | TELLSTICK_STOP;

    const int DATA_LENGTH = 20;

    Handle<Value> TdGetNumberOfDevices( const Arguments& args ) {
        HandleScope scope;
        //tdInit();
        Local<Number> num = Number::New(tdGetNumberOfDevices());
        return scope.Close(num);
    }

    Local<Object> GetSuportedMetods(int id){
    
        int methods = tdMethods( id, SUPPORTED_METHODS );
        Local<Array> methodsObj = Array::New();
        int i = 0;
        if (methods & TELLSTICK_TURNON)
        {
            methodsObj->Set(i++, String::New("TURNON"));
        }
        if (methods & TELLSTICK_TURNOFF)
        {
            methodsObj->Set(i++, String::New("TURNOFF"));
        }
        if (methods & TELLSTICK_BELL)
        {
            methodsObj->Set(i++, String::New("BELL"));
        }
        if (methods & TELLSTICK_TOGGLE)
        {
            methodsObj->Set(i++, String::New("TOGGLE"));
        }
        if (methods & TELLSTICK_DIM)
        {
            methodsObj->Set(i++, String::New("DIM"));
        }
        if (methods & TELLSTICK_UP)
        {
            methodsObj->Set(i++, String::New("UP"));
        }
        if (methods & TELLSTICK_DOWN)
        {
            methodsObj->Set(i++, String::New("DOWN"));
        }
        if (methods & TELLSTICK_STOP)
        {
            methodsObj->Set(i++, String::New("STOP"));
        }

        return methodsObj;

    }

    Local<String> GetDeviceType(int id){
        int type = tdGetDeviceType(id);

        if(type & TELLSTICK_TYPE_DEVICE){
            return String::New("DEVICE");
        }

        if(type & TELLSTICK_TYPE_GROUP){
            return String::New("GROUP");
        }

        if(type & TELLSTICK_TYPE_SCENE){
            return String::New("SCENE");
        }
        return String::New("UNKNOWN");
    }

    Local<Object> GetDeviceStatus(int id){
        Local<Object> status = Object::New();
        int lastSentCommand = tdLastSentCommand(id, SUPPORTED_METHODS);
        char *level = 0;
        switch(lastSentCommand) {
            case TELLSTICK_TURNON:
                status->Set(String::NewSymbol("name"), String::New("ON"));
                break;
            case TELLSTICK_TURNOFF:
                status->Set(String::NewSymbol("name"), String::New("OFF"));
                break;
            case TELLSTICK_DIM:
                status->Set(String::NewSymbol("name"), String::New("DIM"));
                level = tdLastSentValue(id);
                status->Set(String::NewSymbol("level"), String::New(level, strlen(level)));
                tdReleaseString(level);
                break;
            default:
                status->Set(String::NewSymbol("name"), String::New("Unknown"));
        }

        return status;
    }
    
    Local<Object> GetDevice(int index){

        int id = tdGetDeviceId( index );
        char *name = tdGetName( id );
        char *model = tdGetModel(id);
     
        Local<Object> obj = Object::New();
        obj->Set(String::NewSymbol("name"), String::New(name, strlen(name)));
        obj->Set(String::NewSymbol("id"), Number::New(id));
        obj->Set(String::NewSymbol("metods"), GetSuportedMetods(id));
        obj->Set(String::NewSymbol("model"), String::New(model, strlen(model)));
        obj->Set(String::NewSymbol("type"), GetDeviceType(id));
        obj->Set(String::NewSymbol("status"), GetDeviceStatus(id));

        tdReleaseString(name);
        tdReleaseString(model);

        return obj;
    }
    
    Handle<Value> TdGetDevices( const Arguments& args ) {
        HandleScope scope;
        tdInit();


        int intNumberOfDevices = tdGetNumberOfDevices();
        Local<Array> devices = Array::New(intNumberOfDevices);

        for (int i = 0; i < intNumberOfDevices; i++) {
           devices->Set(i, GetDevice(i));
         }
        
        return scope.Close(devices);
    }

}

extern "C"
void init(Handle<Object> target) {
    
     HandleScope scope;
  //Local<FunctionTemplate> t = FunctionTemplate::New(telldus_v8::tdGetNumberOfDevices);

  //target->Set( String::NewSymbol( "tdGetNumberOfDevices" ), t->GetFunction() );
   target->Set(String::NewSymbol("TdGetNumberOfDevices"),
      FunctionTemplate::New(telldus_v8::TdGetNumberOfDevices)->GetFunction());
   target->Set(String::NewSymbol("TdGetDevices"),
      FunctionTemplate::New(telldus_v8::TdGetDevices)->GetFunction());
}

NODE_MODULE(telldus, init)