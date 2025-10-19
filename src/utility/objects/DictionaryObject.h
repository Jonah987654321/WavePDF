#pragma once

#include "BaseObject.h"
#include <unordered_map>

class DictionaryObject: public BaseObject {
    public:
        bool getData() const { return data; }

    private:
        std::unordered_map< keys;
};
