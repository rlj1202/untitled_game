#ifndef H_LUVOASI_TYPES
#define H_LUVOASI_TYPES

#include "luvoasi/core/base.h"

namespace Luvoasi {

class DataType {
public:
    enum Value {
        INT8,   // byte
        UINT8,  // unsigned byte
        INT16,  // short
        UINT16, // unsigned short
        INT32,  // int
        UINT32, // unsigned int
        INT64,  // long long
        UINT64, // unsigned long long

        FLOAT16, // half float
        FLOAT32, // single float (or just float)
        FLOAT64, // double float

        FIXED32, // 16.16 32bit integer

        INT_2_10_10_10_REV,  // 32 bit integer which have 4 components
        UINT_2_10_10_10_REV, // 32 bit unsigned integer which have 4 components
    };

    static uint32_t GetBytes(Value value);

private:
    DataType();
};

}

#endif