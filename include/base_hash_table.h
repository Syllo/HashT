/*
   Copyright (C) 2014 SCHMITT Maxime.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   and the GNU General Public License along with this program.
   If not, see <http://www.gnu.org/licenses/>.
   */

#ifndef HT__BASE_HASH_TABLE_H
#define HT__BASE_HASH_TABLE_H

#include "generic_hash_table.h"

static inline unsigned int HT_hash_function_string(const void* const key, const size_t key_size){
    size_t i;
    unsigned int hash_value = 5381;        // Some magic prime number (see its binary representation)
    for(i=0; i < key_size; ++i){
        hash_value = ((hash_value << 5) + hash_value) + (unsigned int) ((const char*) key)[i];     // hash * 33 + one_char
    }
    return hash_value;
}

static inline HT_hash_table* HT_new_hash_string(const unsigned int size){
    return HT_new_hash(size, HT_hash_function_string);
}

static inline int HT_init_string(HT_hash_table* ht, const unsigned int size){
    return HT_init(ht, size, HT_hash_function_string);
}

int HT_get_element_string(const HT_hash_table* ht, const char* key, char** value);

int HT_add_element_string(HT_hash_table* ht, const char* key, const char* value);

static inline void HT_delete_string(HT_hash_table* ht){
    HT_delete(ht);
}

#endif // ( HT__BASE_HASH_TABLE_H )
