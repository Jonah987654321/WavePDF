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
    OBJT_INDIRECT,
    OBJT_NULL,
    OBJT_INVALID
};

class BaseObject {
    public:
        BaseObject(size_t start, size_t end) 
            : start(start), end(end) {};
        BaseObject(size_t start)
            : start(start) {};
        void setEnd(size_t end) { this->end = end; }
        virtual ObjectType getType() { return OBJT_INVALID; }
        size_t getStart() { return start; }
        size_t getEnd() { return end; }

    protected:
        size_t start;
        size_t end;
};
