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
 * \brief Add a value into a container.
 * \param container A pointer to an already allocated container where to copy the buffer.
 * \param buf A pointer to the buffer to copy into the container.
 * \param buff_size The size of the buffer in bytes.
 * \retval 0 On success.
 * \retval 1 On failure and errno is set appropriately.
 */
static inline int HT_add_to_container(HT_container* const container, const void* const buf, const size_t buff_size){

#ifdef HT__DEBUG    // Since this is internal to the API, error like this is not likely to happened.
    if( buff_size == 0 ){
        errno = EINVAL;
        return 1;
    }
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
 * \param container A pointer to a container.
 */
static inline void HT_destroy_container(HT_container* container){
    if( container->_size_buffer != 0){
        free(container->_buffer);
        container->_size_buffer = 0;
    }
}

/**
 * \brief Destroy a pair
 * \param p The pair to destroy.
 */
static inline void HT_destroy_pair(HT_pair* p){
    HT_destroy_container(&p->_key);
    HT_destroy_container(&p->_value);
    free(p);
}

/**
 * \brief Delete the content of a slot.
 * \param slot A pointer to the slot to reset.
 */
static inline void HT_destroy_slot_content(HT_slot* slot){
    if(slot->_first_pair != NULL){
        HT_pair *next,
                *current;
        current = slot->_first_pair;

        do{
            next = current->_next;
            HT_destroy_pair(current);
            current = next;
        }
        while(current != NULL);
    }
}

/**
 * \brief Return the slot corresponding to the hash function applied to key.
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
    key_val = ht->hash_function(key, key_size);
    slot = &ht->_slots[key_val % ht->_nb_slots];
    return slot;
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
    ht->hash_function = hash_function;
    return 0;
}

/**
 * \brief Function to get the next pair
 * \param pair The pair to get the next from
 */
static inline HT_pair *ht_next(HT_pair* pair){
    return pair->_next;
}

/**
 * \brief Function to get the previous pair
 * \param pair The pair to get the previous from
 */
static inline HT_pair *ht_previous(HT_pair* pair){
    return pair->_previous;
}

/**
 * \brief Test if a pair has the same key value than the given one
 * \param pair The pair to test
 * \param key The key to test
 * \param key_size The size of key
 */
static inline int has_same_key(const HT_pair* pair, const void *key, const size_t key_size){
    int retval;
    if(pair->_key._size_buffer == key_size){
        retval = strncmp((const char*) key, (const char*) pair->_key._buffer, key_size);
    }
    else{
        retval = 1;
    }
    return !retval; // strncmp return 0 on match
}

/**
 * \brief Get the pair matching the key that is the position th one in the list.
 * \param slot The slot to search inside
 * \param key The key
 * \param key_size The size of the key
 * \param position The position wanted
 * \param reverse Search first from end or start
 */
static HT_pair * HT_get_pair(const HT_slot *slot, const void *key, const size_t key_size, unsigned int position, int reverse){
    HT_pair* (*next_one)(HT_pair* pair) = NULL;
    HT_pair* first = NULL;
    HT_pair* last_found = NULL;
    int found = 0;

    if(reverse){
        next_one = ht_previous;
        first = slot->_last_pair;
    }
    else{
        next_one = ht_next;
        first = slot->_first_pair;
    }

    while(first != NULL && !found){
        if(has_same_key(first, key, key_size)){
            last_found = first;
            if(position == 0){
                found = 1;
            }
            else{
                --position;
            }
        }
        first = next_one(first);
    }

    return last_found;
}

int HT_get_element_position(const HT_hash_table* ht, const void* key, const size_t key_size, void** value, size_t* value_size, unsigned int position, int reverse){
    HT_slot *slot;
    HT_pair *pair;
    if( ht == NULL || key == NULL || key_size == 0 ){
        errno = EINVAL;
        return 2;
    }
    slot = HT_get_slot_from_key(ht, key, key_size);
    pair = HT_get_pair(slot, key, key_size, position, reverse);
    if( pair == NULL )
        return 1;
    if( value != NULL && value_size != NULL){
        *value = pair->_value._buffer;
        *value_size = pair->_value._size_buffer;
    }
    return 0;
}

/**
 * \brief Add the pair in after or before where
 * \param pair_to_ad The pair to add
 * \param where The chosen one NULL if the first element
 * \param previous Add previous if true and after if false
 */
static void add_pair_in_pair_chain(HT_pair* pair_to_add, HT_pair* where, int previous){
    if(where == NULL){
        pair_to_add ->_next = NULL;
        pair_to_add ->_previous = NULL;
    }
    else{
        if(previous){
            pair_to_add->_next = where;
            pair_to_add->_previous = where->_previous;
            if(where->_previous != NULL)
                where->_previous->_next = pair_to_add;
            where->_previous = pair_to_add;
        }
        else{
            pair_to_add->_previous = where;
            pair_to_add->_next = where->_next;
            if(where->_next != NULL)
                where->_next->_previous = pair_to_add;
            where->_next = pair_to_add;
        }
    }
}

/**
 * \brief Remove the pair
 * \param pair The pair to remove from the chain
 */
static void remove_pair_in_pair_chain(HT_pair* pair){
    if(pair != NULL){
        if(pair->_previous != NULL)
            pair->_previous->_next = pair->_next;
        if(pair->_next != NULL)
            pair->_next->_previous = pair->_previous;
        HT_destroy_pair(pair);
    }
}

/** \brief Creates a pair
 * \param key The key to add to the pair
 * \param key_size The size of the key
 * \param value The value to add to the pair
 * \param value_size The size of the value
 */
static inline HT_pair* new_pair(const void* const key, const size_t key_size, const void* const value, const size_t value_size){
    HT_pair* new_one = malloc(sizeof(HT_pair));
    if(new_one == NULL)
        return new_one;
    HT_add_to_container(&new_one->_key, key, key_size);
    HT_add_to_container(&new_one->_value, value, value_size);
    new_one->_next = NULL;
    new_one->_previous = NULL;
    return new_one;
}

int HT_add_element_position(HT_hash_table* const ht, const void* const key, const size_t key_size, const void* const value, const size_t value_size, unsigned int position, int reverse){
    HT_slot *slot;
    HT_pair *where;
    if( ht == NULL || key == NULL || value == NULL || key_size == 0 || value_size == 0){
        errno = EINVAL;
        return 2;
    }

    slot = HT_get_slot_from_key(ht, key, key_size);

    HT_pair* new_one = new_pair(key, key_size, value, value_size);

    if(position != 0){ // search for the pair if asked
        where = HT_get_pair(slot, key, key_size, position, reverse);
        add_pair_in_pair_chain(new_one, where, reverse);
    }
    else{ // otherwise add in first or last
        if(reverse)
            where = slot->_last_pair;
        else
            where = slot->_first_pair;
        add_pair_in_pair_chain(new_one, where, !reverse);
    }

    if(new_one->_previous == NULL)
        slot->_first_pair = new_one;
    if(new_one->_next == NULL)
        slot->_last_pair = new_one;

    return 0;
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

int HT_remove_element_position(HT_hash_table* ht, const void* key, const size_t key_size, unsigned int position, int reverse){
    HT_slot *slot;
    HT_pair *where;
    if( ht == NULL || key == NULL || key_size == 0 ){
        errno = EINVAL;
        return 2;
    }

    slot = HT_get_slot_from_key(ht, key, key_size);
    where = HT_get_pair(slot, key, key_size, position, reverse);

    if( where == NULL )
        return 1;

    if(where == slot->_first_pair)
        slot->_first_pair = where->_next;
    if(where == slot->_last_pair)
        slot->_last_pair = where->_previous;

    remove_pair_in_pair_chain(where);

    return 0;
}
