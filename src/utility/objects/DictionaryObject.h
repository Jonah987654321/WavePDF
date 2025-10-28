#pragma once

#include "BaseObject.h"
#include <unordered_map>
#include <memory>

class DictionaryObject: public BaseObject {
    public:
        explicit DictionaryObject(size_t start) : BaseObject(start) {};
        ObjectType getType() override { return OBJT_DICTIONARY; }
        void addElement(std::shared_ptr<NameObject> name, std::shared_ptr<BaseObject> obj) { objects[name] = obj; }  

    private:
        std::unordered_map<std::shared_ptr<NameObject>, std::shared_ptr<BaseObject>> objects;
};
