#include "PdfReader.h"

#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <sstream>
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

// Getter for retrieving the xref offset
std::size_t PdfReader::getXRefOffset() {
    return this->xRefOffset;
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
std::string PdfReader::readOffsetRangeFromBuffer(size_t start, std::optional<size_t> end) {
    size_t startByte = start + this->arbitraryStartByteOffset;
    size_t endByte = end.has_value() ? end.value() + this->arbitraryStartByteOffset : this->buffer.size();
    return this->readByteRangeFromBuffer(startByte, endByte);
}

// Function to get the byte position of the next not whitespace/new line
size_t PdfReader::getNextContentPos(const std::string& read, size_t start) {
    while (start < read.size() && (read[start] == ' ' || read[start] == '\n' || read[start] == '\r')) {
        start++;
    }
    return start;
}

// Helper function to split a string into substrings by given delimiter
std::vector<std::string> PdfReader::split(const std::string& text, char delimiter) {
    std::vector<std::string> result;
    std::istringstream iss(text);
    std::string token;

    while (std::getline(iss, token, delimiter)) {
        result.push_back(token);
    }

    return result;
}

// Helper function to check if a given string can be converted to a size_t
bool PdfReader::canConvertToSizeT(const std::string& s) {
    try {
        size_t pos;
        unsigned long long val = std::stoull(s, &pos);
        // Check if entire string was consumed
        return pos == s.size();
    } catch (const std::invalid_argument&) {
        // Not a number
        return false;
    } catch (const std::out_of_range&) {
        // Number too large for unsigned long long
        return false;
    }
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
    size_t commentStart = this->getNextContentPos(binaryCheck, 0); // Skip whitespace and newlines
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
    size_t startXrefPos = xRefPosRead.find("startxref\n");
    if (startXrefPos == std::string::npos) {
        // No startxref in bytes read
        this->setError("Can't read file", "File missing startxref");
        return false;
    }
    // Extract the number after startxref
    startXrefPos += 9; // skip the "startxref"
    startXrefPos = this->getNextContentPos(xRefPosRead, startXrefPos); // skip whitespace and newlines
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

// Function to parse the xref table from the file
bool PdfReader::parseXRefTable() {
    if (this->xRefOffset == std::string::npos) throw std::logic_error("PdfReader::parseXRefTable() called without parsed xref offset");

    // Read from xRefOffset to end
    std::string xRefRead = this->readOffsetRangeFromBuffer(this->xRefOffset);

    // Verify if xref is starting at parsed offset
    if (xRefRead.substr(0, 4) != "xref") {
        this->setError("Can't read file", "xref not found at parsed offset");
        return false;
    }

    size_t currentReadPos = this->getNextContentPos(xRefRead, 5);
    size_t currentReadEnd = currentReadPos;
    bool continueReading = true;
    xrefSubsection currentSubsection;
    while (continueReading) {
        // Read one line, push read end until a newline CR or LF is detected:
        while (xRefRead[currentReadEnd+1] != '\n' && xRefRead[currentReadEnd+1] != '\r') {
            currentReadEnd++;
        }
        currentReadEnd++; // To include the last character
        std::string line = xRefRead.substr(currentReadPos, currentReadEnd - currentReadPos);
        std::vector<std::string> lineData = this->split(line, ' ');

        bool isPartOfXref = false;

        // Check if we have a new subsection head
        if (lineData.size() == 2 && this->canConvertToSizeT(lineData[0]) && this->canConvertToSizeT(lineData[1])) {
            // Is there a previous finished subsection we can push to the table?
            if (!currentSubsection.objects.empty()) {
                if (currentSubsection.amountObjects == currentSubsection.objects.size()) {
                    this->xrefTable.push_back(currentSubsection);
                } else {
                    this->setError("Can't read file", "xref subsection object count does not match");
                    return false;
                }
            }

            // Check if the new subsection object number range collude with an existing subsection
            size_t startObject = static_cast<size_t>(std::stoull(lineData[0]));
            size_t amountObjects = static_cast<size_t>(std::stoull(lineData[1]));
            for (xrefSubsection sec: this->xrefTable) {
                /* Either:
                    - the right border of existing range needs to be smaller than left of new
                    - or the left border of existing needs to be bigger than right of new
                    to verify the ranges don't overlap
                */
                if (!(sec.startObject > startObject+amountObjects || sec.startObject+sec.amountObjects < startObject)) {
                    this->setError("Can't read file", "xref subsection have overlapping object numbers");
                    return false;
                }
            }

            currentSubsection = xrefSubsection{};
            currentSubsection.startObject = startObject;
            currentSubsection.amountObjects = amountObjects;

            isPartOfXref = true;
        }

        // Check if we have a new xref entry that matches all requirements
        if (lineData.size() == 3 && 
            this->canConvertToSizeT(lineData[0]) && lineData[0].size() == 10 &&
            this->canConvertToSizeT(lineData[1]) && lineData[1].size() == 5 &&
            (lineData[2] == "f" || lineData[3] == "n")) {

                if (currentSubsection.startObject == -1 || currentSubsection.amountObjects == -1) {
                    this->setError("Can't read file", "xref entry before subsection head");
                    return false;
                }

                // Create new entry
                xrefEntry entry;
                entry.entryOne =  static_cast<size_t>(std::stoull(lineData[0]));
                entry.generation = static_cast<size_t>(std::stoull(lineData[1]));
                entry.number = currentSubsection.startObject+currentSubsection.objects.size();
                entry.type = lineData[3][0]; // -> [0] to get as char

                // Add entry to current subsection
                currentSubsection.objects.push_back(entry);
        }
        
        // Was the line still a valid xref element?
        if (!isPartOfXref) {
            // Push a potential last subsection to xref table
            if (!currentSubsection.objects.empty()) {
                if (currentSubsection.amountObjects == currentSubsection.objects.size()) {
                    this->xrefTable.push_back(currentSubsection);
                } else {
                    this->setError("Can't read file", "xref subsection object count does not match");
                    return false;
                }
            }

            // xref table is finished
            continueReading = false;
        }
    }

    return true;
}

// Main function to be called to process the file path
bool PdfReader::process() {
    if (!this->writeToBuffer()) return false;
    if (!this->readFileHeader()) return false;
    if (!this->validateEOF()) return false;
    if (!this->parseXRefOffset()) return false;
    if (!this->parseXRefTable()) return false;
    
    return true;
}
