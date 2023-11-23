#pragma once

#include "core.h"
#include "collections.h"


namespace rustjson {

class Value;
using Array = core::Vec<Value>;
using Object = core::collections::HashMap<core::String, Value>;
struct Error {
    enum ErrorKind {
        UNEXPECTED_END_OF_STRING,
        EXPECTED_VALUE_OR_CLOSER,
        EXPECTED_VALUE,
        EXPECTED_COMMA_OR_CLOSER,
        STRING_IS_NOT_VALID_PRIMITIVE,
        EXPECTED_KEY_OR_CLOSER,
        EXPECTED_KEY,
        EXPECTED_COLON,
    } kind;

    core::String message;

    Error(ErrorKind _kind, core::String _message): kind(_kind) {
        switch (kind) {
            case UNEXPECTED_END_OF_STRING:
                message = "unexpected end of string" + _message;
                break;
            case EXPECTED_VALUE_OR_CLOSER:
                message = "expected value or closer" + _message;
                break;
            case EXPECTED_VALUE:
                message = "expected value" + _message;
                break;
            case EXPECTED_COMMA_OR_CLOSER:
                message = "expected comma or closer" + _message;
                break;
            case STRING_IS_NOT_VALID_PRIMITIVE:
                message = "string is not a valid primitive" + _message;
                break;
            case EXPECTED_KEY_OR_CLOSER:
                message = "expected key or closer" + _message;
                break;
            case EXPECTED_KEY:
                message = "expected key" + _message;
                break;
            case EXPECTED_COLON:
                message = "expected colon" + _message;
                break;
        }
    }

    const char* to_string() const {
        return message.c_str();
    }
};
using Result = core::Result<Value, Error>;


class Value {
private:
    enum ValueKind {
        JSON_NULL,
        JSON_BOOL,
        JSON_I64,
        JSON_U64,
        JSON_F64,
        JSON_STR,
        JSON_ARRAY,
        JSON_OBJECT,
    } m_kind;

    typedef std::variant<bool, i64, u64, f64, core::String, Array, Object> ValueData;

    ValueData m_data;

    Value(ValueKind kind, ValueData data): m_kind(kind), m_data(data) {}

    static bool is_json_int(char* str) {
        if (*str == '-') { str++; }

        for (char* c = str; *c != '\0'; c++) {
            if (*c < 48 || *c > 57) {
                return false;
            }
        }

        return true;
    }

    static core::String array_to_string(const Array& self) {
        core::String buf = "[";
        usize i = 0;

        for (const auto& val : self) {
            if (i > 0) { buf += ","; }
            buf += val.to_string();
            i++;
        }

        buf += "]";
        return buf;
    }

    static core::String array_to_string_pretty(const Array& self, usize depth) {
        core::String buf = "[\n";
        usize i = 0;

        for (const auto& val : self) {
            if (i > 0) { buf += ",\n"; }
            for (usize j = 0; j < depth; j++) { buf += "    "; }
            buf += val.to_string_pretty(depth + 1);
            i++;
        }

        buf += "\n";
        for (usize j = 0; j < depth - 1; j++) { buf += "    "; }
        buf += "]";
        return buf;
    }

    static core::String object_to_string(const Object& self) {
        core::String buf = "{";
        usize i = 0;

        for (const auto& [key, val] : self) {
            if (i > 0) { buf += ","; }
            buf += "\"" + key + "\":" + val.to_string();
            i++;
        }

        buf += "}";
        return buf;
    }

    static core::String object_to_string_pretty(const Object& self, usize depth) {
        core::String buf = "{\n";
        usize i = 0;

        for (const auto& [key, val] : self) {
            if (i > 0) { buf += ",\n"; }
            for (usize j = 0; j < depth; j++) { buf += "    "; }
            buf += "\"" + key + "\": " + val.to_string_pretty(depth + 1);
            i++;
        }

        buf += "\n";
        for (usize j = 0; j < depth - 1; j++) { buf += "    "; }
        buf += "}";
        return buf;
    }

