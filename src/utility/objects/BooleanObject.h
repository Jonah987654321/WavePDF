#pragma once

#include "BaseObject.h"

class BooleanObject: public BaseObject {
    public:
        ObjectType getType() override { return OBJT_BOOLEAN; }
        bool getValue() const { return value; }

    private:
        bool value;
};
