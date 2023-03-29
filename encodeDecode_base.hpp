#pragma once
#include <cstdio>
#include <vector>
#include <unordered_map>
#include <functional>
#include <string>
#include <string.h>
#include <tuple>
#include <stdexcept>

/*
It implements a mechanism for creating serialization and deserialization functions based on templates and macros.
Primitive types and strings are supported.
std::vector<T> is supported only with explicit serialization and deserialization.
std::tuple is also supported and based on it, mechanisms for working with user structures are implemented.
Similar to std::vector<T>, std::tuple only support explicit conversions (at least for now).
WWWalenok.
*/

namespace kmip
{

#ifndef __ENCODER
#define __ENCODER(src, dest, byte_size) memcpy(dest, src, byte_size);
#endif

#ifndef __DECODER
#define __DECODER(src, dest, byte_size) memcpy(dest, src, byte_size);
#endif

typedef std::vector<uint8_t> data_t;


typedef size_t (*deserializer_t)(const data_t&, void*);
typedef size_t (*serializer_t)(data_t&,const void*);

static std::unordered_map<uint16_t, deserializer_t> __deserializer_list; 
static std::unordered_map<uint16_t,   serializer_t> __serializer_list;

static uint16_t gen_uid(const char* name)
{
		size_t hash = 0x811c9dc5;
    while (*name) {
        hash ^= (unsigned char) *name++;
        hash *= 0x01000193;
    }
	return hash & 0xFFFF;
}

static uint16_t __uid_insert_deserializer(const char* name, deserializer_t val) 
{
	uint16_t uid = gen_uid(name);
	__deserializer_list[uid] = val;
	return uid; 
}
static uint16_t __uid_insert_serializer(const char* name, serializer_t val) 
{
	uint16_t uid = gen_uid(name);
	__serializer_list[uid] = val;
	return uid; 
}



#define __GEN_DATA(byte_size) data_t(byte_size, 0)
#define __TO_DATA(ret, data, i) ret[i] = data[i]
#define __FROM_DATA(ret, data, i) ret[i] = data[i]
//ret[i] =  __char2int[data[i * 2]] * 0x10 + __char2int[data[i * 2 + 1]] * 0x1; 


const static uint8_t __char2int[256]
{
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};

// Serialize - Deserialize generator
template<class type> 
static size_t Serialize(data_t& ret, const type* var);

template<class type> 
static size_t Deserialize(const data_t& var, type* ret);


#define __GEN_SERIALIZE_B(type, tag) static size_t __serialize__##tag##__(data_t& ret, const void* var)
#define __GEN_SERIALIZE_E(type, tag) static size_t Serialize(data_t& ret, const type* var){ return __serialize__##tag##__(ret, var); } const uint16_t __uid__##tag##_s = __uid_insert_serializer(#tag, __serialize__##tag##__); static uint16_t GetUidByObject(type* val) { return __uid__##tag##_s; }

#define __GEN_SERIALIZE(type, tag) __GEN_SERIALIZE_B(type, tag); __GEN_SERIALIZE_E(type, tag) __GEN_SERIALIZE_B(type, tag)

#define __GEN_DESERIALIZE_B(type, tag) static size_t __deserialize__##tag##__(const data_t& var, void* ret)
#define __GEN_DESERIALIZE_E(type, tag) static size_t Deserialize(const data_t& var, type* ret){ return __deserialize__##tag##__(var, ret); }  const uint16_t __uid__##tag##_d = __uid_insert_deserializer(#tag, __deserialize__##tag##__);

#define __GEN_DESERIALIZE(type, tag) __GEN_DESERIALIZE_B(type, tag); __GEN_DESERIALIZE_E(type, tag) __GEN_DESERIALIZE_B(type, tag)

#define __GEN_BASE_SERIALIZE_BODY(_byte_size, tag) { \
	if(!var)throw std::invalid_argument("empty pointer"); \
	size_t byte_size = _byte_size; \
	uint8_t* reinterpreted = (uint8_t*)(var); \
	uint8_t* encoded = new uint8_t[byte_size];\
	__ENCODER(reinterpreted, encoded, byte_size); \
	ret = __GEN_DATA(byte_size + 2); \
	*(uint16_t*)(ret.data()) = __uid__##tag##_s; \
	size_t i = 0; \
	for (i = 0; i < byte_size; i++) __TO_DATA((ret.data() + 2), encoded, i); \
	delete[] encoded; \
}

#define __GEN_BASE_DESERIALIZE_BODY(_byte_size) { \
	if(!ret)throw std::invalid_argument("empty pointer"); \
	size_t byte_size = _byte_size; \
	if(_byte_size > var.size()) throw std::out_of_range("buffer too small for deserialization");\
	uint8_t* reinterpreted = (uint8_t*)(ret); \
	uint8_t* encoded = new uint8_t[byte_size]; \
	size_t i = 0; \
	for (i = 0; i < byte_size; i++) __FROM_DATA(encoded, (var.data() + 2), i); \
	__DECODER(encoded, reinterpreted, byte_size); \
	delete[] encoded; \
}

