#pragma once

#include <atomic>
#include <iostream>
#include <cstring>
#include <vector>
#include <unistd.h>  // write
#include <sys/uio.h> //readv

namespace tempserver {

class Buffer {
   private:
    std::vector<char> buffer_;
    std::atomic<std::size_t> read_pos_;
    std::atomic<std::size_t> write_pos_;

    char* BeginPtr_();
    const char* BeginPtr_() const;
    void MakeSpace_(size_t len);

   public:
    Buffer(int initBuffSize = 1024)
        : buffer_(initBuffSize), read_pos_(0), write_pos_(0){};
    ~Buffer() = default;

    size_t WritableBytes() const;
    size_t ReadableBytes() const;
    size_t PrependableBytes() const;

    const char* Peek() const;
    void EnsureWriteable(size_t len);
    void HasWritten(size_t len);

    void Retrieve(size_t len);
    void RetrieveUntil(const char* end);
    void RetrieveAll() ;
    std::string RetrieveAllToStr();

    const char* BeginWriteConst() const;
    char* BeginWrite();

    void Append(const std::string& str);
    void Append(const char* str, size_t len);
    void Append(const void* data, size_t len);
    void Append(const Buffer& buff);

    ssize_t ReadFd(int fd, int* Errno);
    ssize_t WriteFd(int fd, int* Errno);
};

}  // namespace tempserver
