#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdexcept>

namespace NeoDex
{

// Minimal, dependency-free JSON reader.
//
// This is intentionally NOT a general-purpose JSON library: it covers only
// what NeoDex's data files need (objects, arrays, strings, numbers, bools).
// Kept dependency-free on purpose, so the project builds on Termux with
// nothing beyond a standard C++17 toolchain.
class JsonValue
{
public:
    enum class Type
    {
        Null,
        Boolean,
        Number,
        String,
        Array,
        Object
    };

    JsonValue();

    static JsonValue parse(const std::string& text);
    static JsonValue parseFile(const std::string& path);

    Type getType() const;

    bool isNull() const;
    bool isObject() const;
    bool isArray() const;

    // Object access. Throws std::out_of_range if key is missing.
    const JsonValue& operator[](const std::string& key) const;
    bool hasKey(const std::string& key) const;

    // Array access.
    const JsonValue& operator[](size_t index) const;
    size_t size() const;

    std::string asString(const std::string& defaultValue = "") const;
    int asInt(int defaultValue = 0) const;
    double asDouble(double defaultValue = 0.0) const;
    bool asBool(bool defaultValue = false) const;

    // Convenience for arrays of strings, e.g. ["Fire", "Flying"]
    std::vector<std::string> asStringArray() const;

    // Convenience for arrays of ints, e.g. [2, 3]
    std::vector<int> asIntArray() const;

private:
    Type type;
    bool boolValue;
    double numberValue;
    std::string stringValue;
    std::vector<JsonValue> arrayValue;
    std::map<std::string, JsonValue> objectValue;

    // --- parsing internals ---
    struct Parser
    {
        const std::string& text;
        size_t pos;

        explicit Parser(const std::string& t);

        void skipWhitespace();
        char peek() const;
        char advance();
        void expect(char c);

        JsonValue parseValue();
        JsonValue parseObject();
        JsonValue parseArray();
        JsonValue parseString();
        JsonValue parseNumber();
        JsonValue parseLiteral();
    };
};

}
