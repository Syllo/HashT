/**
 * \file generic_hash_table.c
 * \brief Generic hash table implementation.
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

#include "generic_hash_table.h"

/**
 * \brief The hash function used to hash the key value.
 */
static unsigned int (*HT_hash_function)(const void* const key, const size_t key_size) = NULL;

/**
 * \brief Add a value into a container.
 * \see HT_container
 * \param container A pointer to an already allocated container where to copy the buffer.
 * \param buf A pointer to the buffer to copy into the container.
 * \param buff_size The size of the buffer in bytes.
 * \retval 0 On success.
 * \retval 1 On failure and errno is set appropriately.
 */
static inline int HT_add_to_container(HT_container* const container, const void* const buf, const size_t buff_size){
    if( buff_size == 0 ){
        errno = EINVAL;
        return 1;
    }
#ifdef HT__DEBUG    // Since this is internal to the API, error like this is not likely to happened.
    if( container == NULL || buf == NULL ){
        fprintf(stderr, "Null pointer inside of HT_get_slot_from_key\n");
        exit(1);
    }
#endif
    container->_buffer = malloc(buff_size);
    if( container->_buffer == NULL )
        return 1;
    container->_size_buffer = buff_size;
    memcpy(container->_buffer, buf, buff_size);
    return 0;
}

/**
 * \brief Destroys the content of a container and set it's size to zero.
 * \see HT_container
 * \param container A pointer to a container.
 */
static inline void HT_destroy_container_content(HT_container* container){
    if( container->_size_buffer != 0){
        free(container->_buffer);
        container->_size_buffer = 0;
    }
}

/**
 * \brief Delete the content of a slot.
 * \see HT_slot
 * \param slot A pointer to the slot to reset.
 */
static inline void HT_destroy_slot_content(HT_slot* slot){
    unsigned int i;
    if( slot->_size_pairs != 0 ){
        for(i=0; i < slot->_nb_pairs; ++i){
            HT_destroy_container_content(&slot->_pairs[i]._key);
            HT_destroy_container_content(&slot->_pairs[i]._value);
        }
        free(slot->_pairs);
        slot->_size_pairs = 0;
    }
}

/**
 * \brief Return the slot corresponding to the hash function applied to key.
 * \see HT_slot
 * \see HT_hash_table
 * \param ht A pointer to the hash table.
 * \param key A pointer to the key to process.
 * \param key_size The size of the key in bytes.
 * \return A pointer to the slot related to the key.
 */
static inline HT_slot* HT_get_slot_from_key(const HT_hash_table* const ht, const void* const key, const size_t key_size){
#ifdef HT__DEBUG
    if( ht == NULL || key == NULL ){
        fprintf(stderr, "Null pointer inside of HT_get_slot_from_key\n");
        exit(1);
    }
#endif
    unsigned int key_val;
    HT_slot *slot;
    key_val = HT_hash_function(key, key_size);
    slot = &ht->_slots[key_val % ht->_nb_slots];
    return slot;
}

/**
 * \brief Test if a key is already in the slot from the hash table.
 * \relatesalso HT_get_slot_from_key
 * \see HT_slot
 * \param slot A pointer to the slot which should contain the key. See ::HT_get_slot_from_key.
 * \param key A pointer to the key to match with the ones present in the slot.
 * \param key_size The size of the key in bytes.
 * \return A pointer to the pair containing the key.
 * \retval NULL If the key is not found.
 */
static HT_pair* HT_key_already_exists(const HT_slot* const slot, const void* const key, const size_t key_size){
    unsigned int i;
    HT_pair *pair;
#ifdef HT__DEBUG
    if( slot == NULL || key == NULL ){
        fprintf(stderr, "Null pointer inside of HT_key_already_exists\n");
        exit(1);
    }
#endif
    for(i=0; i < slot->_nb_pairs; ++i){
        pair = &slot->_pairs[i];
        if(pair->_key._size_buffer == key_size && strncmp(pair->_key._buffer, key, key_size) == 0){ // The key is the same
            return pair;
        }
    }
    return NULL;
}

HT_hash_table* HT_new_hash(const unsigned int size, unsigned int (*hash_function)(const void* const key, const size_t key_size)){
    HT_hash_table* htable;
    int retval;
    if( size == 0 || hash_function == NULL ){
        errno = EINVAL;
        return NULL;
    }
    htable = malloc(sizeof(HT_hash_table));
    if( htable == NULL )
        return NULL;
    retval = HT_init(htable, size, hash_function);
    if( retval != 0 ){
        int errno_temp = errno;
        free( htable );
        errno = errno_temp;
        return NULL;
    }
    return htable;
}

