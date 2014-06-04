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

int HT_get_element_string(const HT_hash_table* ht, const char* key, char** value);

int HT_add_element_string(HT_hash_table* ht, const char* key, const char* value);

#endif // ( HT__BASE_HASH_TABLE_H )
