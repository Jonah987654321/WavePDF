#include "Buffer.h"

#include <wx/wfstream.h>
#include <wx/log.h>
#include <wx/string.h>


Buffer::Buffer(wxString filePath) {
    this->filePath = filePath;

    wxFileInputStream input_stream(this->filePath);
    if (!input_stream.IsOk()) {
        //throw std::runtime_error("Error reading file "+this->filePath)
        return;
    }

    size_t size = input_stream.GetLength(); // Get file byte size
    this->data.resize(size);

    input_stream.Read(this->data.data(), size);

    if (!input_stream) {
        //throw std::runtime_error("Error reading file "+this->filePath)
        return;
    }
    this->ready = true;
}

void Buffer::setPosition(size_t pos) {
    if (pos >= this->data.size() || pos < 0) {
        throw std::runtime_error("Invalid marker position for buffer size");
    }
    this->readingPos = pos;
}

size_t Buffer::getPosition() {
    return this->readingPos;
}

bool Buffer::markerIsAtEnd() {
    return this->readingPos == this->data.size();
}

char Buffer::readNext() {
    if (this->markerIsAtEnd()) {
        throw std::runtime_error("Attempt to read after buffer end");
    }
    char read = this->data.at(this->readingPos);
    // Increment markerPos so we can read the next char next time
    this->readingPos++;
    return read;
}

bool Buffer::isReady() {
    return this->ready;
}

size_t Buffer::getSize() {
    return this->data.size();
}

void Buffer::skipToNextContent() {
    char current = this->readNext();
    while (!this->markerIsAtEnd() && (current == ' ' || current == '\n' || current == '\r')) {
        current = this->readNext();
    }
    this->backOne();
}

void Buffer::backOne() {
    this->setPosition(this->getPosition()-1);
    if (this->getPosition() < 0) {
        this->setPosition(0);
    }
}

void Buffer::setArbitraryStartByteOffset(size_t s) {
    this->arbitraryStartByteOffset = s;
}

// Function to read from the buffer at a given byte range
std::string Buffer::readByteRange(size_t start, size_t end) {
    // Validate byte range
    if (start >= this->data.size() || end > this->data.size() || start >= end) {
        throw std::runtime_error("Invalid byte range");
    }

    std::string extracted(this->data.begin() + start, this->data.begin() + end + 1);
    return extracted;
}

// Function to read from the buffer based on offset + respecting the arbitrary start bytes
std::string Buffer::readOffsetRange(size_t start, std::optional<size_t> end) {
    size_t startByte = start + this->arbitraryStartByteOffset;
    size_t endByte = end.has_value() ? end.value() + this->arbitraryStartByteOffset : this->data.size();
    return this->readByteRange(startByte, endByte);
}