    enum JsonParseState {
            PARSE_STATE_ROOT = 0,
            PARSE_STATE_WAIT_KEY_OR_END,
            PARSE_STATE_WAIT_KEY,
            PARSE_STATE_KEY,
            PARSE_STATE_COLON,
            PARSE_STATE_WAIT_VALUE_OR_END,
            PARSE_STATE_WAIT_VALUE,
            PARSE_STATE_VALUE_STR,
            PARSE_STATE_VALUE_OTHER,
            PARSE_STATE_COMMA,
    };

    static Result parse_array(char** src) {
        JsonParseState state = PARSE_STATE_ROOT;
        auto result = Array::make();
        char* val = NULL;

        for (; **src != '\0'; (*src)++) {
            char* c = *src;

            switch (state) {
                case PARSE_STATE_ROOT:
                    switch (*c) {
                        case ' ': case '\n': case '\t': case '\r':
                            break;
                        case '[':
                            state = PARSE_STATE_WAIT_VALUE_OR_END;
                            break;
                        default:
                            panic("unreachable");
                    }
                    break;
                case PARSE_STATE_WAIT_VALUE_OR_END:
                    switch (*c) {
                        case ' ': case '\n': case '\t': case '\r':
                            break;
                        case '\"':
                            state = PARSE_STATE_VALUE_STR;
                            val = c + 1;
                            break;
                        case '0': case '1': case '2': case '3': case '4': case '5':case '6': case '7': case '8': case '9': case '-': case 'n':
                            state = PARSE_STATE_VALUE_OTHER;
                            val = c;
                            break;
                        case '[': {
                            state = PARSE_STATE_COMMA;
                            auto temp = Value::parse_array(src);
                            prop_res(temp, result.push(*inner));
                            break; }
                        case '{': {
                            state = PARSE_STATE_COMMA;
                            auto temp = Value::parse_object(src);
                            prop_res(temp, result.push(*inner));
                            break; }
                        case ']':
                            return Result::Ok(Value::from_array(result));
                        default:
                            return Result::Err(Error(Error::EXPECTED_VALUE_OR_CLOSER, ""));
                    }
                    break;
                case PARSE_STATE_WAIT_VALUE:
                    switch (*c) {
                        case ' ': case '\n': case '\t': case '\r':
                            break;
                        case '\"':
                            state = PARSE_STATE_VALUE_STR;
                            val = c + 1;
                            break;
                        case '0': case '1': case '2': case '3': case '4': case '5':case '6': case '7': case '8': case '9': case '-': case 'n':
                            state = PARSE_STATE_VALUE_OTHER;
                            val = c;
                            break;
                        case '[': {
                            state = PARSE_STATE_COMMA;
                            auto temp = Value::parse_array(src);
                            prop_res(temp, result.push(*inner));
                            break; }
                        case '{': {
                            state = PARSE_STATE_COMMA;
                            auto temp = Value::parse_object(src);
                            prop_res(temp, result.push(*inner));
                            break; }
                        default:
                            return Result::Err(Error(Error::EXPECTED_VALUE, ""));
                    }
                    break;
                case PARSE_STATE_VALUE_STR:
                    switch (*c) {
                        case '\"':
                            state = PARSE_STATE_COMMA;
                            *c = '\0';
                            result.push(Value::from_str(val));
                            break;
                        // handle escapes, errors
                    }
                    break;
                case PARSE_STATE_VALUE_OTHER:
                    switch (*c) {
                        case ',': {
                            state = PARSE_STATE_WAIT_VALUE;
                            *c = '\0';
                            auto temp = Value::parse_primitive(val);
                            prop_res(temp, result.push(*inner));
                            break; }
                        case ' ': case '\n': case '\t': case '\r': {
                            state = PARSE_STATE_COMMA;
                            *c = '\0';
                            auto temp = Value::parse_primitive(val);
                            prop_res(temp, result.push(*inner));
                            break; }
                        case ']': {
                            *c = '\0';
                            auto temp = Value::parse_primitive(val);
                            prop_res(temp, result.push(*inner));
                            return Result::Ok(Value::from_array(result)); }
                        // handle escapes, errors
                    }
                    break;
                case PARSE_STATE_COMMA:
                    switch (*c) {
                        case ' ': case '\n': case '\t': case '\r':
                            break;
                        case ',':
                            state = PARSE_STATE_WAIT_VALUE;
                            break;
                        case ']':
                            return Result::Ok(Value::from_array(result));
                        default:
                            return Result::Err(Error(Error::EXPECTED_COMMA_OR_CLOSER, core::String(" found '") + *c + '\''));
                    }
                    break;
                default:
                    break;
            }
        }

        return Result::Err(Error(Error::UNEXPECTED_END_OF_STRING, ""));
    }

