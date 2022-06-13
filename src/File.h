/**
* File.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef FILE_H
#define FILE_H

#include <cstdint>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

/**
 * Provides convenient access to file contents.
 **/
class File
{
    public:
        File() = delete;

        /**
         * Provides access to the contents of the file identified by
         * a \p path.
         **/
        explicit File(const std::string& path) : m_path(path) {}

        ~File() { }

        /**
         * Converts file contents to a byte vector.
         *
         * @return A byte vector representation of the file's content.
         **/
        std::vector<uint8_t> to_bytes();

        /**
         * Converts file contents to a std::string.
         *
         * @return String representation of the file's content.
         **/
        std::string to_string();

    private:
        const std::string m_path;
};

#endif //FILE_H
