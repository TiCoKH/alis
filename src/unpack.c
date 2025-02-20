//
// Copyright 2023 Olivier Huguenot, Vadim Kindl
//
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the “Software”), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in 
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, 
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include "alis.h"
#include "mem.h"
#include "unpack.h"
#include "utils.h"

const u8 kPackedHeaderSize = 6;
const u8 kPackedDictionarySize = 8;
const u8 kVMSpecsSize = 16;
const u8 kPackedDatHeaderSize = 12;

#define kUnpackFolder           "unpacked"
#define kUnpackExtension        "alis"

#define BIT_CLR(v, b)       (v &= ~(1UL << b))
#define BIT_SET(v, b)       (v |= (1UL << b))
#define BIT_CHG(v, b)       (v ^= (1UL << b))
#define BIT_CHK(v, b)       ((v >> b) & 1U)

#if (defined(_WIN32) || defined(__WIN32__))  // mkdir command on Win32 (MinGW) takes one argument
#define mkdir(a, b) mkdir(a)                 // Cygwin (_WIN32 is not set) should work well with two arguments
#endif

// ============================================================================
#pragma mark - Old Silmarils unpacker
// ============================================================================

s8 old_read_byte(u8** ptr_packed, u8* ptr_packed_end) {
    return *ptr_packed == ptr_packed_end ? 0 : *(*ptr_packed)++;
}

void old_write_byte(u8** ptr_unpacked, u8 byte, u8 inc) {

    **ptr_unpacked = byte;
    *ptr_unpacked += inc;
}

void unpack_old(u8* ptr_packed, const u32 packed_sz, u8* ptr_unpacked, const u32 unpacked_sz, s8 modpack) {

    u8* ptr_packed_end = ptr_packed + packed_sz;
    u8* ptr_unpacked_beg = ptr_unpacked;
    u8* ptr_unpacked_end = ptr_unpacked + unpacked_sz;

    u8 cnt = modpack;

    do {
        s8 byte = old_read_byte(&ptr_packed, ptr_packed_end);
        s8 counter = byte & 0x7f;

        if (byte < 0) {
            // read byte, and repeat copy
            byte = old_read_byte(&ptr_packed, ptr_packed_end);
            while (counter--) {
                if (ptr_unpacked >= ptr_unpacked_end) {
                    if ((--cnt) < 1)
                        break;

                    ptr_unpacked = ptr_unpacked_beg + (modpack - cnt);
                }

                *ptr_unpacked = byte;
                ptr_unpacked += modpack;
            }
        }
        else {
            // read & copy bytes
            while (counter--) {

                byte = old_read_byte(&ptr_packed, ptr_packed_end);
                if (ptr_unpacked >= ptr_unpacked_end) {
                    if ((--cnt) < 1)
                        break;

                    ptr_unpacked = ptr_unpacked_beg + (modpack - cnt);
                }

                *ptr_unpacked = byte;
                ptr_unpacked += modpack;
            }
        }
    }
    while (ptr_packed != ptr_packed_end && cnt > 0);
}

// ============================================================================
#pragma mark - New Silmarils unpacker
// ============================================================================

/// @brief swap higher and lower words in long value
/// @param val value to alter
/// @return altered value
u32 swap(u32 val) {
   u32 hi = val & 0xffff0000;
   u32 lo = val & 0x0000ffff;
   return lo << 16 | hi >> 16;
}

/// @brief ROTate Left Long
/// @param shift number bits to rotate
/// @param value value to alter
/// @return altered value
u32 rotll(u8 shift, u32 value) {
    if ((shift &= sizeof(value)*8 - 1) == 0)
        return value;
    return (value << shift) | (value >> (sizeof(value)*8 - shift));
}

s8 byte(u32 value) {
    return value & 0xff;
}

s16 word(u32 value) {
    return value & 0xffff;
}

// TODO: determiner quel type de data on utilise pour les regs
s32 d5, d7;
u32 __unpack_counter = 0;


// USES 
// io d5
// io d7
void decode(u8** ptr_packed, u16 bit) {
    d5 &= 0xffff0000;
    
    d7 = (d7 & 0xffffff00) + ((byte(d7) - byte(bit)) & 0xff);
    if(byte(d7) < 0) {
        // worm
        // TODO: on add d7 car on a soustrait avant... 
        // autant comparer d0 et d7 dans le if au dessus
        // sans modifier d7 ?
        d7 = (d7 & 0xffffff00) + ((byte(d7) + byte(bit)) & 0xff);
        d5 = (d5 & 0xffff0000);
        d5 = rotll(d7, d5);
        d5 = swap(d5);

        // here, original code tests if we need more packed data chunks

        u8 hi = *(*ptr_packed)++;
        u8 lo = *(*ptr_packed)++;
        u16 pack_word = (hi << 8) + lo;
        d5 = (d5 & 0xffff0000) + pack_word;
        d5 = swap(d5);

        bit = ((byte(bit) - byte(d7)) & 0xff);
        d7 = 0x10;

        d5 = rotll(bit, d5);
        d7 = (d7 & 0xffffff00) + ((byte(d7) - byte(bit)) & 0xff);
    }
    else {
        d5 = rotll(bit, d5);
    }
}

