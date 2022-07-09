#include "Object.h"
#include <memory>
#include <unordered_map>
#include "../MappingUtils.h"
#include "Class.h"
#include "ScriptEngine.h"
#include "Utils.h"

#define MAX_STRING_LEN 512
namespace se {
std::unique_ptr<std::unordered_map<Object*, void*>> __objectMap; // Currently, the value `void*` is always nullptr

Object::Object() {}
Object::~Object() {
    if (__objectMap) {
        __objectMap->erase(this);
    }
}

Object* Object::createObjectWithClass(Class* cls) {
    napi_value jsobj = Class::_createJSObjectWithClass(cls);
    Object*    obj   = Object::_createJSObject(ScriptEngine::getEnv(), jsobj, cls);
    return obj;
}

bool Object::setProperty(const char* name, const Value& data) {
    napi_status status;
    napi_value  jsVal;
    internal::seToJsValue(data, &jsVal);
    NODE_API_CALL(status, _env, napi_set_named_property(_env, _objRef.getValue(_env), name, jsVal));
    return status == napi_ok;
}

bool Object::getProperty(const char* name, Value* d) {
    napi_status status;
    napi_value  jsVal;
    Value       data;
    NODE_API_CALL(status, _env, napi_get_named_property(_env, _objRef.getValue(_env), name, &jsVal));
    if (status == napi_ok) {
        internal::jsToSeValue(jsVal, &data);
        *d = data;
        if (data.isUndefined()) {
            return false;
        }
        return true;
    }
    return false;
}

bool Object::isArray() const {
    napi_status status;
    bool        ret = false;
    NODE_API_CALL(status, _env, napi_is_array(_env, _objRef.getValue(_env), &ret));
    return ret;
}

bool Object::getArrayLength(uint32_t* length) const {
    napi_status status;
    uint32_t    len = 0;
    NODE_API_CALL(status, _env, napi_get_array_length(_env, _objRef.getValue(_env), &len));
    if (length) {
        *length = len;
    }
    return true;
}

bool Object::getArrayElement(uint32_t index, Value* data) const {
    napi_status status;
    napi_value  val;
    NODE_API_CALL(status, _env, napi_get_element(_env, _objRef.getValue(_env), index, &val));
    internal::jsToSeValue(val, data);
    return true;
}

bool Object::setArrayElement(uint32_t index, const Value& data) {
    napi_status status;
    napi_value  val;
    internal::seToJsValue(data, &val);
    NODE_API_CALL(status, _env, napi_set_element(_env, _objRef.getValue(_env), index, val));
    return true;
}

bool Object::isTypedArray() const {
    napi_status status;
    bool        ret = false;
    NODE_API_CALL(status, _env, napi_is_typedarray(_env, _objRef.getValue(_env), &ret));
    return ret;
}

Object::TypedArrayType Object::getTypedArrayType() const {
    napi_status          status;
    napi_typedarray_type type;
    napi_value           inputBuffer;
    size_t               byteOffset;
    size_t               length;
    NODE_API_CALL(status, _env, napi_get_typedarray_info(_env, _objRef.getValue(_env), &type, &length, NULL, &inputBuffer, &byteOffset));

    TypedArrayType ret = TypedArrayType::NONE;
    switch (type) {
        case napi_int8_array:
            ret = TypedArrayType::INT8;
            break;
        case napi_uint8_array:
            ret = TypedArrayType::UINT8;
            break;
        case napi_uint8_clamped_array:
            ret = TypedArrayType::UINT8_CLAMPED;
            break;
        case napi_int16_array:
            ret = TypedArrayType::INT16;
            break;
        case napi_uint16_array:
            ret = TypedArrayType::UINT16;
            break;
        case napi_int32_array:
            ret = TypedArrayType::INT32;
            break;
        case napi_uint32_array:
            ret = TypedArrayType::UINT32;
            break;
        case napi_float32_array:
            ret = TypedArrayType::FLOAT32;
            break;
        case napi_float64_array:
            ret = TypedArrayType::FLOAT64;
            break;
        default:
            break;
    }
    return ret;
}

bool Object::getTypedArrayData(uint8_t** ptr, size_t* length) const {
    napi_status          status;
    napi_typedarray_type type;
    napi_value           inputBuffer;
    size_t               byteOffset;
    size_t               byteLength;
    void*                data = nullptr;
    NODE_API_CALL(status, _env, napi_get_typedarray_info(_env, _objRef.getValue(_env), &type, &byteLength, &data, &inputBuffer, &byteOffset));
    *ptr = (uint8_t*)(data) + byteOffset;
    if (length) {
        *length = byteLength;
    }
    return true;
}

bool Object::isArrayBuffer() const {
    bool        ret = false;
    napi_status status;
    NODE_API_CALL(status, _env, napi_is_arraybuffer(_env, _objRef.getValue(_env), &ret));
    return ret;
}

bool Object::getArrayBufferData(uint8_t** ptr, size_t* length) const {
    napi_status status;
    size_t      len = 0;
    NODE_API_CALL(status, _env, napi_get_arraybuffer_info(_env, _objRef.getValue(_env), reinterpret_cast<void**>(ptr), &len));
    if (length) {
        *length = len;
    }
    return true;
}

Object* Object::createTypedArray(Object::TypedArrayType type, const void* data, size_t byteLength) {
    napi_status status;
    if (type == TypedArrayType::NONE) {
        SE_LOGE("Don't pass se::Object::TypedArrayType::NONE to createTypedArray API!");
        return nullptr;
    }

    if (type == TypedArrayType::UINT8_CLAMPED) {
        SE_LOGE("Doesn't support to create Uint8ClampedArray with Object::createTypedArray API!");
        return nullptr;
    }
    napi_typedarray_type napiType;
    napi_value           outputBuffer;
    void*                outputPtr = nullptr;
    NODE_API_CALL(status, ScriptEngine::getEnv(), napi_create_arraybuffer(ScriptEngine::getEnv(), byteLength, &outputPtr, &outputBuffer));
    size_t sizeOfEle = 0;
    switch (type) {
        case TypedArrayType::INT8:
            napiType  = napi_int8_array;
            sizeOfEle = 1;
            break;
        case TypedArrayType::UINT8:
            napiType  = napi_uint8_array;
            sizeOfEle = 1;
            break;
        case TypedArrayType::INT16:
            napiType  = napi_int8_array;
            sizeOfEle = 2;
        case TypedArrayType::UINT16:
            napiType  = napi_uint8_array;
            sizeOfEle = 2;
            break;
        case TypedArrayType::INT32:
            napiType  = napi_int32_array;
            sizeOfEle = 4;
        case TypedArrayType::UINT32:
            napiType  = napi_uint32_array;
            sizeOfEle = 4;
        case TypedArrayType::FLOAT32:
            napiType  = napi_float32_array;
            sizeOfEle = 4;
            break;
        case TypedArrayType::FLOAT64:
            napiType  = napi_float64_array;
            sizeOfEle = 8;
            break;
        default:
            assert(false); // Should never go here.
            break;
    }
    size_t     eleCounts = byteLength / sizeOfEle;
    napi_value outputArray;
    NODE_API_CALL(status, ScriptEngine::getEnv(), napi_create_typedarray(ScriptEngine::getEnv(), napiType, eleCounts, outputBuffer, 0, &outputArray));

    Object* obj = Object::_createJSObject(ScriptEngine::getEnv(), outputArray, nullptr);
    return obj;
}

bool Object::isFunction() const {
    napi_valuetype valuetype0;
    napi_status    status;
    NODE_API_CALL(status, _env, napi_typeof(_env, _objRef.getValue(_env), &valuetype0));
    return (valuetype0 == napi_function);
}

bool Object::defineFunction(const char* funcName, napi_callback func) {
    napi_value  fn;
    napi_status status;
    NODE_API_CALL(status, _env, napi_create_function(_env, funcName, NAPI_AUTO_LENGTH, func, NULL, &fn));
    NODE_API_CALL(status, _env, napi_set_named_property(_env, _objRef.getValue(_env), funcName, fn));
    return true;
}

bool Object::defineProperty(const char* name, napi_callback getter, napi_callback setter) {
    napi_status              status;
    napi_property_descriptor properties[] = {{name, nullptr, nullptr, getter, setter, 0, napi_default, 0}};
    LOGI("get this :%p", this);
    status = napi_define_properties(_env, _objRef.getValue(_env), sizeof(properties) / sizeof(napi_property_descriptor), properties);
    if (status == napi_ok) {
        return true;
    }
    return false;
}

Object* Object::_createJSObject(napi_env env, napi_value js_object, Class* cls) { // NOLINT(readability-identifier-naming)
    auto* ret = new Object();
    if (!ret->init(env, js_object, cls)) {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

Object* Object::createPlainObject() {
    napi_value  result;
    napi_status status;
    NODE_API_CALL(status, ScriptEngine::getEnv(), napi_create_object(ScriptEngine::getEnv(), &result));
    Object* obj = _createJSObject(ScriptEngine::getEnv(), result, nullptr);
    return obj;
}

Object* Object::createArrayObject(size_t length) {
    napi_value  result;
    napi_status status;
    NODE_API_CALL(status, ScriptEngine::getEnv(), napi_create_array_with_length(ScriptEngine::getEnv(), length, &result));
    Object* obj = _createJSObject(ScriptEngine::getEnv(), result, nullptr);
    return obj;
}

Object* Object::createArrayBufferObject(void* data, size_t byteLength) {
    napi_value  result;
    napi_status status;
    void*       retData;
    Object*     obj = nullptr;
    NODE_API_CALL(status, ScriptEngine::getEnv(), napi_create_arraybuffer(ScriptEngine::getEnv(), byteLength, &retData, &result));
    if (status == napi_ok) {
        if (data) {
            memcpy(retData, data, byteLength);
        }
        obj = _createJSObject(ScriptEngine::getEnv(), result, nullptr);
    }
    return obj;
}

bool Object::getAllKeys(std::vector<std::string>* allKeys) const {
    napi_status status;
    napi_value  names;

    NODE_API_CALL(status, _env, napi_get_property_names(_env, _objRef.getValue(_env), &names));
    if (status != napi_ok) {
        return false;
    }
    uint32_t name_len = 0;
    NODE_API_CALL(status, _env, napi_get_array_length(_env, names, &name_len));
    for (uint32_t i = 0; i < name_len; i++) {
        napi_value val;
        NODE_API_CALL(status, _env, napi_get_element(_env, names, i, &val));
        if (status == napi_ok) {
            char   buffer[MAX_STRING_LEN];
            size_t result = 0;
            NODE_API_CALL(status, _env, napi_get_value_string_utf8(_env, val, buffer, MAX_STRING_LEN, &result));
            if (result > 0) {
                allKeys->push_back(buffer);
            }
        }
    }

    return true;
}

bool Object::init(napi_env env, napi_value js_object, Class* cls) {
    assert(env);
    _cls = cls;
    _env = env;
    _objRef.initWeakref(env, js_object);

    if (__objectMap) {
        assert(__objectMap->find(this) == __objectMap->end());
        __objectMap->emplace(this, nullptr);
    }

    napi_status status;
    LOGI("init this :%p", this);
    return true;
}

bool Object::call(const ValueArray& args, Object* thisObject, Value* rval) {
    size_t                  argc = 0;
    std::vector<napi_value> argv;
    argv.reserve(10);
    argc = args.size();
    internal::seToJsArgs(_env, args, &argv);
    napi_value  return_val;
    napi_status status;
    assert(isFunction());
    napi_value thisObj = thisObject ? thisObject->_getJSObject() : nullptr;
    LOGE("qgh object::call start %{public}p", thisObj);
    status =
        napi_call_function(_env, thisObj, _getJSObject(), argc, argv.data(), &return_val);
    LOGE("qgh object::call end thisObj %{public}p _getJSObject  %{public}p", thisObj, _getJSObject());
    if (rval) {
        internal::jsToSeValue(return_val, rval);
    }
    LOGE("qgh object::call finish %{public}p", return_val);
    return true;
}

void Object::_setFinalizeCallback(napi_finalize finalizeCb) {
    assert(finalizeCb != nullptr);
    _finalizeCb = finalizeCb;
}

void* Object::getPrivateData() const {
    void*       obj;
    napi_status status;
    NODE_API_CALL(status, _env, napi_unwrap(_env, _objRef.getValue(_env), reinterpret_cast<void**>(&obj)));
    return obj;
}

void Object::setPrivateData(void* data) {
    assert(_privateData == nullptr);
    assert(NativePtrToObjectMap::find(data) == NativePtrToObjectMap::end());
    NativePtrToObjectMap::emplace(data, this);

    napi_status status;
    _privateData = data;

    napi_valuetype valType;
    NODE_API_CALL(status, ScriptEngine::getEnv(), napi_typeof(ScriptEngine::getEnv(), _objRef.getValue(_env), &valType));
    LOGI("this type is %d, native this:%p", valType, data);

    //issue https://github.com/nodejs/node/issues/23999
    auto tmpThis = _objRef.getValue(_env);
    //_objRef.deleteRef();
    napi_ref result = nullptr;
    NODE_API_CALL(status, _env,
                  napi_wrap(_env, tmpThis, data, weakCallback,
                            (void*)this /* finalize_hint */, &result));
    //_objRef.setWeakref(_env, result);
    setProperty("__native_ptr__", se::Value(static_cast<uint64_t>(reinterpret_cast<uintptr_t>(data))));

    return;
}

bool Object::attachObject(Object* obj) {
    assert(obj);

    Object* global = ScriptEngine::getInstance()->getGlobalObject();
    Value   jsbVal;
    if (!global->getProperty("jsb", &jsbVal)) {
        return false;
    }
    Object* jsbObj = jsbVal.toObject();

    Value func;

    if (!jsbObj->getProperty("registerNativeRef", &func)) {
        return false;
    }

    ValueArray args;
    args.push_back(Value(this));
    args.push_back(Value(obj));
    func.toObject()->call(args, global);
    return true;
}

bool Object::detachObject(Object* obj) {
    assert(obj);

    Object* global = ScriptEngine::getInstance()->getGlobalObject();
    Value   jsbVal;
    if (!global->getProperty("jsb", &jsbVal)) {
        return false;
    }
    Object* jsbObj = jsbVal.toObject();

    Value func;

    if (!jsbObj->getProperty("unregisterNativeRef", &func)) {
        return false;
    }

    ValueArray args;
    args.push_back(Value(this));
    args.push_back(Value(obj));
    func.toObject()->call(args, global);
    return true;
}

std::string Object::toString() const {
    std::string ret;
    napi_status status;
    if (isFunction() || isArray() || isTypedArray()) {
        napi_value result;
        NODE_API_CALL(status, _env, napi_coerce_to_string(_env, _objRef.getValue(_env), &result));
        char   buffer[MAX_STRING_LEN];
        size_t result_t = 0;
        NODE_API_CALL(status, _env, napi_get_value_string_utf8(_env, result, buffer, MAX_STRING_LEN, &result_t));
        ret = buffer;
    } else if (isArrayBuffer()) {
        ret = "[object ArrayBuffer]";
    } else {
        ret = "[object Object]";
    }
    return ret;
}

void Object::root() {
    napi_status status;
    if (_rootCount == 0) {
        uint32_t result = 0;
        _objRef.incRef(_env);
        //NODE_API_CALL(status, _env, napi_reference_ref(_env, _wrapper, &result));
    }
    ++_rootCount;
}

void Object::unroot() {
    napi_status status;
    if (_rootCount > 0) {
        --_rootCount;
        if (_rootCount == 0) {
            _objRef.decRef(_env);
        }
    }
}

bool Object::isRooted() const {
    return _rootCount > 0;
}

Class* Object::_getClass() const {
    return _cls;
}

Object* Object::getObjectWithPtr(void* ptr) {
    Object* obj  = nullptr;
    auto    iter = NativePtrToObjectMap::find(ptr);
    if (iter != NativePtrToObjectMap::end()) {
        obj = iter->second;
        obj->incRef();
    }
    return obj;
}

napi_value Object::_getJSObject() const {
    return _objRef.getValue(_env);
}

void Object::weakCallback(napi_env env, void* nativeObject, void* finalizeHint /*finalize_hint*/) {
    if (finalizeHint) {
        Object* obj = reinterpret_cast<Object*>(finalizeHint);

        if (nativeObject == nullptr) {
            return;
        }

        auto iter = NativePtrToObjectMap::find(nativeObject);
        if (iter != NativePtrToObjectMap::end()) {
            Object* obj = iter->second;
            if (obj->_finalizeCb != nullptr) {
                obj->_finalizeCb(env, nativeObject, finalizeHint);
            } else {
                assert(obj->_getClass() != nullptr);
                if (obj->_getClass()->_getFinalizeFunction() != nullptr) {
                    obj->_getClass()->_getFinalizeFunction()(env, nativeObject, finalizeHint);
                }
            }
            obj->decRef();
            NativePtrToObjectMap::erase(iter);
        } else {
            //            assert(false);
        }
    }
}

void Object::setup() {
    __objectMap = std::make_unique<std::unordered_map<Object*, void*>>();
}

void Object::cleanup() {
    void*   nativeObj = nullptr;
    Object* obj       = nullptr;
    Class*  cls       = nullptr;

    const auto& nativePtrToObjectMap = NativePtrToObjectMap::instance();
    for (const auto& e : nativePtrToObjectMap) {
        nativeObj = e.first;
        obj       = e.second;

        if (obj->_finalizeCb != nullptr) {
            obj->_finalizeCb(ScriptEngine::getEnv(), nativeObj, nullptr);
        } else {
            if (obj->_getClass() != nullptr) {
                if (obj->_getClass()->_getFinalizeFunction() != nullptr) {
                    obj->_getClass()->_getFinalizeFunction()(ScriptEngine::getEnv(), nativeObj, nullptr);
                }
            }
        }
        obj->decRef();
    }

    NativePtrToObjectMap::clear();
    NonRefNativePtrCreatedByCtorMap::clear();

    if (__objectMap) {
        std::vector<Object*> toReleaseObjects;
        for (const auto& e : *__objectMap) {
            obj = e.first;
            cls = obj->_getClass();
            obj->_rootCount = 0;

            if (cls != nullptr && cls->getName() == "__PrivateData") {
                toReleaseObjects.push_back(obj);
            }
        }
        for (auto* e : toReleaseObjects) {
            e->decRef();
        }
    }

    __objectMap.reset();
}

Object* Object::createJSONObject(const std::string& jsonStr) {
    //not impl
    return nullptr;
}

} // namespace se