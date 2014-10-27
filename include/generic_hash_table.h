/**
 * \file generic_hash_table.h
 * \brief Generic hash table header file.
 * \author Maxime SCHMITT
 * \version 0.1
 * \date 2014
 * \copyright LGPL v3, or any later version.
 */

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

#ifndef __HASH_TABLE_H
#define __HASH_TABLE_H

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#ifdef HT__DEBUG
#include <stdio.h>
#endif

/**
 * \brief The initial number of pairs in a slot.
 */
#define HT_INITIAL_SLOT_SIZE 16

/**
 * \brief A basic container where to store data.
 */
typedef struct{
    void *_buffer;                  /**<- The buffer where to store the data */
    size_t _size_buffer;            /**<- The size of the buffer */
} HT_container;

/**
 * \brief A definition of a pair containing the key and its associated value.
 */
typedef struct HT_pair{
    HT_container _key,          /**<- The key. */
                 _value;        /**<- The value associated. */
    struct HT_pair *_next,      /**<- The next pair */
                   *_previous;  /**<- The previous pair */
} HT_pair;

/**
 * \brief A definition made to handle the collisions cases.
 */
typedef struct{
    HT_pair *_first_pair,            /**<- The first pair */
            *_last_pair;
} HT_slot;

/**
 * \brief A typedef for the hash table.
 */
typedef struct{
    HT_slot *_slots;            /**<- The slots used for the hash function. */
    unsigned int _nb_slots;     /**<- The number of slots used for the hash table. */
    unsigned int (*hash_function)(const void* const key, const size_t key_size); /**<- the hesh function. */
}   HT_hash_table;

/**
 * \brief Create a new hash table already initialized with size number of keys.
 * \see HT_delete_pointer
 * \param size The number of keys present in the hash table.
 * \param hash_function A pointer to a function to use for hashing the key values.
 * \pre size must be a strictly positive number(size > 0).
 * \pre hash_function should not be NULL.
 * \return A pointer to an already initialized hash table;
 * \retval NULL On failure and errno is set appropriately.
 * \warning Do not use ::HT_init after this or you will ensure memory leaks.
 */
HT_hash_table* HT_new_hash(const unsigned int size, unsigned int (*hash_function)(const void* const key, const size_t key_size));

/**
 * \brief Use this to initialise an static defined hash table.
 * \see HT_delete
 * \param ht A pointer to the hash table to initialise.
 * \param hash_function A pointer to a function to use for hashing the key values.
 * \param size The number of keys present in the hash table.
 * \pre ht and hash_function must not be NULL.
 * \pre size must be an strictly positive number (size > 0).
 * \retval 0 On success.
 * \retval 1 On failure and errno is set appropriately.
 */
int HT_init(HT_hash_table* ht, const unsigned int size, unsigned int (*hash_function)(const void* const key, const size_t key_size));

/**
 * \brief Search for an element in the hash table.
 * \param ht A pointer to the hash table.
 * \param key A pointer to the key to search in the table.
 * \param key_size The size of the key in bytes.
 * \param value The value holding the corresponding value if the key is found.
 * \param value_size A pointer to a variable that will be set to value size in bytes if a match is found.
 * \param position The number of match before returning the value.
 * \param reverse 0 to search from begin to end and any other integer otherwise.
 * \pre ht and key must not be NULL.
 * \pre key_size must be an strictly positive number (key_size > 0).
 * \retval 0 On success and if not NULL value is set to point to the corresponding value.
 * \retval 1 If the key is not found.
 * \retval 2 On failure and errno is set appropriately.
 * \warning value will point directly to the hash table value.
 * \note If value is NULL this function acts like a membership test.
 */
int HT_get_element_position(const HT_hash_table* ht, const void* key, const size_t key_size, void** value, size_t* value_size, unsigned int position, int reverse);

/**
 * \brief Search for an element in the hash table.
 * \param ht A pointer to the hash table.
 * \param key A pointer to the key to search in the table.
 * \param key_size The size of the key in bytes.
 * \param value The value holding the corresponding value if the key is found.
 * \param value_size A pointer to a variable that will be set to value size in bytes if a match is found.
 * \pre ht and key must not be NULL.
 * \pre key_size must be an strictly positive number (key_size > 0).
 * \retval 0 On success and if not NULL value is set to point to the corresponding value.
 * \retval 1 If the key is not found.
 * \retval 2 On failure and errno is set appropriately.
 * \warning value will point directly to the hash table value.
 * \note If value is NULL this function acts like a membership test.
 */
inline int HT_get_element(const HT_hash_table* ht, const void* key, const size_t key_size, void** value, size_t* value_size){
    return HT_get_element_position(ht, key, key_size, value, value_size, 0, 0);
}

/**
 * \brief Add an element to the hash table.
 * \param ht A pointer to the hash table.
 * \param key A pointer to the key for the hash.
 * \param key_size The size of the key in bytes.
 * \param value A pointer to the value corresponding with the key.
 * \param value_size The size of the value in bytes.
 * \param position The number of match before returning the value.
 * \param reverse 0 to search from begin to end and any other integer otherwise.
 * \pre ht, key and value must not be NULL.
 * \pre key_size and value_size must be strictly positive numbers (key_size > 0).
 * \retval 0 On success.
 * \retval 1 If the key is already present in the hash table.
 * \retval 2 On error and errno is set appropriately.
 */
int HT_add_element_position(HT_hash_table* ht, const void* key, const size_t key_size, const void* value, const size_t value_size, unsigned int position, int reverse);

/**
 * \brief Add an element to the hash table.
 * \param ht A pointer to the hash table.
 * \param key A pointer to the key for the hash.
 * \param key_size The size of the key in bytes.
 * \param value A pointer to the value corresponding with the key.
 * \param value_size The size of the value in bytes.
 * \pre ht, key and value must not be NULL.
 * \pre key_size and value_size must be strictly positive numbers (key_size > 0).
 * \retval 0 On success.
 * \retval 1 If the key is already present in the hash table.
 * \retval 2 On error and errno is set appropriately.
 */
inline int HT_add_element(HT_hash_table* ht, const void* key, const size_t key_size, const void* value, const size_t value_size){
    int retval = HT_get_element_position(ht, key, key_size, NULL, NULL, 0, 0);
    if(retval == 1)
        return HT_add_element_position(ht, key, key_size, value, value_size, 0, 0);
    else
        return 1;
}

/**
 * \brief Deletes the hash table created with ::HT_new_hash.
 * \see HT_new_hash
 * \param ht The hash table to delete.
 * \pre ht must not be NULL.
 */
void HT_delete_pointer(HT_hash_table* ht);

/**
 * \brief Delete a hash table initialized with ::HT_init.
 * \see HT_init
 * \param ht The hash table.
 * \pre ht must not be NULL.
 */
void HT_delete(HT_hash_table* ht);

/**
 * \brief Reset the content of the hash table without the need of creating a new one.
 * \param ht A pointer to the hash table.
 * \pre ht must not be NULL.
 * \post ht is still usable.
 */
void HT_reset_table(HT_hash_table* ht);

#endif // ( __HASH_TABLE_H )