void write_neg(u8** ptr_unpacked, s16 val, s16* counter) {
    // FML - neg lower word of d5

    s16 offset = val * -1;
    // SXX 
    do {
        u8 c = *(*ptr_unpacked + offset - 1);
        *(*ptr_unpacked)++ = c;
        __unpack_counter++;
    } while(--(*counter) != -1);
}

void count(u8** ptr_packed, u8 start, u8 stop, s16* counter) {
    do {
        decode(ptr_packed, start);
        *counter += word(d5);
    } while (word(d5) == stop);
}

/// @brief Unpack ALIS script
/// @param ptr_packed   pointer to raw packed data (w/o header, specs, dictionary)
/// @param ptr_unpacked pointer to allocated buffer for unpacked data
/// @param unpacked_sz  unpacked data size (read from packed header)
/// @param dictionary   dictionary (8-byte buffer read from packed header)
/// @return length of unpacked data
u32 unpack_new(u8* ptr_packed,
               u8* ptr_unpacked, 
               const u32 unpacked_sz,
               u8* dictionary) {

    s16 counter = 0;
    __unpack_counter = 0;
    u8* ptr_unpacked_end = ptr_unpacked + unpacked_sz;
    d7 = 0;
    while(ptr_unpacked < ptr_unpacked_end) {
        // loop
        decode(&ptr_packed, 1);
        if(byte(d5) != 0) {
            counter = 0;
            
            // ZMB - get number of bytes to write to output
            count(&ptr_packed, 2, 3, &counter);

            // QQCH - write d2+1 unpacked bytes
            do {
                decode(&ptr_packed, 8);
                *(u8*)ptr_unpacked++ = byte(d5);
                __unpack_counter++;
            } while(--counter != -1);
            
            // check unpack end 
            // TODO: move test to MMI ?
            if(ptr_unpacked >= ptr_unpacked_end) {
                break;
            }
        }

        // MMI
        decode(&ptr_packed, 3);
        u16 bit = dictionary[byte(d5)];
        d5 = (d5 & 0xffff0000) + (word(d5 & 3));
        if(word(d5) != 0) {
            counter = word(d5);
            decode(&ptr_packed, bit);
            write_neg(&ptr_unpacked, word(d5), &counter);
        }
        else {
            // SNS
            decode(&ptr_packed, bit);
            u16 save_d5w = word(d5);
            counter = 0;

            // CRVL
            count(&ptr_packed, 3, 7, &counter);

            // CRVL2
            d5 = (d5 & 0xffff0000) + save_d5w;
            counter += 4;
            write_neg(&ptr_unpacked, word(d5), &counter);
        }
    }
    ALIS_DEBUG(EDebugInfo, "unpack counter=%d\n", __unpack_counter);
    return __unpack_counter;
}

// ============================================================================
#pragma mark - Unpacker
// ============================================================================

u8 is_packed(u8 packer_kind) {
    return packer_kind < 0;
}