    static Result parse_object(char** src) {
        JsonParseState state = PARSE_STATE_ROOT;
        auto result = Object::make();
        char* key = NULL;
        char* val = NULL;

        for (; **src != '\0'; (*src)++) {
            char* c = *src;

            switch (state) {
                case PARSE_STATE_ROOT:
                    switch (*c) {
                        case ' ': case '\n': case '\t': case '\r':
                            break;
                        case '{':
                            state = PARSE_STATE_WAIT_KEY;
                            break;
                        default:
                            panic("unreachable");
                    }
                    break;
                case PARSE_STATE_WAIT_KEY_OR_END:
                    switch (*c) {
                        case ' ': case '\n': case '\t': case '\r':
                            break;
                        case '\"':
                            state = PARSE_STATE_KEY;
                            key = c + 1;
                            break;
                        case '}':
                            return Result::Ok(Value::from_object(result));
                        default:
                            return Result::Err(Error(Error::EXPECTED_KEY_OR_CLOSER, ""));
                    }
                    break;
                case PARSE_STATE_WAIT_KEY:
                    switch (*c) {
                        case ' ': case '\n': case '\t': case '\r':
                            break;
                        case '\"':
                            state = PARSE_STATE_KEY;
                            key = c + 1;
                            break;
                        default:
                            return Result::Err(Error(Error::EXPECTED_KEY, ""));
                    }
                    break;
                case PARSE_STATE_KEY:
                    switch (*c) {
                        case '\"':
                            state = PARSE_STATE_COLON;
                            *c = '\0';
                            break;
                        // handle escapes, errors
                    }
                    break;
                case PARSE_STATE_COLON:
                    switch (*c) {
                        case ' ': case '\n': case '\t': case '\r':
                            break;
                        case ':':
                            state = PARSE_STATE_WAIT_VALUE;
                            break;
                        default:
                            return Result::Err(Error(Error::EXPECTED_COLON, ""));
                    }
                    break;
                case PARSE_STATE_WAIT_VALUE:
                    switch (*c) {
                        case ' ': case '\n': case '\t': case '\r':
                            break;
                        case '\"':
                            state = PARSE_STATE_VALUE_STR;
                            val = c + 1;
                            break;
                        case '0': case '1': case '2': case '3': case '4': case '5':case '6': case '7': case '8': case '9': case '-': case 'n':
                            state = PARSE_STATE_VALUE_OTHER;
                            val = c;
                            break;
                        case '[': {
                            state = PARSE_STATE_COMMA;
                            auto temp = Value::parse_array(src);
                            prop_res(temp, result.insert(key, *inner));
                            break; }
                        case '{': {
                            state = PARSE_STATE_COMMA;
                            auto temp = Value::parse_object(src);
                            prop_res(temp, result.insert(key, *inner));
                            break; }
                        default:
                            return Result::Err(Error(Error::EXPECTED_VALUE, ""));
                    }
                    break;
                case PARSE_STATE_VALUE_STR:
                    switch (*c) {
                        case '\"':
                            state = PARSE_STATE_COMMA;
                            *c = '\0';
                            result.insert(key, Value::from_str(val));
                            break;
                        // handle escapes, errors
                    }
                    break;
                case PARSE_STATE_VALUE_OTHER:
                    switch (*c) {
                        case ',': {
                            state = PARSE_STATE_WAIT_KEY;
                            *c = '\0';
                            auto temp = Value::parse_primitive(val);
                            prop_res(temp, result.insert(key, *inner));
                            break; }
                        case ' ': case '\n': case '\t': case '\r': {
                            state = PARSE_STATE_COMMA;
                            *c = '\0';
                            auto temp = Value::parse_primitive(val);
                            prop_res(temp, result.insert(key, *inner));
                            break; }
                        case '}': {
                            *c = '\0';
                            auto temp = Value::parse_primitive(val);
                            prop_res(temp, result.insert(key, *inner));
                            return Result::Ok(Value::from_object(result)); }
                        // default:
                        //     *c = '\0';
                        //     return Result::Err(Error(Error::STRING_IS_NOT_VALID_PRIMITIVE, std::string(" '") + val + '\''));
                    }
                    break;
                case PARSE_STATE_COMMA:
                    switch (*c) {
                        case ' ': case '\n': case '\t': case '\r':
                            break;
                        case ',':
                            state = PARSE_STATE_WAIT_KEY;
                            break;
                        case '}':
                            return Result::Ok(Value::from_object(result));
                        default:
                            return Result::Err(Error(Error::EXPECTED_COMMA_OR_CLOSER, core::String(" found '") + *c + '\''));
                    }
                    break;
                default:
                    break;
            }
        }

        return Result::Err(Error(Error::UNEXPECTED_END_OF_STRING, ""));
    }