#define __GEN_BASE_SERIALIZE(type, tag) __GEN_SERIALIZE(type, tag) { __GEN_BASE_SERIALIZE_BODY(sizeof(type), tag) return sizeof(type) + 2; }

#define __GEN_BASE_DESERIALIZE(type, tag) __GEN_DESERIALIZE(type, tag) { __GEN_BASE_DESERIALIZE_BODY(sizeof(type)) return sizeof(type) + 2; }

#define __GEN_BASE_SERIALIZE_DESERIALIZE(type, tag) \
__GEN_BASE_SERIALIZE(type, type) \
__GEN_BASE_DESERIALIZE(type, type)

#define __GEN_BASE_SERIALIZE_DESERIALIZE_BASE(type) \
__GEN_BASE_SERIALIZE_DESERIALIZE(type, type)

__GEN_BASE_SERIALIZE_DESERIALIZE_BASE(int8_t)

__GEN_BASE_SERIALIZE_DESERIALIZE_BASE(uint8_t)

__GEN_BASE_SERIALIZE_DESERIALIZE_BASE(int16_t)

__GEN_BASE_SERIALIZE_DESERIALIZE_BASE(uint16_t)

__GEN_BASE_SERIALIZE_DESERIALIZE_BASE(int32_t)

__GEN_BASE_SERIALIZE_DESERIALIZE_BASE(uint32_t)

__GEN_BASE_SERIALIZE_DESERIALIZE_BASE(int64_t)

__GEN_BASE_SERIALIZE_DESERIALIZE_BASE(uint64_t)

__GEN_BASE_SERIALIZE_DESERIALIZE_BASE(float)

__GEN_BASE_SERIALIZE_DESERIALIZE_BASE(double)

// Advanced types

__GEN_SERIALIZE(std::string, std_string)
{
	if(!var)throw std::invalid_argument("empty pointer");
	std::string* data = (std::string*)var;
	size_t _byte_size = data->size() * sizeof(char);
	ret = __GEN_DATA(_byte_size + 4);
	*(uint32_t*)(ret.data()) = _byte_size;
	data_t tret;

	size_t byte_size = _byte_size; 
	uint8_t* reinterpreted = (uint8_t*)(data->data()); 
	uint8_t* encoded = new uint8_t[byte_size];
	__ENCODER(reinterpreted, encoded, byte_size); 
	tret = __GEN_DATA(byte_size); 
	size_t i = 0; 
	for (i = 0; i < byte_size; i++) __TO_DATA(tret, encoded, i); 
	delete[] encoded;

	memcpy(ret.data() + 4, tret.data(), _byte_size);

	return _byte_size + 4;
}

__GEN_DESERIALIZE(std::string, std_string)
{
	if(!ret)throw std::invalid_argument("empty pointer");
	std::string* data = (std::string*)ret;
	size_t _byte_size = *(uint32_t*)(var.data());
	size_t element_size = _byte_size;
	*data = std::string(element_size, '\0');
	size_t byte_size = _byte_size;

	if(_byte_size > var.size()) throw std::out_of_range("buffer too small for deserialization");
	uint8_t* reinterpreted = (uint8_t*)(data->data()); 
	uint8_t* encoded = new uint8_t[byte_size]; 
	size_t i = 0; 
	for (i = 0; i < byte_size; i++) __FROM_DATA(encoded, (&var[4]), i);
	__DECODER(encoded, reinterpreted, byte_size); 
	delete[] encoded; 
	return _byte_size + 4;
}

const uint16_t __uid__std__vector_s = __uid_insert_serializer("std__vector", 0);
template <typename T> size_t Serialize(data_t& ret, const std::vector<T>* var)
{
	if(!var)throw std::invalid_argument("empty pointer");
	std::vector<T>* data = (std::vector<T>*)var;
	size_t _byte_size = 0;
	ret.resize(6, 0);

	for (size_t i = 0; i < data->size(); i++) {
		data_t temp = {};
		const T* dt = data->data() + i;
		_byte_size = _byte_size + Serialize(temp, dt);
		ret.insert(ret.end(), temp.begin(), temp.end());
	}
	*(uint16_t*)(ret.data()) = __uid__std__vector_s;
	*(uint32_t*)(ret.data() + 2) = _byte_size;
	return _byte_size + 6;
}

static size_t Deerialize_bytearray(const data_t& var, void* ret);
const uint16_t __uid__std__vector_d = __uid_insert_deserializer("std__vector", Deerialize_bytearray);

static size_t Deerialize_bytearray(const data_t& var, void* ret){
	return 0;
}

