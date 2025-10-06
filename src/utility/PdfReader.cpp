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

void PdfReader::setError(const std::string& msg, const std::optional<std::string>& log) {
    this->errorMessage = msg;
    this->error = true;

    std::string logMessage = log ? log.value() : msg;

    wxLogError(wxString(logMessage));
}

std::string PdfReader::getErrorMessage() {
    return this->errorMessage;
}

bool PdfReader::writeToBuffer() {
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

std::string PdfReader::readByteRangeFromBuffer(size_t start, size_t end) {
    // Validate byte range
    if (start >= this->buffer.size() || end > this->buffer.size() || start >= end) {
        throw std::runtime_error("Invalid byte range");
    }

    std::string extracted(this->buffer.begin() + start, this->buffer.begin() + end + 1);

    return extracted;
}

bool PdfReader::process() {
    if (!this->writeToBuffer()) {
        return false;
    }

    // Debug output for testing purposes
    //std::cout << this->readByteRangeFromBuffer(0, this->buffer.size());
    //std::cout << (int)this->buffer[this->buffer.size()-2] << "\n";

    // Check if file correctly ends with EOF
    size_t startEOFRead = this->buffer.size()-20, endEOFRead = this->buffer.size();
    std::string eof = this->readByteRangeFromBuffer(startEOFRead, endEOFRead);
    int eofStartPos = eof.find("%%EOF");
    if (eofStartPos == std::string::npos) {
        // No %%EOF in bytes read
        this->setError("Can't read file", "File missing %%EOF");
        return false;
    }

    // Locate startxref
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

    std::cout << this->xRefOffset << std::endl;



    return true;
}
