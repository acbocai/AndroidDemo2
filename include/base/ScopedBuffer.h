//
// Created by liuzhuangzhuang.das on 2021/11/05.
//

#ifndef SCOPED_BUFFER_H_INCLUDE
#define SCOPED_BUFFER_H_INCLUDE

#if !defined(DISALLOW_COPY_AND_ASSIGN)
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&) = delete;  \
    void operator=(const TypeName&) = delete
#endif

template <typename T, size_t S, bool alloc_default = true>
class ScopedBuffer {
 public:
  ScopedBuffer() {
    if (alloc_default) {
      buf_ = new T[S]();
    } else {
      buf_ = nullptr;
    }
  }

  ~ScopedBuffer() {
    if (buf_) {
      delete[] buf_;
    }
  }

  void alloc() {
    if (buf_ == nullptr) {
      buf_ = new T[S];
    }
  }

  T* buffer() const {
    return buf_;
  }

  size_t size() const {
    return S;
  }

  size_t sizeBytes() const {
    return S*sizeof(T);
  }

 private:
  T* buf_;

  DISALLOW_COPY_AND_ASSIGN(ScopedBuffer);
};

#endif  // SCOPED_BUFFER_H_INCLUDE
