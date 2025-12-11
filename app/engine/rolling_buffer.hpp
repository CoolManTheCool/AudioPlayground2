#ifndef ROLLING_BUFFER_HPP
#define ROLLING_BUFFER_HPP
#include <vector>
#include <memory>
#include <string>

template<typename T>
class RollingBuffer {
public:
    RollingBuffer(size_t size);

    void add(const T& value);
    void resize(size_t newSize);
    
    const std::vector<T>& getBuffer() const;
    size_t getIndex() const;
    size_t getCount() const;
    std::unique_ptr<T[]> getOrderedArray() const;

private:
    size_t size;
    size_t index;
    size_t count;
    std::vector<T> buffer;
};

#endif // ROLLING_BUFFER_HPP