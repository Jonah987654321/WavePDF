#pragma once

#include "BaseObject.h"

class BooleanObject: public BaseObject {
    public:
        bool getValue() const { return value; }

    private:
        bool value;
};
