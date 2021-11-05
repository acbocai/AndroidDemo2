//
// Created by admin on 11/5/21.
//

#ifndef DEMO2_UNIQUE_FD_H
#define DEMO2_UNIQUE_FD_H
class unique_fd final {
public:
    explicit unique_fd(int fd) : fd_(fd) {};
    int get() const { return fd_; }
    ~unique_fd() { if (fd_ > 0) close(fd_); }
private:
    int fd_;
};
#endif //DEMO2_UNIQUE_FD_H
