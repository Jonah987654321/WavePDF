#ifndef UTILITY_PDFREADER_H
#define UTILITY_PDFREADER_H

#include <vector>
#include <string>
#include <optional>
#include <wx/string.h>

class PdfReader {
    public:
        PdfReader(const wxString& filePath);
        bool process();

        // Getter & setter methods
        std::string getErrorMessage();
    private:
        // Helper methods:
        void setError(const std::string& msg, const std::optional<std::string>& log = std::nullopt);
        std::string readByteRangeFromBuffer(size_t start, size_t end);

        // Methods used for PdfReader::process()
        bool writeToBuffer();
        bool validateEOF();
        bool parseXRefOffset();

        // Attributes:
        wxString filePath;
        std::vector<char> buffer;
        size_t xRefOffset;

        // For error handling
        std::string errorMessage;
        bool error = false;
};

#endif