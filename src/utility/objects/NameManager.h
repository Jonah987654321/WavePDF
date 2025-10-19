#pragma once

#include "NameObject.h"
#include <vector>
#include <string>

class NameManger {
    public:
        bool createName(std::string);
        NameObject* getCreated();

    private:
        std::vector<NameObject> names;
};
