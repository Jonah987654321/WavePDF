#pragma once

#include <vector>
#include <string>
#include <optional>
#include <wx/string.h>

class Buffer {
    public:
        Buffer(wxString filePath);
        void setPosition(size_t pos);
        size_t getPosition();
        bool markerIsAtEnd();
        char readNext();
        bool isReady();
        size_t getSize();

        void setArbitraryStartByteOffset(size_t s);

        std::string readByteRange(size_t start, size_t end);
        std::string readOffsetRange(size_t start, std::optional<size_t> end = std::nullopt);

    private:
        wxString filePath;
        std::vector<char> data;
        size_t readingPos;
        bool ready = false;
        size_t arbitraryStartByteOffset;
};