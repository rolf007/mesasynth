#ifndef _MANAGER_H_
#define _MANAGER_H_

template<typename BASE, unsigned MAX_SIZE>
class Manager {
public:
    Manager() : entries_(0), maxEntries_(4)
    {
        bases_ = new char[maxSize_*maxEntries_];
    }
    ~Manager()
    {
        for (unsigned i = (entries_ < maxEntries_) ? 0 : entries_ - maxEntries_; i < entries_; ++i)
            destroy(i);
        delete [] (char*)bases_;
    }
    template<typename T, typename... Args>
    unsigned mk(Args... args)
    {
        static_assert(sizeof(T) <= maxSize_);
        if (entries_ >= maxEntries_)
            destroy(entries_);
        new(bases_ + (entries_%maxEntries_)*maxSize_)T(args...);
        ++entries_;
        return entries_-1;
    }
    BASE* get(unsigned i)
    {
        if (i >= entries_ || i + maxEntries_ < entries_)
            return nullptr;
        return (BASE*)(bases_ + (i%maxEntries_)*maxSize_);
    }
private:
    void destroy(unsigned i)
    {
        ((BASE*)(bases_+(i%maxEntries_)*maxSize_))->~BASE();
    }
    unsigned entries_;
    char* bases_;
    static constexpr unsigned maxSize_ = MAX_SIZE;
    const unsigned maxEntries_;
};

#endif
