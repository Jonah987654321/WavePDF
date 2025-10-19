#pragma once

#include "BaseObject.h"
#include <string>

class NameObject: public BaseObject {
    public:
        explicit NameObject(size_t start, size_t end, const std::string& name) : BaseObject(OBJT_NAME, start, end), value(name) {};
        const std::string getValue() const { return value; }

    private:
        const std::string value;
};
