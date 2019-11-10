#ifndef _H_LIB_RIXJSON_
#define _H_LIB_RIXJSON_

#include <string>
#include <locale>
#include <utility>
#include <codecvt>
#include <exception>
#include <vector>

namespace Rix
{

    namespace Json
    {
        typedef long long SizeType;
        enum ValueType
        {
            STRING,
            REAL,
            BOOLEAN,
            OBJECT,
            ARRAY,
            UNKOWNED
        };
        class Object
        {
            std::string key;
            std::string value;

            ValueType type;

            std::vector<Object> children;

            public:

            std::string GetKey();
            std::string GetRawValue();
            ValueType GetType();
            Object &GetChild(SizeType index);
            std::vector<Object> &GetChildren();

            void SetKey(std::string s);
            void SetRawValue(std::string s);
            void SetType(ValueType t);
            void SetChild(SizeType index, Object &o);
            void AddChild(Object &o);

            std::string ToStr(bool beaty = true);

            Object &operator= (Object &o);
            
            Object();
            ~Object();
        };

        class ParserException : public std::exception
        {
            std::string errorMsg;
            public:
            virtual const char *what() const throw();

            ParserException(std::string _s);
            ~ParserException();
        };
        class Parser
        {
            std::string jsonDoc;
            SizeType next;
            SizeType line, column;

            Parser();
            void _ParsingError(std::string reason);
            private://Bottom to top
            void _JsonObject(Object &p);
            void _JsonKeyAndValueS(Object &p);
            void _JsonKeyAndValue(Object &p);

            void _JsonArray(Object &p);
            void _JsonValueS(Object &p);

            void _JsonKey(Object &p);
            void _JsonValue(Object &p);

            void _Boolean();

            void _String();

            void _Real();
            void _Digits();
            bool _IsDigit(char c);

            void _Blanks();
            bool _IsBlank(char c);

            bool _Match(char c);

            public:
            Object Parse();

            Parser(std::string _jsonDoc);
            ~Parser();
        };
    }

}


#endif