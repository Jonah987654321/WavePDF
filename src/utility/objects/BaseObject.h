#pragma once

#include <vector>
#include <cstdint>

enum ObjectType {
    BOOLEAN,
    INTEGER,
    REAL,
    STRING_LITERAL,
    STRING_HEXADECIMAL,
    NAME,
    ARRAY,
    DICTIONARY,
    STREAM,
    INDIRECT
};

class BaseObject {
    public:

    protected:
        size_t number;
        uint16_t generation;
        ObjectType type;
};
