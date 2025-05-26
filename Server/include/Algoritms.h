#pragma once
#include "Dependencies.h"

// ----------------------------------
// Copyright (c) 2017 Brian Barto
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the MIT License. See LICENSE for more details.

char *base64simple_encode(unsigned char *, size_t);
unsigned char *base64simple_decode(char *, size_t, size_t *);

// ----------------------------------

// Check that any of strings is having another  
bool stringCompare(char *str1, char *str2);