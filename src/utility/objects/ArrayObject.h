#pragma once

#include "BaseObject.h"
#include <vector>
#include <memory>

class ArrayObject: public BaseObject {
    public:
        explicit ArrayObject(size_t start) : BaseObject(start) {};
        void addObject(std::shared_ptr<BaseObject> obj) { objects.push_back(obj); } 
        ObjectType getType() override { return OBJT_ARRAY; }

    private:
        std::vector<std::shared_ptr<BaseObject>> objects;
};
