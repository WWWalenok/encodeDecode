#pragma once
#include <cstdio>
#include <vector>
#include <unordered_map>
#include <functional>
#include <string>
#include <string.h>
#include <tuple>
#include <stdexcept>

#ifndef WENDC_ENCRYPT_FILE 
	#define WENDC_ENCRYPT_FILE "endecrypt_base.hpp"
#endif

#include WENDC_ENCRYPT_FILE

#define WENDC_DEFAULT_DATA_FILE "data_is_byte.hpp"

#if defined(WENDC_DATA_IS_VECTOR_BYTE)
	#define WENDC_DATA_FILE "data_is_byte.hpp"
#elif defined(WENDC_DATA_IS_STRING)
	#error WENDC_DATA_IS_STRING wip
	#define WENDC_DATA_FILE "data_is_string.hpp"
#elif !defined(WENDC_DATA_FILE)
	#define WENDC_DATA_FILE WENDC_DEFAULT_DATA_FILE
#endif

#include WENDC_DATA_FILE

/*
It implements a mechanism for creating serialization and deserialization functions based on templates and macros.
Primitive types and strings are supported.
std::vector<T> is supported only with explicit serialization and deserialization.
std::tuple is also supported and based on it, mechanisms for working with user structures are implemented.
Similar to std::vector<T>, std::tuple only support explicit conversions (at least for now).
WWWalenok.
*/

typedef size_t (*deserializer_t)(const data_t&, void*);
typedef size_t (*serializer_t)(data_t&,const void*);

static std::unordered_map<uint32_t, deserializer_t> __deserializer_list; 
static std::unordered_map<uint32_t,   serializer_t> __serializer_list;

static uint32_t gen_uid(const char* name)
{
	size_t hash = 0x811c9dc5;
    while (*name) {
        hash ^= (unsigned char) *name++;
        hash *= 0x01000193;
    }
	return hash & 0x7FFFFFFF;
}

static uint32_t __uid_insert_deserializer(const char* name, deserializer_t val) 
{
	uint32_t uid = gen_uid(name);
	__deserializer_list[uid] = val;
	return uid; 
}
static uint32_t __uid_insert_serializer(const char* name, serializer_t val) 
{
	uint32_t uid = gen_uid(name);
	__serializer_list[uid] = val;
	return uid; 
}

// Serialize - Deserialize generator
template<class type> 
static size_t Serialize(data_t& ret, const type* var);

template<class type> 
static size_t Deserialize(const data_t& var, type* ret);

static size_t __serialize__primitive__(data_t& ret, const void* var, const size_t& byte_size)
{
	if(!var)throw std::invalid_argument("empty pointer");
	if(byte_size > 0x7FFFFFFF) throw std::invalid_argument("object too big for write as primitive type"); \
	uint8_t* reinterpreted = (uint8_t*)(var);
	uint8_t* encoded = new uint8_t[byte_size];
	__ENCRYPTER(reinterpreted, encoded, byte_size);
	ret = __GEN_DATA(byte_size + 4);
	*(uint32_t*)(ret.data()) = (0x80000000U | (uint32_t)byte_size);
	size_t i = 0;
	for (i = 0; i < byte_size; i++) __TO_DATA((ret.data() + 4), encoded, i);
	delete[] encoded;
	return byte_size + 4;
}

static size_t __deserialize__primitive__(const data_t& var, void* ret)
{
	if(!ret)throw std::invalid_argument("empty pointer");
	size_t byte_size = *(uint32_t*)(var.data()) & 0x7FFFFFFF;
	if(byte_size + 4 > var.size()) throw std::out_of_range("buffer too small for deserialization");
	uint8_t* reinterpreted = (uint8_t*)(ret);
	uint8_t* encoded = new uint8_t[byte_size];
	size_t i = 0;
	for (i = 0; i < byte_size; i++) __FROM_DATA(encoded, (var.data() + 4), i);
	__DECRYPTER(encoded, reinterpreted, byte_size);
	delete[] encoded;
	return byte_size + 4;
}

#define __GEN__SERIALIZE_FUNC(type) template<> size_t Serialize<type>(data_t& ret, const type* var)
#define __GEN__DESERIALIZE_FUNC(type) template<> size_t Deserialize<type>(const data_t& var, type* ret)

