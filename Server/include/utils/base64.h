/**
 * @file base64.h
 * @brief file has interfaces to encode and decode in base64 algorithm
 */

/**
 * Copyright (c) 2017 Brian Barto
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the MIT License. See LICENSE for more details.
 */

#pragma once
#include "dependencies.h"

/**
 * @brief encode data in base64 algorithm
 * @param data pointer on data
 * @param data_len sizeof data to encode
 * @return new encoded data
 */
char *base64_encode(unsigned char *data, size_t data_len);

/**
 * @brief decode data that was encoded in base64 algorithm
 * @param data encoded data to decode
 * @param data_len len of encoded data
 * @param decoded_len pointer on mem that will be overwriten by length of decoded data 
 * @return new decoded data
 */
unsigned char *base64_decode(char *data, size_t data_len, size_t *decoded_len);