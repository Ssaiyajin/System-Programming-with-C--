#include "lib/TempFile.hpp"
#include <iostream> 
//---------------------------------------------------------------------------
namespace raii {
//---------------------------------------------------------------------------

    TempFile::TempFile(const std::string& fileName, const std::string& directoryPath) {
        filePath = directoryPath + fileName; // Logic to create file path
        std::cout << "Created temporary file: " << filePath << std::endl;
        // Other constructor logic if needed
    }

    TempFile::TempFile() {
        // Default constructor logic, if needed
    }

    TempFile::~TempFile() {
        std::cout << "Removing temporary file: " << filePath << std::endl;
        // Logic to remove file
    }

    std::string TempFile::getFilePath() const {
        return filePath;
    }
    
    // Move constructor
    TempFile::TempFile(TempFile&& other) noexcept 
        : filePath(std::move(other.filePath)) {
        // Move other resources if needed
    }

    // Move assignment operator
    TempFile& TempFile::operator=(TempFile&& other) noexcept {
        if (this != &other) {
            filePath = std::move(other.filePath);
            // Move other resources if needed
        }
        return *this;
    }
//---------------------------------------------------------------------------
} // namespace raii
//---------------------------------------------------------------------------
