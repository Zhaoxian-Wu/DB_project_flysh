#pragma once
#include <cstring>

template<typename T>
class Array {
public:
    Array(Array& other) {
        max_size = other.max_size;
        _buffer = new T[other.max_size];
        memcpy(_buffer, other._buffer, max_size * sizeof(T));
    }
	Array(size_t size) {
		_buffer = new T[size];
		max_size = size;
	}
	Array(size_t size, T* buffer) {
		_buffer = new T[size];
		memcpy(_buffer, buffer, size * sizeof(T));
		max_size = size;
	}
	~Array() {
		delete[] _buffer;
	}
	T& operator[](size_t index) {
		assert(index >= 0 && index < max_size);
		return _buffer[index];
	}
private:
	size_t max_size;
	T * _buffer;
};