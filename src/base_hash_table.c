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

#include "base_hash_table.h"

int HT_get_element_string(const HT_hash_table* ht, const char* key, char** value){
    size_t size_key,
           size_value;
    if( key == NULL ){
        errno = EINVAL;
        return 2;
    }
    size_key = strlen(key); // The key don't contain the '\0' delimiter
    return HT_get_element(ht, key, size_key, (void*) value, &size_value);
}

int HT_add_element_string(HT_hash_table* ht, const char* key, const char* value){
    size_t size_key,
           size_value;
    if( key == NULL || value == NULL ){
        errno = EINVAL;
        return 2;
    }
    size_key = strlen(key);         // The key don't contain the '\0' delimiter
    size_value = strlen(value) + 1; // The value is stored with the '\0' delimiter
    return HT_add_element(ht, key, size_key, value, size_value);
}
