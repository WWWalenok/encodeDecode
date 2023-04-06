#pragma once
#include <vector>



typedef std::vector<uint8_t> data_t;
#define __GEN_DATA(byte_size) data_t(byte_size, 0)
#define __TO_DATA(ret, data, i) ret[i] = data[i]
#define __FROM_DATA(ret, data, i) ret[i] = data[i]
//ret[i] =  __char2int[data[i * 2]] * 0x10 + __char2int[data[i * 2 + 1]] * 0x1; 