#define __GEN_SERIALIZE_B(type, tag) static size_t __serialize__##tag##__(data_t& ret, const void* var)
#define __GEN_SERIALIZE_E(type, tag) __GEN__SERIALIZE_FUNC(type){ return __serialize__##tag##__(ret, var); } const uint32_t __uid__##tag##_s = __uid_insert_serializer(#tag, __serialize__##tag##__); static uint32_t GetUidByObject(type* val) { return __uid__##tag##_s; }

#define __GEN_SERIALIZE(type, tag) __GEN_SERIALIZE_B(type, tag); __GEN_SERIALIZE_E(type, tag) __GEN_SERIALIZE_B(type, tag)

#define __GEN_DESERIALIZE_B(type, tag) static size_t __deserialize__##tag##__(const data_t& var, void* ret)
#define __GEN_DESERIALIZE_E(type, tag) __GEN__DESERIALIZE_FUNC(type){ return __deserialize__##tag##__(var, ret); }  const uint32_t __uid__##tag##_d = __uid_insert_deserializer(#tag, __deserialize__##tag##__);

#define __GEN_DESERIALIZE(type, tag) __GEN_DESERIALIZE_B(type, tag); __GEN_DESERIALIZE_E(type, tag) __GEN_DESERIALIZE_B(type, tag)

#define __GEN_BASE_SERIALIZE(type) __GEN__SERIALIZE_FUNC(type) { return __serialize__primitive__(ret, var, sizeof(type)); } \
static uint32_t GetUidByObject(type* val) { return sizeof(type) | 0x80000000; }

#define __GEN_BASE_DESERIALIZE(type) __GEN__DESERIALIZE_FUNC(type) { return __deserialize__primitive__(var, ret); }

#define __GEN_BASE_SERIALIZE_DESERIALIZE(type) \
__GEN_BASE_SERIALIZE(type) \
__GEN_BASE_DESERIALIZE(type)

#define __GEN_BASE_SERIALIZE_DESERIALIZE_BASE(type) \
__GEN_BASE_SERIALIZE_DESERIALIZE(type)

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
	ret = __GEN_DATA(_byte_size + 8);
	data_t tret;

	size_t byte_size = _byte_size; 
	uint8_t* reinterpreted = (uint8_t*)(data->data()); 
	uint8_t* encoded = new uint8_t[byte_size];
	__ENCRYPTER(reinterpreted, encoded, byte_size); 
	tret = __GEN_DATA(byte_size); 
	size_t i = 0; 
	for (i = 0; i < byte_size; i++) __TO_DATA(tret, encoded, i); 
	delete[] encoded;

	*(uint32_t*)(ret.data()) = __uid__std_string_s;
	*(uint32_t*)(ret.data() + 4) = _byte_size;

	memcpy(ret.data() + 8, tret.data(), _byte_size);

	return _byte_size + 8;
}

__GEN_DESERIALIZE(std::string, std_string)
{
	if(!ret)throw std::invalid_argument("empty pointer");
	std::string* data = (std::string*)ret;
	size_t _byte_size = *(uint32_t*)(var.data() + 4);
	size_t uid = *(uint32_t*)(var.data());
	size_t element_size = _byte_size;
	*data = std::string(element_size, '\0');
	size_t byte_size = _byte_size;

	if(_byte_size + 8 > var.size()) throw std::out_of_range("buffer too small for deserialization");
	uint8_t* reinterpreted = (uint8_t*)(data->data()); 
	uint8_t* encoded = new uint8_t[byte_size]; 
	size_t i = 0; 
	for (i = 0; i < byte_size; i++) __FROM_DATA(encoded, (&var[8]), i);
	__DECRYPTER(encoded, reinterpreted, byte_size); 
	delete[] encoded; 
	return _byte_size + 8;
}

const uint32_t __uid__std__vector_s = __uid_insert_serializer("std__vector", 0);
template <typename T> size_t Serialize(data_t& ret, const std::vector<T>* var)
{
	if(!var)throw std::invalid_argument("empty pointer");
	std::vector<T>* data = (std::vector<T>*)var;
	size_t _byte_size = 0;
	ret.resize(8, 0);

	for (size_t i = 0; i < data->size(); i++) {
		data_t temp = {};
		const T* dt = data->data() + i;
		_byte_size = _byte_size + Serialize(temp, dt);
		ret.insert(ret.end(), temp.begin(), temp.end());
	}
	*(uint32_t*)(ret.data()) = __uid__std__vector_s;
	*(uint32_t*)(ret.data() + 4) = _byte_size;
	return _byte_size + 8;
}

