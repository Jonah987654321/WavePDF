#pragma once

#include "BaseObject.h"
#include <vector>

class ArrayObject: public BaseObject {
    public:
        explicit ArrayObject(size_t start) : BaseObject(OBJT_ARRAY, start) {};
        void addObject(BaseObject obj) { objects.push_back(obj); } 

    private:
        std::vector<BaseObject> objects;
};
