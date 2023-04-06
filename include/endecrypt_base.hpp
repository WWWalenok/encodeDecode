#pragma once

#ifndef __ENCRYPTER
#define __ENCRYPTER(src, dest, byte_size) memcpy(dest, src, byte_size);
#endif

#ifndef __DECRYPTER
#define __DECRYPTER(src, dest, byte_size) memcpy(dest, src, byte_size);
#endif