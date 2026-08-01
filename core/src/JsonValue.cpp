#include "../include/JsonValue.h"

#include <fstream>
#include <sstream>
#include <cctype>
#include <cstdlib>

namespace NeoDex
{

JsonValue::JsonValue()
: type(Type::Null), boolValue(false), numberValue(0.0)
{
}

JsonValue::Type JsonValue::getType() const { return type; }
bool JsonValue::isNull() const { return type == Type::Null; }
bool JsonValue::isObject() const { return type == Type::Object; }
bool JsonValue::isArray() const { return type == Type::Array; }

bool JsonValue::hasKey(const std::string& key) const
{
    return type == Type::Object && objectValue.find(key) != objectValue.end();
}

const JsonValue& JsonValue::operator[](const std::string& key) const
{
    if(type != Type::Object)
    {
        throw std::out_of_range("JsonValue::operator[] - not an object (key: " + key + ")");
    }

    auto it = objectValue.find(key);
    if(it == objectValue.end())
    {
        throw std::out_of_range("JsonValue::operator[] - missing key: " + key);
    }

    return it->second;
}

const JsonValue& JsonValue::operator[](size_t index) const
{
    if(type != Type::Array || index >= arrayValue.size())
    {
        throw std::out_of_range("JsonValue::operator[] - invalid array index");
    }

    return arrayValue[index];
}

size_t JsonValue::size() const
{
    if(type == Type::Array) return arrayValue.size();
    if(type == Type::Object) return objectValue.size();
    return 0;
}

std::string JsonValue::asString(const std::string& defaultValue) const
{
    return type == Type::String ? stringValue : defaultValue;
}

int JsonValue::asInt(int defaultValue) const
{
    return type == Type::Number ? static_cast<int>(numberValue) : defaultValue;
}

double JsonValue::asDouble(double defaultValue) const
{
    return type == Type::Number ? numberValue : defaultValue;
}

bool JsonValue::asBool(bool defaultValue) const
{
    return type == Type::Boolean ? boolValue : defaultValue;
}

std::vector<std::string> JsonValue::asStringArray() const
{
    std::vector<std::string> result;

    if(type != Type::Array) return result;

    for(const JsonValue& item : arrayValue)
    {
        result.push_back(item.asString());
    }

    return result;
}

std::vector<int> JsonValue::asIntArray() const
{
    std::vector<int> result;

    if(type != Type::Array) return result;

    for(const JsonValue& item : arrayValue)
    {
        result.push_back(item.asInt());
    }

    return result;
}

JsonValue JsonValue::parse(const std::string& text)
{
    Parser parser(text);
    parser.skipWhitespace();
    JsonValue result = parser.parseValue();
    return result;
}

JsonValue JsonValue::parseFile(const std::string& path)
{
    std::ifstream file(path);

    if(!file.is_open())
    {
        throw std::runtime_error("JsonValue::parseFile - could not open file: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return parse(buffer.str());
}

// ---------------------------------------------------------------------------
// Parser
// ---------------------------------------------------------------------------

JsonValue::Parser::Parser(const std::string& t) : text(t), pos(0) {}

void JsonValue::Parser::skipWhitespace()
{
    while(pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos])))
    {
        pos++;
    }
}

char JsonValue::Parser::peek() const
{
    if(pos >= text.size())
    {
        throw std::runtime_error("JsonValue: unexpected end of input");
    }
    return text[pos];
}

char JsonValue::Parser::advance()
{
    char c = peek();
    pos++;
    return c;
}

void JsonValue::Parser::expect(char c)
{
    if(advance() != c)
    {
        throw std::runtime_error(std::string("JsonValue: expected '") + c + "'");
    }
}

JsonValue JsonValue::Parser::parseValue()
{
    skipWhitespace();
    char c = peek();

    if(c == '{') return parseObject();
    if(c == '[') return parseArray();
    if(c == '"') return parseString();
    if(c == 't' || c == 'f' || c == 'n') return parseLiteral();

    return parseNumber();
}

JsonValue JsonValue::Parser::parseObject()
{
    JsonValue result;
    result.type = Type::Object;

    expect('{');
    skipWhitespace();

    if(peek() == '}')
    {
        advance();
        return result;
    }

    while(true)
    {
        skipWhitespace();
        JsonValue key = parseString();
        skipWhitespace();
        expect(':');
        JsonValue value = parseValue();

        result.objectValue[key.asString()] = value;

        skipWhitespace();
        char next = advance();

        if(next == '}') break;
        if(next != ',')
        {
            throw std::runtime_error("JsonValue: expected ',' or '}' in object");
        }
    }

    return result;
}

JsonValue JsonValue::Parser::parseArray()
{
    JsonValue result;
    result.type = Type::Array;

    expect('[');
    skipWhitespace();

    if(peek() == ']')
    {
        advance();
        return result;
    }

    while(true)
    {
        JsonValue value = parseValue();
        result.arrayValue.push_back(value);

        skipWhitespace();
        char next = advance();

        if(next == ']') break;
        if(next != ',')
        {
            throw std::runtime_error("JsonValue: expected ',' or ']' in array");
        }

        skipWhitespace();
    }

    return result;
}

JsonValue JsonValue::Parser::parseString()
{
    JsonValue result;
    result.type = Type::String;

    expect('"');

    std::string out;
    while(peek() != '"')
    {
        char c = advance();

        if(c == '\\')
        {
            char escaped = advance();
            switch(escaped)
            {
                case 'n': out += '\n'; break;
                case 't': out += '\t'; break;
                case 'r': out += '\r'; break;
                case '"': out += '"'; break;
                case '\\': out += '\\'; break;
                case '/': out += '/'; break;
                default: out += escaped; break;
            }
        }
        else
        {
            out += c;
        }
    }

    expect('"');

    result.stringValue = out;
    return result;
}

JsonValue JsonValue::Parser::parseNumber()
{
    size_t start = pos;

    if(peek() == '-') advance();

    while(pos < text.size() && (std::isdigit(static_cast<unsigned char>(text[pos])) || text[pos] == '.' || text[pos] == 'e' || text[pos] == 'E' || text[pos] == '+' || text[pos] == '-'))
    {
        pos++;
    }

    std::string numberStr = text.substr(start, pos - start);

    JsonValue result;
    result.type = Type::Number;
    result.numberValue = std::atof(numberStr.c_str());
    return result;
}

JsonValue JsonValue::Parser::parseLiteral()
{
    JsonValue result;

    if(text.compare(pos, 4, "true") == 0)
    {
        result.type = Type::Boolean;
        result.boolValue = true;
        pos += 4;
    }
    else if(text.compare(pos, 5, "false") == 0)
    {
        result.type = Type::Boolean;
        result.boolValue = false;
        pos += 5;
    }
    else if(text.compare(pos, 4, "null") == 0)
    {
        result.type = Type::Null;
        pos += 4;
    }
    else
    {
        throw std::runtime_error("JsonValue: invalid literal");
    }

    return result;
}

}
