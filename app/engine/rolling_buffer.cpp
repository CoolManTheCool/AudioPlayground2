#include "rolling_buffer.hpp"
#include <string>

template<typename T>
RollingBuffer<T>::RollingBuffer(size_t size) : size(size), index(0), count(0) {
    buffer.resize(size);
}

template<typename T>
void RollingBuffer<T>::add(const T& value) {
    buffer[index] = value;
    index = (index + 1) % size;
    if (count < size) ++count;
}

template<typename T>
void RollingBuffer<T>::resize(size_t newSize) {
    if (newSize == size) return;  // No need to resize if the size is unchanged
    
    std::vector<T> newBuffer(newSize);
    size_t validCount = std::min(count, newSize);  // Preserve as many elements as possible from the old buffer

    // Copy elements from the old buffer into the new buffer, maintaining the circular order
    for (size_t i = 0; i < validCount; ++i) {
        size_t realIndex = (index + i) % size;
        newBuffer[i] = buffer[realIndex];
    }

    // Update the internal buffer, size, and reset the index to the correct position
    buffer = std::move(newBuffer);
    size = newSize;
    index = validCount % newSize;  // New index points to the next valid position
    count = validCount;  // Preserve the count of valid entries
}

template<typename T>
const std::vector<T>& RollingBuffer<T>::getBuffer() const {
    return buffer;
}

template<typename T>
size_t RollingBuffer<T>::getIndex() const {
    return index;
}

template<typename T>
size_t RollingBuffer<T>::getCount() const {
    return count;
}

template<typename T>
std::unique_ptr<T[]> RollingBuffer<T>::getOrderedArray() const {
    std::unique_ptr<T[]> ordered = std::make_unique<T[]>(count);
    for (size_t i = 0; i < count; ++i) {
        size_t realIndex = (index + i) % size;
        ordered[i] = buffer[realIndex];
    }
    return ordered;
}

template class RollingBuffer<double>;
template class RollingBuffer<float>;
template class RollingBuffer<bool>;
template class RollingBuffer<int>;
template class RollingBuffer<unsigned int>;
template class RollingBuffer<std::string>;