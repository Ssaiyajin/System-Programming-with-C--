#ifndef H_lib_TempFile
#define H_lib_TempFile
//---------------------------------------------------------------------------
#include <string>
namespace raii {
//---------------------------------------------------------------------------
 class TempFile {
    public:
        TempFile(); // Default constructor
        TempFile(const std::string& fileName, const std::string& directoryPath); // Constructor
        ~TempFile(); // Destructor
        std::string getFilePath() const; // Function to get the file path of the temporary file
        // Prevent copy operation but allow move
        TempFile(const TempFile&) = delete;
        TempFile& operator=(const TempFile&) = delete;
        TempFile(TempFile&&) noexcept;
        TempFile& operator=(TempFile&&) noexcept;

        // Add functions for managing the file

    private:
        std::string filePath; // Member variable to store file path
        int fileDescriptor; // File descriptor associated with the file
        // Other members as needed
    };
//---------------------------------------------------------------------------
} // namespace raii
//---------------------------------------------------------------------------
#endif