template <typename T> static size_t Deserialize(const data_t& var, std::vector<T>* ret)
{
	if(!ret)throw std::invalid_argument("empty pointer");
	uint16_t uid = *(uint16_t*)(var.data());

	if(uid != __uid__std__vector_d)
		throw std::invalid_argument("uncorrect uid");
	
	size_t _byte_size = *(uint32_t*)(var.data() + 2);
	size_t size = 0;

	for (int i = 0; _byte_size > size; i++) {
		data_t temp;
		ret->resize(ret->size() + 1);
		try {
			T* dt = ret->data() + i;
			temp = data_t(var.begin() + size + 6, var.end());
			size = size + Deserialize(temp, dt);
		}
		catch (const std::exception&) {
			ret->resize(ret->size() - 1);
		}

	}
	
	return _byte_size + 6;
}

const uint16_t __uid__std__tuple_s = __uid_insert_serializer("std__tuple", 0);
const uint16_t __uid__std__tuple_d = __uid_insert_deserializer("std__tuple", 0);

template <typename... T>
static size_t Serialize(data_t& ret, const std::tuple<T...>* var)
{
	if(!var)throw std::invalid_argument("empty pointer");
	size_t size = 0;
	ret.resize(2, 0);
	*(uint16_t*)(ret.data()) = __uid__std__tuple_s;

	std::apply([&] (auto&&... args) {
		data_t temp;
	(
		(
			size = size + Serialize(temp, &args),
			ret.insert(ret.end(), temp.begin(), temp.end())
			), 
		...
		);
		}, *var);
	
	return size;
}

template <typename... T>
static size_t Deserialize(const data_t& var, std::tuple<T...>* ret)
{
	if(!ret)throw std::invalid_argument("empty pointer");
	size_t size = 2;
	uint16_t uid = *((uint16_t*)(var.data()));
	std::apply([&] (auto&&... args) {
		data_t temp;
	(
		(
			temp = data_t(var.begin() + size, var.end()),
			size = size + Deserialize(temp, &args)
			), 
		...
		);
		}, *ret);
	return size;
}

// Basic Serialize - Deserialize END

// Struct Serialize - Deserealize

static size_t Serialize(data_t& ret, const void* var, const uint16_t* uid)
{
	if(!var)throw std::invalid_argument("empty pointer");
	if(uid == 0)
		return 0;
	size_t size = 0;
	auto val = __serializer_list.find(*uid);
	if (val != __serializer_list.end()) {
		if(__serializer_list[*uid] != 0)
			size = ((serializer_t)(__serializer_list[*uid]))(ret, var);
	}
	return size;
}

static size_t Deserialize(const data_t& var, void* ret, uint16_t* uid)
{
	if(!ret)throw std::invalid_argument("empty pointer");
	if(uid == 0)
		return 0;
	*uid = *((uint16_t*)(var.data()));
	auto val = __deserializer_list.find(*uid);
	if (val != __deserializer_list.end()) {
		if(__deserializer_list[*uid] != 0)
			return ((deserializer_t)(__deserializer_list[*uid]))(var, ret);
	}
	return 0;
}

#define __GEN_STRUCT_SERIALIZE(type, tag, ...) 						\
__GEN_SERIALIZE(type, tag) {										\
	if(!var)throw std::invalid_argument("empty pointer");			\
	type* data = (type*)(var); 										\
	std::tuple tpl = {__VA_ARGS__}; 								\
	ret.resize(2); *((uint16_t*)ret.data()) = __uid__##tag##_s;		\
	data_t tret; 													\
	Serialize(tret, &tpl); 											\
	ret.insert(ret.end(), tret.begin(), tret.end()); 				\
	return ret.size();												\
}

#define __GEN_STRUCT_DESERIALIZE(type, tag, ...) 					\
__GEN_DESERIALIZE(type, tag) {										\
	if(!ret)throw std::invalid_argument("empty pointer");			\
	type* data = (type*)(ret); 										\
	std::tuple tpl = {__VA_ARGS__};									\
	uint16_t uid;													\
	uid = *((uint16_t*)(var.data()));								\
	if(uid != __uid__##tag##_d)										\
		throw std::invalid_argument(								\
			"data uid not equal to type uid");						\
	data_t tvar(var.begin() + 2, var.end());						\
	size_t ret_s = Deserialize(tvar, &tpl); 						\
	std::tie(__VA_ARGS__) = tpl;									\
	return ret_s + 4;												\
}


#define __GEN_STRUCT_SERIALIZE_DESERIALIZE(type, tag, ...) \
__GEN_STRUCT_SERIALIZE(type, tag, __VA_ARGS__) \
__GEN_STRUCT_DESERIALIZE(type, tag, __VA_ARGS__)

// Struct Serialize - Deserealize End


struct object_t
{
    uint16_t uid;
    void* data;

    template<class T>
    object_t(T* val)
    {
        uid = GetUidByObject(val);
        data = val;
    }

    object_t()
    {
        uid = -1;
        data = 0;
    }
};


__GEN_SERIALIZE(object_t, object_t)
{
	return Serialize(ret, ((object_t*)var)->data, &((object_t*)var)->uid); 
}

__GEN_DESERIALIZE(object_t, object_t)
{
    return Deserialize(var, ((object_t*)ret)->data, &((object_t*)ret)->uid);
}

}