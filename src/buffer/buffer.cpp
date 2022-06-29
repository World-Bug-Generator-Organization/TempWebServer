#include "buffer.h"

#include <assert.h>

namespace tempserver {

size_t Buffer::ReadableBytes() const {
    // 可读字节数
    return write_pos_ - read_pos_;
}
size_t Buffer::WritableBytes() const {
    // 可写字节数
    return buffer_.size() - write_pos_;
}

size_t Buffer::PrependableBytes() const { return read_pos_; }

const char* Buffer::Peek() const {
    // 首个可读字节指针
    return BeginPtr_() + read_pos_;
}

void Buffer::EnsureWriteable(size_t len) {
    // 保证足够的写空间
    if (WritableBytes() < len) {
        MakeSpace_(len);
    }
    assert(WritableBytes() >= len);
}

void Buffer::HasWritten(size_t len) {
    // 移动写指针
    write_pos_ += len;
}

void Buffer::Retrieve(size_t len) {
    // 移动读指针指定长度
    assert(len <= ReadableBytes());
    read_pos_ += len;
}

void Buffer::RetrieveUntil(const char* end) {
    // 移动读指针至指定长度
    assert(Peek() <= end);
    Retrieve(end - Peek());
}

void Buffer::RetrieveAll() {
    // 清空buffer
    bzero(&buffer_[0], buffer_.size());
    read_pos_ = 0;
    write_pos_ = 0;
}

std::string Buffer::RetrieveAllToStr() {
    // 清空buffer并返回剩下未读内容
    std::string str(Peek(), ReadableBytes());
    RetrieveAll();
    return str;
}

// 不同风格的写入
void Buffer::Append(const std::string& str) {
    Append(str.data(), str.length());
}

void Buffer::Append(const void* data, size_t len) {
    assert(data);
    Append(static_cast<const char*>(data), len);
}

void Buffer::Append(const char* str, size_t len) {
    // 最基本的写入
    assert(str);
    EnsureWriteable(len);
    std::copy(str, str + len, BeginWrite());
    HasWritten(len);
}

void Buffer::Append(const Buffer& buff) {
    Append(buff.Peek(), buff.ReadableBytes());
}

ssize_t Buffer::ReadFd(int fd, int* saveErrno) {
    char buff[65535];
    struct iovec iov[2];
    const size_t writable = WritableBytes();
    /* 分散读， 保证数据全部读完 */
    iov[0].iov_base = BeginPtr_() + write_pos_;
    iov[0].iov_len = writable;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

    // 同时向上述申请的缓冲区和成员变量中写入
    // 成员变量空间不足时,在第三个case中扩展空间并追加剩下内容
    const ssize_t len = readv(fd, iov, 2);
    if (len < 0) {
        *saveErrno = errno;
    } else if (static_cast<size_t>(len) <= writable) {
        write_pos_ += len;
    } else {
        write_pos_ = buffer_.size();
        Append(buff, len - writable);
    }
    return len;
}

ssize_t Buffer::WriteFd(int fd, int* saveErrno) {
    size_t readSize = ReadableBytes();
    ssize_t len = write(fd, Peek(), readSize);
    if (len < 0) {
        *saveErrno = errno;
        return len;
    }
    read_pos_ += len;
    return len;
}

const char* Buffer::BeginWriteConst() const { return BeginPtr_() + write_pos_; }

char* Buffer::BeginWrite() { return BeginPtr_() + write_pos_; }

char* Buffer::BeginPtr_() { return &*buffer_.begin(); }

const char* Buffer::BeginPtr_() const { return &*buffer_.begin(); }

void Buffer::MakeSpace_(size_t len) {
    if (WritableBytes() + PrependableBytes() < len) {
        buffer_.resize(write_pos_ + len + 1);
    } else {
        size_t readable = ReadableBytes();
        std::copy(BeginPtr_() + read_pos_, BeginPtr_() + write_pos_,
                  BeginPtr_());
        read_pos_ = 0;
        write_pos_ = read_pos_ + readable;
        assert(readable == ReadableBytes());
    }
}
}  // namespace tempserver