/// @brief Unpacks a script file to buffer
/// @param packed_file_path full path to packed file
/// @param unpacked_buffer output buffer
/// @return unpacked file size if success, a negative value if error, or
/// zero if the input file is not packed
int unpack_script(const char *packed_file_path, u8 *unpacked_buffer) {
    int ret = 0;
    FILE *pfp = fopen(packed_file_path, "rb");

    if(pfp) {
        // get packed sz
        fseek(pfp, 0, SEEK_END);
        u32 packed_size = (u32)ftell(pfp);
        rewind(pfp);

        u32 magic = fread32(pfp);
        
        u16 is_main = fread16(pfp) == 0;
        u32 unpacked_size = (magic & 0x00ffffff);
        u8 *dict = malloc(kPackedDictionarySize);

        packed_size -= kPackedHeaderSize; // size of magic + is_main
        unpacked_size -= kPackedHeaderSize; // TODO: not sure about that
        if(is_main) {
            // skip vm specs
            fseek(pfp, kVMSpecsSize, SEEK_CUR);
            packed_size -= kVMSpecsSize;
            unpacked_size -= kVMSpecsSize; // TODO: not sure about that
        }

        if ((alis.platform.version > 11 && (alis.typepack & 0xf0) == 0xa0)
        // TODO: a quick fix to make the MadShow re-release work
         || (alis.platform.version == 11 && (alis.typepack & 0xff) == 0xa1)) {
            // read dictionary
            fread(dict, sizeof(u8), kPackedDictionarySize, pfp);
            packed_size -= kPackedDictionarySize;
        }

        // read packed bytes in alloc'd buffer
        u8 *packed_buffer = (u8*)malloc(0xff + packed_size * sizeof(u8));
        memset(packed_buffer, 0, 0xff + packed_size * sizeof(u8));
        fread(packed_buffer, sizeof(u8), packed_size, pfp);
        fclose(pfp);

        if ((alis.platform.version > 11 && (alis.typepack & 0xf0) == 0xa0)
        // TODO: a quick fix to make the MadShow re-release work
         || (alis.platform.version == 11 && (alis.typepack & 0xff) == 0xa1))
        {
            unpack_new(packed_buffer, unpacked_buffer, unpacked_size, dict);
        }
        else 
        {
            if (alis.platform.kind == EPlatformPC)
            {
                alis.typepack &= 0xfe;
                
                s8 modpack = ((u8)alis.typepack == 0x80) ? 1 : ((u8)alis.typepack == 0xa0) ? 2 : 8;
                unpack_old(packed_buffer, packed_size, unpacked_buffer, unpacked_size, modpack);
            }
            else
            {
                s8 modpack = (((u8)alis.typepack & 0x40) == 0) ? 1 : 8;
                unpack_old(packed_buffer, packed_size, unpacked_buffer, unpacked_size, modpack);
            }
        }
        
        ALIS_DEBUG(EDebugInfo, "Unpacked %s: %d bytes into %d bytes (~%d%% packing ratio) using %s packer.\n", packed_file_path, packed_size, unpacked_size, 100 - (int)((packed_size * 100) / unpacked_size), (alis.platform.version >= 20 && (alis.typepack & 0xf0) == 0xa0) ? "new" : "old");
        free(dict);
        free(packed_buffer);
        packed_buffer = NULL;
        ret = unpacked_size;
    }
    else {
        // error
        ALIS_DEBUG(EDebugFatal, "Cannot open input file (%s)\n", packed_file_path);
        ret = EUnpackErrorInput;
    }
    return ret;
}

int unpack_script_fp(FILE *fp, u8 *unpacked_buffer, u32 unpacked_size) {
    int ret = 0;

    if(fp) {

        u32 packed_size = unpacked_size;
        
        // read packed bytes in alloc'd buffer
        u8 *packed_buffer = (u8 *)malloc(0xff + packed_size * sizeof(u8));
        memset(packed_buffer, 0, 0xff + packed_size * sizeof(u8));
        
        if ((alis.platform.version > 11 && (alis.typepack & 0xf0) == 0xa0)
        // TODO: a quick fix to make the MadShow re-release work
         || (alis.platform.version == 11 && (alis.typepack & 0xff) == 0xa1))
        {
            u8 *dict = malloc(kPackedDictionarySize);
            fread(dict, sizeof(u8), kPackedDictionarySize, fp);
            fread(packed_buffer, sizeof(u8), packed_size, fp);

            unpack_new(packed_buffer, unpacked_buffer, unpacked_size, dict);

            free(dict);
        }
        else
        {
            fread(packed_buffer, sizeof(u8), packed_size, fp);

            if (alis.platform.kind == EPlatformPC)
            {
                alis.typepack &= 0xfe;
                
                s8 modpack = ((u8)alis.typepack == 0x80) ? 1 : ((u8)alis.typepack == 0xa0) ? 2 : 8;
                unpack_old(packed_buffer, packed_size, unpacked_buffer, unpacked_size, modpack);
            }
            else
            {
                s8 modpack = (((u8)alis.typepack & 0x40) == 0) ? 1 : 8;
                unpack_old(packed_buffer, packed_size, unpacked_buffer, unpacked_size, modpack);
            }
        }
        
        ALIS_DEBUG(EDebugInfo, "Unpacked FP: %d bytes into %d bytes (~%d%% packing ratio) using %s packer.\n", packed_size, unpacked_size, 100 - (int)((packed_size * 100) / unpacked_size), (alis.platform.version >= 20 && (alis.typepack & 0xf0) == 0xa0) ? "new" : "old");
        free(packed_buffer);
        ret = unpacked_size;
    }
    else {
        // error
        ALIS_DEBUG(EDebugFatal, "FP is NULL\n");
        ret = EUnpackErrorInput;
    }
    return ret;
}
