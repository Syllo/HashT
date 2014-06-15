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

/**
 * \brief Compute a hash value giving a string.
 * \param key The key value represented by a string.
 * \param key_size The size of the string.
 * \return The value representing the hash of the key.
 */
static inline unsigned int HT_hash_function_string(const void* const key, const size_t key_size){
    size_t i;
    unsigned int hash_value = 5381;        // Some magic prime number (see its binary representation)
    for(i=0; i < key_size; ++i){
        hash_value = ((hash_value << 5) + hash_value) + (unsigned int) ((const char*) key)[i];     // hash * 33 + one_char
    }
    return hash_value;
}

/**
 * \brief Create a new string hash table already initialized with size number of keys.
 * \see HT_delete_pointer_string
 * \param size The number of keys present in the hash table.
 * \pre size must be a strictly positive number(size > 0).
 * \return A pointer to an already initialized hash table;
 * \retval NULL On failure and errno is set appropriately.
 * \warning Do not use ::HT_init_string after this or you will ensure memory leaks.
 */
static inline HT_hash_table* HT_new_hash_string(const unsigned int size){
    return HT_new_hash(size, HT_hash_function_string);
}

/**
 * \brief Use this to initialise an static defined hash table.
 * \see HT_delete_string
 * \param ht A pointer to the hash table to initialise.
 * \param size The number of keys present in the hash table.
 * \pre ht and hash_function must not be NULL.
 * \pre size must be an strictly positive number (size > 0).
 * \retval 0 On success.
 * \retval 1 On failure and errno is set appropriately.
 */
static inline int HT_init_string(HT_hash_table* ht, const unsigned int size){
    return HT_init(ht, size, HT_hash_function_string);
}

/**
 * \brief Search for an element in the hash table.
 * \param ht A pointer to the hash table.
 * \param key A pointer to the key to search in the table.
 * \param value The value holding the corresponding value if the key is found.
 * \pre ht and key must not be NULL.
 * \retval 0 On success and if not NULL value is set to point to the corresponding value.
 * \retval 1 If the key is not found.
 * \retval 2 On failure and errno is set appropriately.
 * \warning value will point directly to the hash table value.
 * \note If value is NULL this function acts like a membership test.
 */
int HT_get_element_string(const HT_hash_table* ht, const char* key, char** value);

/**
 * \brief Add an element to the hash table.
 * \param ht A pointer to the hash table.
 * \param key A pointer to the key for the hash.
 * \param value A pointer to the value corresponding with the key.
 * \pre ht, key and value must not be NULL.
 * \pre key_size and value_size must be strictly positive numbers (key_size > 0).
 * \retval 0 On success.
 * \retval 1 If the key is already present in the hash table.
 * \retval 2 On error and errno is set appropriately.
 */
int HT_add_element_string(HT_hash_table* ht, const char* key, const char* value);

/**
 * \brief Deletes the hash table created with ::HT_new_hash_string.
 * \see HT_new_hash_string
 * \param ht The hash table to delete.
 * \pre ht must not be NULL.
 */
static inline void HT_delete_pointer_string(HT_hash_table* ht){
    HT_delete_pointer(ht);
}

/**
 * \brief Delete a hash table initialized with ::HT_init_string.
 * \see HT_init_string
 * \param ht The hash table.
 * \pre ht must not be NULL.
 */
static inline void HT_delete_string(HT_hash_table* ht){
    HT_delete(ht);
}

#endif // ( HT__BASE_HASH_TABLE_H )