int HT_init(HT_hash_table* const ht, const unsigned int size, unsigned int (*hash_function)(const void* const key, const size_t key_size)){
    if( ht == NULL || size == 0 || hash_function == NULL ){
        errno = EINVAL;
        return 1;
    }
    ht->_slots = malloc( size * sizeof(HT_slot) );
    if( ht->_slots == NULL )
        return 1;
    ht->_nb_slots = size;
    memset(ht->_slots, 0, size * sizeof(HT_slot));
    HT_hash_function = hash_function;
    return 0;
}

int HT_get_element(const HT_hash_table* const ht, const void* const key, const size_t key_size, void** const value, size_t* const value_size){
    HT_slot *slot;
    HT_pair *pair;
    if( ht == NULL || key == NULL || key_size == 0 ){
        errno = EINVAL;
        return 2;
    }
    slot = HT_get_slot_from_key(ht, key, key_size);
    if( slot == NULL )
        return 1;
    pair = HT_key_already_exists(slot, key, key_size);
    if( pair == NULL )
        return 1;
    *value = pair->_value._buffer;
    *value_size = pair->_value._size_buffer;
    return 0;
}

/**
 * \brief Double the size of the slot.
 * \see HT_slot
 * \param slot A pointer to the slot to initialize.
 * \retval 0 On success.
 * \retval 1 On failure and errno is set appropriately.
 */
static inline int HT_double_slot_size(HT_slot* slot){
    slot->_pairs = realloc(slot->_pairs, slot->_size_pairs * 2 * sizeof(HT_pair));
    if( slot->_pairs == NULL )
        return 1;
    memset(slot->_pairs + slot->_size_pairs, 0, slot->_size_pairs * sizeof(HT_pair));
    slot->_size_pairs *= 2;
    return 0;
}

/**
 * \brief Initializes the slot with an initial size.
 * \see HT_slot
 * \param slot A pointer to the slot to initialize.
 * \retval 0 On success.
 * \retval 1 On failure and errno is set appropriately.
 */
static inline int HT_init_slot(HT_slot* slot){
    slot->_pairs = malloc( HT_INITIAL_SLOT_SIZE * sizeof(HT_pair) );
    if( slot->_pairs == NULL )
        return 1;
    memset(slot->_pairs, 0, HT_INITIAL_SLOT_SIZE * sizeof(HT_pair));
    slot->_size_pairs = HT_INITIAL_SLOT_SIZE;
    return 0;
}

int HT_add_element(HT_hash_table* const ht, const void* const key, const size_t key_size, const void* const value, const size_t value_size){
    int retval;
    HT_slot *slot;
    HT_pair *pair;
    if( ht == NULL || key == NULL || value == NULL || key_size == 0 || value_size == 0){
        errno = EINVAL;
        return 2;
    }

    slot = HT_get_slot_from_key(ht, key, key_size);

    if( slot->_size_pairs == 0 ){ // The first time the slot is used
        retval = HT_init_slot( slot );
        if( retval != 0 )
            return 2;
    }

    if( slot->_nb_pairs == slot->_size_pairs ){ // The slot is full ... what about double the buffer size.
        retval = HT_double_slot_size( slot );
        if( retval != 0 )
            return 2;
    }

    pair = &slot->_pairs[slot->_nb_pairs];
    retval = HT_add_to_container(&pair->_key, key, key_size);
    if( retval == 0 )
        retval = HT_add_to_container(&pair->_value, value, value_size);
    slot->_nb_pairs += 1;

    return retval;
}

void HT_delete_pointer(HT_hash_table* ht){
    HT_delete(ht);
    free(ht);
}

void HT_delete(HT_hash_table* const ht){
    if( ht != NULL && ht->_nb_slots != 0 ){
        HT_reset_table(ht);
        free(ht->_slots);
    }
}

void HT_reset_table(HT_hash_table* ht){
    unsigned int i;
    if( ht != NULL && ht->_nb_slots != 0 ){
        for(i=0; i < ht->_nb_slots; ++i)
            HT_destroy_slot_content(&ht->_slots[i]);
    }
}