static size_t Deerialize_bytearray(const data_t& var, void* ret);
const uint32_t __uid__std__vector_d = __uid_insert_deserializer("std__vector", Deerialize_bytearray);

static size_t Deerialize_bytearray(const data_t& var, void* ret){
	return 0;
}

template <typename T> static size_t Deserialize(const data_t& var, std::vector<T>* ret)
{
	if(!ret)throw std::invalid_argument("empty pointer");
	uint32_t uid = *(uint32_t*)(var.data());

	if(uid != __uid__std__vector_d)
		throw std::invalid_argument("uncorrect uid");
	
	size_t _byte_size = *(uint32_t*)(var.data() + 4);
	size_t size = 0;

	for (int i = 0; _byte_size > size; i++) {
		data_t temp;
		ret->resize(ret->size() + 1);
		try {
			T* dt = ret->data() + i;
			temp = data_t(var.begin() + size + 8, var.end());
			size = size + Deserialize(temp, dt);
		}
		catch (const std::exception&) {
			ret->resize(ret->size() - 1);
		}

	}
	
	return _byte_size + 8;
}

const uint32_t __uid__std__tuple_s = __uid_insert_serializer("std__tuple", 0);
const uint32_t __uid__std__tuple_d = __uid_insert_deserializer("std__tuple", 0);

template <typename... T>
static size_t Serialize(data_t& ret, const std::tuple<T...>* var)
{
	if(!var)throw std::invalid_argument("empty pointer");
	size_t size = 4;
	ret.resize(4, 0);
	*(uint32_t*)(ret.data()) = __uid__std__tuple_s;

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
	size_t size = 4;
	uint32_t uid = *((uint32_t*)(var.data()));
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

static size_t Serialize(data_t& ret, const void* var, const uint32_t* uid)
{
	if(!var)throw std::invalid_argument("empty pointer");
	if(uid == 0)
		return 0;
	size_t size = 0;
	if(*uid & 0x80000000)
		size = __serialize__primitive__(ret, var, *uid);
	else {
		auto val = __serializer_list.find(*uid);
		if (val != __serializer_list.end()) {
			if(__serializer_list[*uid] != 0)
				size = ((serializer_t)(__serializer_list[*uid]))(ret, var);
		}
	}
	return size;
}

static size_t Deserialize(const data_t& var, void* ret, uint32_t* uid)
{
	if(!ret)throw std::invalid_argument("empty pointer");
	if(uid == 0)
		return 0;
	*uid = *((uint32_t*)(var.data()));
	if(*uid & 0x80000000)
		return __deserialize__primitive__(var, ret);
	else {
		auto val = __deserializer_list.find(*uid);
		if (val != __deserializer_list.end()) {
			if(__deserializer_list[*uid] != 0)
				return ((deserializer_t)(__deserializer_list[*uid]))(var, ret);
		}
	}
	return 0;
}

#define __GEN_STRUCT_SERIALIZE(type, tag, ...) 						\
__GEN_SERIALIZE(type, tag) {										\
	if(!var)throw std::invalid_argument("empty pointer");			\
	type* d = (type*)(var); 										\
	std::tuple tpl = {__VA_ARGS__}; 								\
	ret.resize(4); *((uint32_t*)ret.data()) = __uid__##tag##_s;		\
	data_t tret; 													\
	Serialize(tret, &tpl); 											\
	ret.insert(ret.end(), tret.begin(), tret.end()); 				\
	return ret.size();												\
}

#define __GEN_STRUCT_DESERIALIZE(type, tag, ...) 					\
__GEN_DESERIALIZE(type, tag) {										\
	if(!ret)throw std::invalid_argument("empty pointer");			\
	type* d = (type*)(ret); 										\
	std::tuple tpl = {__VA_ARGS__};									\
	uint32_t uid;													\
	uid = *((uint32_t*)(var.data()));								\
	if(uid != __uid__##tag##_d)										\
		throw std::invalid_argument(								\
			"data uid not equal to type uid");						\
	data_t tvar(var.begin() + 4, var.end());						\
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
    uint32_t uid;
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