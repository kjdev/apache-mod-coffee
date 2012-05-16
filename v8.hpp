#ifndef V8_JS_HPP
#define V8_JS_HPP

/* v8 */
#include "v8.h"

/* coffee-script.hpp */
#include "coffee-script.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "http_log.h"
#include "ap_config.h"
#include "apr_strings.h"
#ifdef __cplusplus
}
#endif

/* log */
#ifdef AP_COFFEE_DEBUG_LOG_LEVEL
#define COFFEE_DEBUG_LOG_LEVEL AP_V8_DEBUG_LOG_LEVEL
#else
#define COFFEE_DEBUG_LOG_LEVEL APLOG_DEBUG
#endif

#define _RERR(r, format, args...)                                       \
    ap_log_rerror(APLOG_MARK, APLOG_CRIT, 0,                            \
                  r, "[COFFEE] %s(%d): "format, __FILE__, __LINE__, ##args)
#define _SERR(s, format, args...)                                       \
    ap_log_error(APLOG_MARK, APLOG_CRIT, 0,                             \
                 s, "[COFFEE] %s(%d): "format, __FILE__, __LINE__, ##args)
#define _PERR(p, format, args...)                                       \
    ap_log_perror(APLOG_MARK, APLOG_CRIT, 0,                            \
                  p, "[COFFEE] %s(%d): "format, __FILE__, __LINE__, ##args)

#define _RDEBUG(r, format, args...)                                     \
    ap_log_rerror(APLOG_MARK, COFFEE_DEBUG_LOG_LEVEL, 0,                \
                  r, "[COFFEE_DEBUG] %s(%d): "format, __FILE__, __LINE__, ##args)
#define _SDEBUG(s, format, args...)                                     \
    ap_log_error(APLOG_MARK, COFFEE_DEBUG_LOG_LEVEL, 0,                 \
                 s, "[COFFEE_DEBUG] %s(%d): "format, __FILE__, __LINE__, ##args)
#define _PDEBUG(p, format, args...)                                     \
    ap_log_perror(APLOG_MARK, COFFEE_DEBUG_LOG_LEVEL, 0,                \
                  p, "[COFFEE_DEBUG] %s(%d): "format, __FILE__, __LINE__, ##args)

/* V8::js class */
namespace V8 {
class js
{
public:
    js() {
        if (!v8::Context::InContext()) {
            context_enter_ = true;
            global_context_ = v8::Context::New();
            global_context_->Enter();
            context_ = v8::Local<v8::Context>::New(global_context_);
        } else {
            context_enter_ = false;
            context_ = v8::Context::GetCurrent();
        }

        v8::Context::Scope scope(context_);
    }

    bool init(void) {
        v8::TryCatch try_catch;

        //Compile coffee-script.js
        v8::Handle<v8::Script> script =
            v8::Script::Compile(v8::String::New(coffee_script_compiler_js));
        if (script.IsEmpty()) {
            v8::String::Utf8Value error(try_catch.Exception());
            _PERR(NULL, "Failed: %s", *error);
            return false;
        }

        //Run the script to get the result.
        v8::Handle<v8::Value> result = script->Run();
        if (result.IsEmpty()) {
            v8::String::Utf8Value error(try_catch.Exception());
            _PERR(NULL, "Failed: %s", *error);
            return false;
        }

        //Get CoffeeScript object.
        coffee_ = v8::Local<v8::Object>::Cast(
            context_->Global()->Get(v8::String::New("CoffeeScript")));

        compile_ = v8::Local<v8::Function>::Cast(
            coffee_->GetRealNamedProperty(v8::String::New("compile")));

        return true;
    }

    ~js() {
        if (context_enter_) {
            global_context_->DetachGlobal();
            global_context_->Exit();
            global_context_.Dispose();
        }
    }

    bool run(const char *src, apr_size_t len,
             request_rec *r, int *code, bool bare) {
        v8::TryCatch try_catch;

        v8::Local<v8::Value> args[2];

        v8::Local<v8::Object> options = v8::Object::New();
        options->Set(v8::String::New("filename"), v8::Undefined());
        options->Set(v8::String::New("bare"), v8::Boolean::New(bare));

        args[0] = v8::String::New(src, len);
        args[1] = options;

        v8::Handle<v8::Value> result = compile_->Call(coffee_, 2, args);
        if (try_catch.HasCaught()) {
            v8::String::Utf8Value error(try_catch.Exception());
            _RERR(r, "Script(%s) Failed: %s", r->filename, *error);
            return false;
        } else {
            v8::String::Utf8Value code(result->ToString());
            ap_rputs(*code, r);
        }

        return true;
    }

private:
    bool context_enter_;

    v8::HandleScope scope_;
    v8::Persistent<v8::Context> global_context_;

    v8::Local<v8::Context> context_;

    v8::Local<v8::Object> coffee_;
    v8::Local<v8::Function> compile_;
};
}

#endif // V8_JS_HPP