    static Result parse_primitive(char* src) {
        if (strncmp(src, "null", 8) == 0) {
            return Result::Ok(Value::new_null());
        }

        if (strncmp(src, "true", 8) == 0) {
            return Result::Ok(Value::from_bool(true));
        }

        if (strncmp(src, "false", 8) == 0) {
            return Result::Ok(Value::from_bool(false));
        }

        if (Value::is_json_int(src)) {
            return Result::Ok(Value::from_i64(_atoi64(src)));
        }

        return Result::Err(Error(Error::STRING_IS_NOT_VALID_PRIMITIVE, src));
    }

public:
    Value(): m_kind(JSON_NULL), m_data(false) {}
    Value(const Value& other): m_kind(other.m_kind), m_data(other.m_data) {}
    Value(bool val)          : m_kind(JSON_BOOL),    m_data(val) {}
    Value(i64 val)           : m_kind(JSON_I64),     m_data(val) {}
    Value(u64 val)           : m_kind(JSON_U64),     m_data(val) {}
    Value(f64 val)           : m_kind(JSON_F64),     m_data(val) {}
    Value(core::String val)  : m_kind(JSON_STR),     m_data(val) {}
    Value(Array val)         : m_kind(JSON_ARRAY),   m_data(val) {}
    Value(Object val)        : m_kind(JSON_OBJECT),  m_data(val) {}

    static Value from_bool(bool val)         { return Value(JSON_BOOL, val); }
    static Value from_i64 (i64 val)          { return Value(JSON_I64, val); }
    static Value from_u64 (u64 val)          { return Value(JSON_U64, val); }
    static Value from_f64 (f64 val)          { return Value(JSON_F64, val); }
    static Value from_str (core::String val) { return Value(JSON_STR, val); }
    static Value from_array (Array val)      { return Value(JSON_ARRAY, val); }
    static Value from_object(Object val)     { return Value(JSON_OBJECT, val); }

