#pragma once

#include <vector>
#include <cstdint>

enum ObjectType {
    OBJT_BOOLEAN,
    OBJT_INTEGER,
    OBJT_REAL,
    OBJT_STRING_LITERAL,
    OBJT_STRING_HEXADECIMAL,
    OBJT_NAME,
    OBJT_ARRAY,
    OBJT_DICTIONARY,
    OBJT_STREAM,
    OBJT_INDIRECT
};

class BaseObject {
    public:
        BaseObject(ObjectType type, size_t start, size_t end) 
            : type(type), start(start), end(end) {};
        BaseObject(ObjectType type, size_t start)
            : type(type), start(start) {};
        void setEnd(size_t end) { this->end = end; }
        ObjectType getType() { return type; }
        size_t getStart() { return start; }
        size_t getEnd() { return end; }

    protected:
        ObjectType type;
        size_t start;
        size_t end;
};
