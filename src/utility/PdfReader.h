#ifndef UTILITY_PDFREADER_H
#define UTILITY_PDFREADER_H

#include <vector>
#include <string>
#include <optional>
#include <wx/string.h>
#include <cstdint>


struct xrefEntry {
    size_t entryOne;
    uint16_t generation;
    size_t number;
    char type;
};

struct xrefSubsection {
    size_t startObject = -1;
    size_t amountObjects = -1;
    std::vector<xrefEntry> objects;
};

class PdfReader {
    public:
        PdfReader(const wxString& filePath);
        bool process();

        // Getter methods
        std::string getErrorMessage() { return errorMessage; }
        std::string getLog() { return log; }
        std::size_t getXRefOffset() {return xRefOffset; }
        std::vector<xrefSubsection> getXRefTable() { return xrefTable; }
    private:
        // Helper methods:
        void setError(const std::string& msg, const std::optional<std::string>& log = std::nullopt);
        std::string readByteRangeFromBuffer(size_t start, size_t end);
        std::string readOffsetRangeFromBuffer(size_t start, std::optional<size_t> end = std::nullopt);
        size_t getNextContentPos(const std::string& read, size_t start);
        std::vector<std::string> split(const std::string& text, char delimiter);
        bool canConvertToSizeT(const std::string& s);

        // Methods used for PdfReader::process()
        bool writeToBuffer();
        bool readFileHeader();
        bool validateEOF();
        bool parseXRefOffset();
        bool parseXRefTable();

        // General attributes:
        wxString filePath;
        std::vector<char> buffer;

        // Data parsed from PDF
        size_t arbitraryStartByteOffset;
        std::string pdfVersion;
        bool pdfIsBinary;
        size_t xRefOffset = std::string::npos;
        std::vector<xrefSubsection> xrefTable;

        // For error handling
        std::string errorMessage;
        std::string log;
        bool error = false;
};

#endif