    static Value new_null()   { return Value(JSON_NULL, false); }
    static Value new_bool()   { return Value::from_bool(false); }
    static Value new_i64 ()   { return Value::from_i64(0); }
    static Value new_u64 ()   { return Value::from_i64(0); }
    static Value new_f64 ()   { return Value::from_i64(0.f); }
    static Value new_str ()   { return Value::from_str(""); }
    static Value new_array () { return Value::from_array(Array::make()); }
    static Value new_object() { return Value::from_object(Object::make()); }

#define GETTER_VAL(T, kind) core::Option<T> as_##T() const { if (m_kind == kind) { return core::Option<T>::Some( std::get<T>(m_data)); } else { return core::Option<T>::None(); } }
#define GETTER_REF(T, kind, name) core::Option<const T*> as_##name() const { if (m_kind == kind) { return core::Option<const T*>::Some(&std::get<T>(m_data)); } else { return core::Option<const T*>::None(); } }
#define GETTER_MUT(T, kind, name) core::Option<T*> as_##name##_mut() { if (m_kind == kind) { return core::Option<T*>::Some(&std::get<T>(m_data)); } else { return core::Option<T*>::None(); } }

    GETTER_VAL(bool,         JSON_BOOL)
    core::Option<i64> as_i64() const {
        if (m_kind == JSON_I64) {
            return core::Option<i64>::Some(std::get<i64>(m_data));
        } else if (m_kind == JSON_U64) {
            return core::Option<i64>::Some((i64)std::get<u64>(m_data));
        } else if (m_kind == JSON_F64) {
            return core::Option<i64>::Some((i64)std::get<f64>(m_data));
        } else {
            return core::Option<i64>::None();
        }
    }
    core::Option<u64> as_u64() const {
        if (m_kind == JSON_I64) {
            return core::Option<u64>::Some((u64)std::get<i64>(m_data));
        } else if (m_kind == JSON_U64) {
            return core::Option<u64>::Some(std::get<u64>(m_data));
        } else if (m_kind == JSON_F64) {
            return core::Option<u64>::Some((u64)std::get<f64>(m_data));
        } else {
            return core::Option<u64>::None();
        }
    }
    core::Option<f64> as_f64() const {
        if (m_kind == JSON_I64) {
            return core::Option<f64>::Some((f64)std::get<i64>(m_data));
        } else if (m_kind == JSON_U64) {
            return core::Option<f64>::Some((f64)std::get<u64>(m_data));
        } else if (m_kind == JSON_F64) {
            return core::Option<f64>::Some(std::get<f64>(m_data));
        } else {
            return core::Option<f64>::None();
        }
    }
    GETTER_REF(core::String, JSON_STR,    str)
    GETTER_REF(Array,        JSON_ARRAY,  array)
    GETTER_REF(Object,       JSON_OBJECT, object)

    GETTER_MUT(bool,         JSON_BOOL,   bool)
    GETTER_MUT(i64,          JSON_I64,    i64)
    GETTER_MUT(u64,          JSON_U64,    u64)
    GETTER_MUT(f64,          JSON_F64,    f64)
    GETTER_MUT(core::String, JSON_STR,    str)
    GETTER_MUT(Array,        JSON_ARRAY,  array)
    GETTER_MUT(Object,       JSON_OBJECT, object)

#define FORCE_VAL(T) T get_##T(const core::String& key) const { return as_object().expect("value is not object")->get(key).expect("key is not in map")->as_##T().expect("value is not expected type"); } \
                     T get_##T(usize index)             const { return as_array().expect("value is not array")->get(index).expect("index out of bounds")->as_##T().expect("value is not expected type"); }
#define FORCE_REF(T, name) const T& get_##name(const core::String& key) const { return *as_object().expect("value is not object")->get(key).expect("key is not in map")->as_##name().expect("value is not expected type"); } \
                           const T& get_##name(usize index)             const { return *as_array().expect("value is not array")->get(index).expect("index out of bounds")->as_##name().expect("value is not expected type"); }
#define FORCE_MUT(T, name) T& get_##name##_mut(const core::String& key) { return *as_object_mut().expect("value is not object")->get_mut(key).expect("key is not in map")->as_##name##_mut().expect("value is not expected type"); } \
                           T& get_##name##_mut(usize index)             { return *as_array_mut().expect("value is not array")->get_mut(index).expect("index out of bounds")->as_##name##_mut().expect("value is not expected type"); }

