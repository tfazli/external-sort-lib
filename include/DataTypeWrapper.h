#ifndef DATATYPEWRAPPER_H
#define DATATYPEWRAPPER_H

#include <fstream>

template <typename T>
class DataTypeWrapper {
public:
    DataTypeWrapper() = default;
    ~DataTypeWrapper() = default;
    DataTypeWrapper(const DataTypeWrapper &rhs) = default;
    DataTypeWrapper(DataTypeWrapper &&rhs) = default;
    DataTypeWrapper& operator=(const DataTypeWrapper &rhs) = default;
    DataTypeWrapper& operator=(DataTypeWrapper &&rhs) = default;

    friend std::istream& operator >>(std::istream &in, DataTypeWrapper &to) {
        return in.read(reinterpret_cast<char*>(&to.mData), sizeof(T));
    }
    friend std::ostream& operator <<(std::ostream &out, const DataTypeWrapper &from) {
        return out.write(reinterpret_cast<const char*>(&from.mData), sizeof(T));
    }
    friend bool operator <(const DataTypeWrapper &lhs, const DataTypeWrapper &rhs) {
        return lhs.mData < rhs.mData;
    }
private:
    T mData;
};

#endif // DATATYPEWRAPPER_H
