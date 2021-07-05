#include "luvoasi/core/types.h"

namespace Luvoasi {

uint32_t DataType::GetBytes(DataType::Value value) {
    switch (value) {
    case DataType::INT8: return 1;
    case DataType::UINT8: return 1;
    case DataType::INT16: return 2;
    case DataType::UINT16: return 2;
    case DataType::INT32: return 4;
    case DataType::UINT32: return 4;
    case DataType::INT64: return 8;
    case DataType::UINT64: return 8;

    case DataType::FLOAT16: return 2;
    case DataType::FLOAT32: return 4;
    case DataType::FLOAT64: return 8;

    case DataType::FIXED32: return 4;
    
    case DataType::INT_2_10_10_10_REV: return 4;
    case DataType::UINT_2_10_10_10_REV: return 4;

    default: return 0;
    }
}

}