    const Value& get(const core::String& key) const { return *as_object().unwrap()->get(key).unwrap(); }
    Value& get_mut(const core::String& key) { return *as_object_mut().unwrap()->get_mut(key).unwrap(); }

    FORCE_VAL(bool)
    FORCE_VAL(i64)
    FORCE_VAL(u64)
    FORCE_VAL(f64)
    FORCE_REF(core::String, str)
    FORCE_REF(Array,        array)
    FORCE_REF(Object,       object)

    FORCE_MUT(bool,         bool)
    FORCE_MUT(i64,          i64)
    FORCE_MUT(u64,          u64)
    FORCE_MUT(f64,          f64)
    FORCE_MUT(core::String, str)
    FORCE_MUT(Array,        array)
    FORCE_MUT(Object,       object)

    void set_null  ()                  { m_kind = JSON_NULL; }
    void set_bool  (bool data)         { m_kind = JSON_BOOL; m_data = data; }
    void set_i64   (i64 data)          { m_kind = JSON_I64;  m_data = data; }
    void set_u64   (u64 data)          { m_kind = JSON_U64;  m_data = data; }
    void set_f64   (f64 data)          { m_kind = JSON_F64;  m_data = data; }
    void set_str   (core::String data) { m_kind = JSON_STR;    m_data = data; }
    void set_array (Array data)        { m_kind = JSON_ARRAY;  m_data = data; }
    void set_object(Object data)       { m_kind = JSON_OBJECT; m_data = data; }

    bool is_null  () const { return m_kind == JSON_NULL; }
    bool is_bool  () const { return m_kind == JSON_BOOL; }
    bool is_true  () const { if (m_kind != JSON_BOOL) { return false; } return  std::get<bool>(m_data); }
    bool is_false () const { if (m_kind != JSON_BOOL) { return false; } return !std::get<bool>(m_data); }
    bool is_i64   () const { return m_kind == JSON_I64; }
    bool is_u64   () const { return m_kind == JSON_U64; }
    bool is_f64   () const { return m_kind == JSON_F64; }
    bool is_str   () const { return m_kind == JSON_STR; }
    bool is_array () const { return m_kind == JSON_ARRAY; }
    bool is_object() const { return m_kind == JSON_OBJECT; }

    core::String to_string() const {
        switch (m_kind) {
            case JSON_NULL: return "null";
            case JSON_BOOL:
                if (std::get<bool>(m_data)) {
                    return "true";
                } else {
                    return "false";
                }
            case JSON_I64: return std::to_string(std::get<i64>(m_data));
            case JSON_U64: return std::to_string(std::get<u64>(m_data));
            case JSON_F64: return std::to_string(std::get<f64>(m_data));
            case JSON_STR: return "\"" + std::get<core::String>(m_data) + "\"";
            case JSON_ARRAY:  return Value::array_to_string(std::get<Array>(m_data));
            case JSON_OBJECT: return Value::object_to_string(std::get<Object>(m_data));
        }

        return "";
    }
    core::String to_string_pretty(usize depth = 1) const {
        switch (m_kind) {
            case JSON_NULL: return "null";
            case JSON_BOOL:
                if (std::get<bool>(m_data)) {
                    return "true";
                } else {
                    return "false";
                }
            case JSON_I64: return std::to_string(std::get<i64>(m_data));
            case JSON_U64: return std::to_string(std::get<u64>(m_data));
            case JSON_F64: return std::to_string(std::get<f64>(m_data));
            case JSON_STR: return "\"" + std::get<core::String>(m_data) + "\"";
            case JSON_ARRAY:  return Value::array_to_string_pretty(std::get<Array>(m_data), depth);
            case JSON_OBJECT: return Value::object_to_string_pretty(std::get<Object>(m_data), depth);
        }

        return "";
    }
    static Result parse(char* src) {
        return Value::parse_object(&src);
    }

};

}

