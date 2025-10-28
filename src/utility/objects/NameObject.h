#pragma once

#include "BaseObject.h"
#include <string>
#include <functional>

class NameObject: public BaseObject {
    public:
        explicit NameObject(size_t start, size_t end, const std::string& name) : BaseObject(start, end), value(name) {};
        ObjectType getType() override { return OBJT_NAME; }
        const std::string getValue() const { return value; }

        bool operator==(const NameObject& other) const {
            return value == other.value;
        }

    private:
        const std::string value;
};

// Prove hash function for NameObject -> allow usage in Hashtables
namespace std {
    template <>
    struct hash<NameObject> {
        std::size_t operator()(const NameObject& n) const noexcept {
            return std::hash<std::string>{}(n.getValue());
        }
    };
}
