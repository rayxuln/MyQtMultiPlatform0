
#include "RixJson.h"

#include <iostream>
#include <sstream>


// #define ENABLE_DEBUG_MESSAGE_MATCHING
// #define ENABLE_DEBUG_MESSAGE_FOUND

#ifdef ENABLE_DEBUG_MESSAGE_MATCHING
#define DEBUG_MESSAGE_MATCHING(x) {x}
#else
#define DEBUG_MESSAGE_MATCHING(x) {}
#endif

#ifdef ENABLE_DEBUG_MESSAGE_FOUND
#define DEBUG_MESSAGE_FOUND(x) {x}
#else
#define DEBUG_MESSAGE_FOUND(x) {}
#endif

using namespace std;
using namespace Rix::Json;

Object::Object():type(ValueType::UNKOWNED){}
Object::~Object(){}

string Object::GetKey()
{
    return key;
}
string Object::GetRawValue()
{
    return value;
}
ValueType Object::GetType()
{
    return type;
}
Object &Object::GetChild(SizeType index)
{
    return children[index];
}
vector<Object> &Object::GetChildren()
{
    return children;
}

void Object::SetKey(string s)
{
    key = s;
}
void Object::SetRawValue(string s)
{
    value = s;
}
void Object::SetType(ValueType t)
{
    type = t;
}
void Object::SetChild(SizeType index, Object &o)
{
    children[index] = o;
}
void Object::AddChild(Object &o)
{
    children.push_back(o);
}

void _ObjectToStr(stringstream &ss, Object &o, SizeType index, Object *po, SizeType d, bool b)
{
    for(SizeType i=0;b && i<d;++i)ss<<'\t';
    if(!o.GetKey().empty() && (po == nullptr || po->GetType() != ValueType::ARRAY))
    {
        ss<<o.GetKey();
        if(b) ss<<' ';
        ss<<":";
        if(b) ss<<' ';
    }else if(po && po->GetType() == ValueType::OBJECT)
    {
        ss<<'\"'<<index<<'\"';
        if(b) ss<<' ';
        ss<<":";
        if(b) ss<<' ';
    }
    switch (o.GetType())
    {
    case ValueType::REAL :
    case ValueType::STRING :
    case ValueType::BOOLEAN :
        ss<<o.GetRawValue();
        break;
    case ValueType::ARRAY :
        {
            ss<<'[';
            if(b) ss<<endl;
            auto &cs = o.GetChildren();
            for(SizeType i=0; i < cs.size(); ++i)
            {
                _ObjectToStr(ss, cs[i], i, &o, d+1, b);
                if(i < cs.size()-1) ss<<',';
                if(b) ss<<endl;
            }
            for(SizeType i=0;b && i<d;++i)ss<<'\t';
            ss<<']';
            break;
        }
    case ValueType::OBJECT :
        {
            ss<<'{';
            if(b) ss<<endl;
            auto &cs = o.GetChildren();
            for(SizeType i=0; i < cs.size(); ++i)
            {
                _ObjectToStr(ss, cs[i], i, &o, d+1, b);
                if(i < cs.size()-1) ss<<',';
                if(b) ss<<endl;
            }
            for(SizeType i=0;b && i<d;++i)ss<<'\t';
            ss<<'}';
            break;
        }
    default:
        ss<<"<Unkoned Json Type>";
        break;
    }
}
string Object::ToStr(bool beaty)
{
    stringstream ss;

    _ObjectToStr(ss, *this, -1, nullptr, 0, beaty);

    return ss.str();
}

Object &Object::operator=(const Object &o)
{
    key = o.key;
    value = o.value;
    type = o.type;
    children.assign(o.children.begin(), o.children.end());
    return *this;
}

const char *ParserException::what() const throw()
{
    return (string("\nParsing error: \n") + errorMsg).c_str();
}
ParserException::ParserException(std::string _s):errorMsg(_s){}
ParserException::~ParserException(){}

/*
    Compile Json Doc

    json_object -> { json_key_and_value_s }
    json_key_and_value_s -> json_key_and_value | json_key_and_value_s , json_key_and_value
    json_key_and_value -> json_key : json_value

    json_array -> [ json_value_s ]
    json_value_s -> json_value | json_value_s , json_value

    json_key -> string

    json_value -> string | real | boolean | json_array | json_object

    boolean -> true | false

    string -> '"' .* '"' | ''' .* '''

    real -> digits | digits . digits
    digits -> digit | digits digit
    digit -> [0-9]

    blanks -> blank | blanks blank
    blank -> ' ' | '\n' | '\r' | '\t'
*/
void Parser::_ParsingError(string reason)
{
    stringstream error;
    error<<reason<<endl;
    error<<"It happened at line: "<<line+1<<" column:"<<column+1<<endl;
    for(SizeType i=0,cnt=0;i<jsonDoc.size();++i)
    {
        if(cnt < line)
        {
            if(jsonDoc[i] == '\n') cnt += 1;
        }else
        {
            if(jsonDoc[i] != '\n')
            {
                error<<jsonDoc[i];
            }else break;
        }
    }
    error<<endl;
    for(SizeType i=0;i < column;++i) error<<' ';
    error<<'^'<<endl;
    throw(ParserException(error.str()));
}

#define NEXT jsonDoc[next]


