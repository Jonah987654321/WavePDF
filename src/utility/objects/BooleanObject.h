#include "BaseObject.h";

class BooleanObject: BaseObject {
    public:
        bool getData() const { return data; }

    private:
        bool data;
};
