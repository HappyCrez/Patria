#pragma once
#include "dependencies.h"

/**
 * Copyright (c) 2017 Brian Barto
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the MIT License. See LICENSE for more details.
 */
char *base64simple_encode(unsigned char *a, size_t s);
unsigned char *base64simple_decode(char *, size_t, size_t *);