void Parser::_JsonObject(Object &p)
{
    _Match('{');
    _JsonKeyAndValueS(p);
    _Match('}');
}
void Parser::_JsonKeyAndValueS(Object &p)
{
    do
    {
        Object q;
        _JsonKeyAndValue(q);
        p.AddChild(q);
    } while (NEXT == ',' && _Match(','));
}
void Parser::_JsonKeyAndValue(Object &p)
{
    _JsonKey(p);
    _Match(':');
    _JsonValue(p);
}

void Parser::_JsonArray(Object &p)
{
    _Match('[');
    _JsonValueS(p);
    _Match(']');
}
void Parser::_JsonValueS(Object &p)
{
    do
    {
        Object q;
        _JsonValue(q);
        p.AddChild(q);
    }while(NEXT == ',' && _Match(','));
}

void Parser::_JsonKey(Object &p)
{
    _Blanks();

    auto l = next;

    _String();

    auto r = next;
    auto s = jsonDoc.substr(l, r-l);
    DEBUG_MESSAGE_FOUND(cout<<"> Found a key: "<<s<<endl;);

    p.SetKey(s);

    _Blanks();
}
void Parser::_JsonValue(Object &p)
{
    _Blanks();
    auto l = next;
    if(NEXT == '\'' || NEXT == '\"')
    {
        _String();

        auto r = next;
        auto s = jsonDoc.substr(l, r-l);
        DEBUG_MESSAGE_FOUND(cout<<"> Found a value: "<<s<<" [string]"<<endl;);

        p.SetRawValue(s);
        p.SetType(ValueType::STRING);
    }else
    if(_IsDigit(NEXT))
    {
        _Real();

        auto r = next;
        auto s = jsonDoc.substr(l, r-l);
        DEBUG_MESSAGE_FOUND(cout<<"> Found a value: "<<s<<" [real]"<<endl;);

        p.SetRawValue(s);
        p.SetType(ValueType::REAL);
    }
    else
    if(NEXT == '{')
    {
        _JsonObject(p);

        auto r = next;
        auto s = jsonDoc.substr(l, r-l);
        DEBUG_MESSAGE_FOUND(cout<<"> Found a value: "<<s<<" [object]"<<endl;);

        p.SetType(ValueType::OBJECT);
    }else
    if(NEXT == '[')
    {
        _JsonArray(p);

        auto r = next;
        auto s = jsonDoc.substr(l, r-l);
        DEBUG_MESSAGE_FOUND(cout<<"> Found a value: "<<s<<" [array]"<<endl;);

        p.SetType(ValueType::ARRAY);
    }else
    if(NEXT == 'f' || NEXT == 't')
    {
        _Boolean();

        auto r = next;
        auto s = jsonDoc.substr(l, r-l);
        DEBUG_MESSAGE_FOUND(cout<<"> Found a value: "<<s<<" [boolean]"<<endl;);

        p.SetRawValue(s);
        p.SetType(ValueType::BOOLEAN);
    }
    else _ParsingError("Unkonwn value type!");
    _Blanks();
}

void Parser::_Boolean()
{
    if(NEXT == 'f')
    {
        _Match('f');
        _Match('a');
        _Match('l');
        _Match('s');
        _Match('e');
    }else
    {
        _Match('t');
        _Match('r');
        _Match('u');
        _Match('e');
    }
}

void Parser::_String()
{
    if(NEXT == '\"' && _Match('\"'))
    {
        while(NEXT != '\"' || (next > 0 && jsonDoc[next-1] == '\\'))_Match(NEXT);
        _Match('\"');
    }else
    {
        _Match('\'');
        while(NEXT != '\'' || (next > 0 && jsonDoc[next-1] == '\\'))_Match(NEXT);
        _Match('\'');
    }
    
}

void Parser::_Real()
{
    _Digits();
    if(NEXT == '.' && _Match('.')) _Digits();
}
void Parser::_Digits()
{
    do
    {
        _Match(NEXT);
    }while(_IsDigit(NEXT));
}
bool Parser::_IsDigit(char c)
{
    return c >= '0' && c <= '9';
}

void Parser::_Blanks()
{
    while(_IsBlank(NEXT))
    {
        _Match(NEXT);
    }
}
bool Parser::_IsBlank(char c)
{
    static char blanks[] = {' ', '\n', '\r', '\t'};
    for(int i=0;i < sizeof(blanks) / sizeof(char); ++i)
    {
        if(blanks[i] == c) return true;
    }
    return false;
}
bool Parser::_Match(char c)
{
    if(next >= jsonDoc.size())
    {
        throw(ParserException("Meet the end of this json doc!"));
    }
    DEBUG_MESSAGE_MATCHING(cout<<">- Matching: "<<NEXT<<" with "<<c;);
    if(jsonDoc[next] == c)
    {
        DEBUG_MESSAGE_MATCHING(cout<<"  v"<<endl;);
        column += 1;
        if(NEXT == '\n')
        {
            line += 1;
            column = 0;
        }
        next += 1;
        return true;
    }
    DEBUG_MESSAGE_MATCHING(cout<<"  x"<<endl;);
    //print error messages.
    _ParsingError(string("Expecting a \'") + c + '\'');
    
    return false;
}

Parser::Parser():next(0),line(0),column(0){}
Parser::Parser(string _jsonDoc):next(0),jsonDoc(_jsonDoc),line(0),column(0)
{

}
Parser::~Parser()
{
    
}

Object Parser::Parse()
{
    next = 0;

    line = 0;
    column = 0;

    Object jObject;

    _Blanks();
    _JsonValue(jObject);

    return jObject;
}