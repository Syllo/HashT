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

#define HT_INITIAL_SLOT_SIZE 10

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
typedef struct{
    HT_container _key,         /**<- The key. */
                 _value;       /**<- The value associated. */
} HT_pair;

/**
 * \brief A definition made to handle the collisions cases.
 */
typedef struct{
    HT_pair *_pairs;            /**<- An array of elements with the same hash. */
    unsigned int _nb_pairs,     /**<- The numbers of elements in the array. */
                 _size_pairs;   /**<- The total size of the actual array. */
} HT_slot;

/**
 * \brief A typedef for the hash table.
 */
typedef struct{
    HT_slot *_slots;            /**<- The slots used for the hash function. */
    unsigned int _nb_slots;     /**<- The number of slots used for the hash table. */
}   HT_hash_table;

/**
 * \brief Create a new hash table already initialized with size number of keys.
 * \relatesalso HT_hash_table
 * \param size The number of keys present in the hash table. "size" must be a positive non zero number (size > 0).
 * \return A pointer to an already initialized hash table;
 * \retval NULL On failure and errno is set appropriately.
 * \warning Do not use HT_init after this or you will probably have memory leaks.
 */
HT_hash_table* HT_new_hash(const unsigned int size);

/**
 * \brief Use this to initialise an static defined hash table.
 * \relatesalso HT_hash_table
 * \param ht The hash table to initialise.
 * \param size The number of keys present in the hash table. "size" must be a positive non zero number (size > 0).
 * \retval 0 On success.
 * \retval 1 On failure and errno is set appropriately.
 */
int HT_init(HT_hash_table* ht, const unsigned int size);

/**
 * \brief Search for an element in the hash table.
 * \relatesalso HT_hash_table
 * \param ht The hash table. A non NULL pointer.
 * \param key The key to search in the table. A non NULL pointer.
 * \param value The value holding the corresponding value if the key is found.
 * \retval 0 On success and value is set to point to the corresponding value.
 * \retval 1 If the key is not found.
 * \retval 2 On failure and errno is set appropriately.
 * \warning "value" will be changed to point to the element in the table.
 */
int HT_get_element(const HT_hash_table* ht, const void* key, const size_t key_size, void** value, size_t* value_size);

/**
 * \brief Add an element to the hash table.
 * \relatesalso HT_hash_table
 * \param ht The hash table.
 * \param key The key for the hash;
 * \param value The value corresponding with the key;
 * \retval 0 On success.
 * \retval 1 If the key is already present in the hash table.
 * \retval 2 On error and errno is set appropriately.
 */
int HT_add_element(HT_hash_table* ht, const void* key, const size_t key_size, const void* value, const size_t value_size);

/**
 * \brief Deletes the hash table and all of its content.
 * \param ht The hash table to delete.
 * \retval 0 On succes.
 * \retval 1 On failure and errno is set appropriately.
 */
int HT_delete(HT_hash_table* ht);

#endif // ( __HASH_TABLE_H )
