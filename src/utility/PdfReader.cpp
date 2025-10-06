#include "PdfReader.h"

#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <wx/wfstream.h>
#include <wx/log.h>
#include <wx/string.h>

// Constructor, save filepath as attribute
PdfReader::PdfReader(const wxString& filePath): filePath(filePath) {}

// Set error message & log to wxLog
void PdfReader::setError(const std::string& msg, const std::optional<std::string>& log) {
    this->errorMessage = msg;
    this->error = true;

    std::string logMessage = log ? log.value() : msg;

    wxLogError(wxString(logMessage));
}

// Getter for retrieving the error msg
std::string PdfReader::getErrorMessage() {
    return this->errorMessage;
}

// Helper function to read from the buffer at a given byte range
std::string PdfReader::readByteRangeFromBuffer(size_t start, size_t end) {
    // Validate byte range
    if (start >= this->buffer.size() || end > this->buffer.size() || start >= end) {
        throw std::runtime_error("Invalid byte range");
    }

    std::string extracted(this->buffer.begin() + start, this->buffer.begin() + end + 1);

    return extracted;
}

// Helper function to read from the buffer based on offset
std::string PdfReader::readOffsetRangeFromBuffer(size_t start, size_t end) {
    size_t startByte = start + this->arbitraryStartByteOffset;
    size_t endByte = end + this->arbitraryStartByteOffset;
    return this->readByteRangeFromBuffer(startByte, endByte);
}

// ********** START FUNCTIONS FOR PROCESS ********** 

// Function to open the input stream to given file path & write to buffer
bool PdfReader::writeToBuffer() {
    if (!this->buffer.empty()) return true; // buffer has already been written

    wxFileInputStream input_stream(this->filePath);
    if (!input_stream.IsOk()) {
        std::string logMsg = "Error on opening stream for " + this->filePath.ToStdString();
        this->setError("Can't open selected PDF!",
            logMsg);
        return false;
    }

    size_t size = input_stream.GetLength(); // Get file byte size
    this->buffer.resize(size);

    input_stream.Read(this->buffer.data(), size);

    if (!input_stream) {
        this->setError("Error reading file!");
        return false;
    }

    return true;
}

// Function to parse the PDF version & wether its binary or not
bool PdfReader::readFileHeader() {
    if (this->buffer.empty()) throw std::logic_error("PdfReader::readFileHeader() called before buffer was loaded");

    /* Find start of file (%PDF-) as we need to skip potential 
        preceding arbitrary bytes based on ISO32000 7.5.2 note 1 */
    bool entryFound = false;
    size_t startVersion = 0, endVersion = 4;
    while (!entryFound) {
        if (this->readByteRangeFromBuffer(startVersion, endVersion) == "%PDF-") {
            entryFound = true;
        } else if (endVersion > 1024) {
            // No file header found in first 1024 bytes
            this->setError("Invalid PDF Format", "No %PDF- found in first 1024 bytes");
            return false;
        } else {
            startVersion++;
            endVersion++;
        }
    }

    // Set arbitraryStartByteOffset to be added to all offsets as all offsets are calculated from the starting % 
    this->arbitraryStartByteOffset = startVersion;

    // Read the version number:
    this->pdfVersion = this->readByteRangeFromBuffer(endVersion+1, endVersion+3);

    // Check if there is a command following including atleast 4 binary bits
    size_t binaryCheckStart = endVersion+4;
    size_t binaryCheckEnd = binaryCheckStart+50;
    std::string binaryCheck = this->readByteRangeFromBuffer(binaryCheckStart, binaryCheckEnd);
    size_t commentStart = 0;
    while (commentStart < binaryCheck.size() && (binaryCheck[commentStart] == ' ' || binaryCheck[commentStart] == '\n' || binaryCheck[commentStart] == '\r')) {
        commentStart++; // skip whitespace and newlines
    }
    if (commentStart == binaryCheck.size() || binaryCheck[commentStart] != '%'){
        this->pdfIsBinary = false;
    } else {
        // Loop until next new line & count binary bytes
        int binaryCount = 0;
        while (commentStart < binaryCheck.size() && binaryCheck[commentStart] != '\n' && binaryCheck[commentStart] != '\r') {
            if ((unsigned char)binaryCheck[commentStart] >= 128) {
                binaryCount++;
            }
            commentStart++;
        }
        this->pdfIsBinary = binaryCount >= 4;
    }

    return true;
}

// Function to check if file correctly ends with EOF
bool PdfReader::validateEOF() {
    if (this->buffer.empty()) throw std::logic_error("PdfReader::validateEOF() called before buffer was loaded");

    size_t startEOFRead = this->buffer.size()-20, endEOFRead = this->buffer.size();
    std::string eof = this->readByteRangeFromBuffer(startEOFRead, endEOFRead);
    int eofStartPos = eof.find("%%EOF");
    if (eofStartPos == std::string::npos) {
        // No %%EOF in bytes read
        this->setError("Can't read file", "File missing %%EOF");
        return false;
    }
    return true;
}

// Function to locate & read startxref (byte offset for xref)
bool PdfReader::parseXRefOffset() {
    if (this->buffer.empty()) throw std::logic_error("PdfReader::parseXRefOffset() called before buffer was loaded");

    // Locating startxref in buffer
    size_t startXRefPosRead = this->buffer.size()-1024, endXRefPosRead = this->buffer.size();
    std::string xRefPosRead = this->readByteRangeFromBuffer(startXRefPosRead, endXRefPosRead);
    int startXrefPos = xRefPosRead.find("startxref\n");
    if (startXrefPos == std::string::npos) {
        // No startxref in bytes read
        this->setError("Can't read file", "File missing startxref");
        return false;
    }
    // Extract the number after startxref
    startXrefPos += 9; // skip the "startxref"
    while (startXrefPos < xRefPosRead.size() && (xRefPosRead[startXrefPos] == ' ' || xRefPosRead[startXrefPos] == '\n' || xRefPosRead[startXrefPos] == '\r')) {
        startXrefPos++; // skip whitespace and newlines
    }
    size_t endXRefPos = startXrefPos;
    while (endXRefPos < xRefPosRead.size() && isdigit(xRefPosRead[endXRefPos])) {
        // Select all following connected digits -> the xref offset
        endXRefPos++;
    }
    if (startXrefPos == endXRefPos) {
        this->setError("Can't read file", "startxref has no offset number");
        return false;
    }
    // Convert string to integer & write to attribute
    try {
        this->xRefOffset = std::stoul(xRefPosRead.substr(startXrefPos, endXRefPos - startXrefPos));
    } catch (...) {
        this->setError("Can't read file", "Invalid startxref number");
        return false;
    }
    return true;
}

// Main function to be called to process the file path
bool PdfReader::process() {
    if (!this->writeToBuffer()) return false;
    if (!this->readFileHeader()) return false;
    if (!this->validateEOF()) return false;
    if (!this->parseXRefOffset()) return false;
    
    return true;
}
