
#ifndef FMU_PROXY_FILE_READER_HPP
#define FMU_PROXY_FILE_READER_HPP

#include <cstdio>
#include <string>

namespace {

    void write_data(std::string const &fileName, std::string const &data) {

        FILE *file = fopen(fileName.c_str(), "wb");
        size_t bytes_written = fwrite(data.c_str(), sizeof(unsigned char), data.size(), file);
        fclose(file);
    }

    void read_data(std::string const &fileName, std::string &data) {


        FILE *file = fopen(fileName.c_str(), "rb");
        if (file == NULL) return;
        fseek(file, 0, SEEK_END);
        long int size = ftell(file);
        fclose(file);
        file = fopen(fileName.c_str(), "rb");

        data.resize(size);
        size_t bytes_read = fread(data.data(), sizeof(unsigned char), size, file);
        fclose(file);

    }

}

#endif //FMU_PROXY_FILE_READER_HPP
