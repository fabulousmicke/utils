/**
 * @file tkey_sparse.h
 * @brief Sparse ternary key definition.
 * @author Mikael Sundstrom <micke@fabinv.com>
 *
 * @copyright Copyright (c) 2020 Fabulous Inventions AB - all rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#ifndef TKEY_SPARSE
#define TKEY_SPARSE

#include <stdint.h>
#include <stdbool.h>

#include <tbit.h>
#include <ttype.h>
#include <relation.h>

/**
 * @brief Ternary key histogram type.
 *
 * Used to keep track of the distribution of of ternary bit values across bit
 * indices for a collection of keys.
 */
struct tkey_sparse;
typedef struct tkey_sparse tkey_sparse_t;

/**
 * @brief Ternary key histogram type.
 *
 * Used to keep track of the distribution of of ternary bit values across bit
 * indices for a collection of keys.
 */
struct tkey_sparse_hist;
typedef struct tkey_sparse_hist tkey_sparse_hist_t;

/**
 * @brief Create new ternary key where all bits are set to the default value according to the type.
 *
 * @param key Location of key to be created.
 * @param type Key type.
 */
void tkey_sparse_create(tkey_sparse_t **key, ttype_t *type);

/**
 * @brief Destroy ternary key previously created with tkey_sparse_create or tkey_sparse_clone.
 *
 * @param key Location of key to be destroyed.
 */
void tkey_sparse_destroy(tkey_sparse_t **key);

/**
 * @brief Clone key.
 *
 * @param original Original source key.
 * @param clone Location of cloned destination key.
 */
void tkey_sparse_clone(tkey_sparse_t *original, tkey_sparse_t **clone);

/**
 * @brief Assign bits in key from string.
 *
 * @param key Key to assign bit values to. 
 * @param buf String representing key.
 * @param len Length of string.
 *
 * @return Number of bits assigned.
 *
 * @note Assignment starts with bit index 0 and stops either all bits of the key has been assigned, the string ends or an invalid ternary bit character is encountered.
 */
size_t tkey_sparse_from_string(tkey_sparse_t *key, char *buf, size_t len);

/**
 * @brief Convert key to string.
 *
 * @param key Key to be converted.
 * @param buf Buffer where string is stored.
 * @param len Maximum size of string including EOS at end fo string. 
 */
void tkey_sparse_to_string(tkey_sparse_t *key, char *buf, size_t len);

/**
 * @brief Get ternary bit with given index from key.
 *
 * @param key Key to get ternary bit from.
 * @param index Bit index.
 *
 * @return Ternary bit at the given index/position.
 */
tbit_t tkey_sparse_get(tkey_sparse_t *key, size_t index);

/**
 * @brief Put ternary bit at position with given index in key.
 *
 * @param key Key to put ternary bit in.
 * @param index Bit index.
 * @param bit Ternary bit to put at the given index/position.
 */
void tkey_sparse_put(tkey_sparse_t *key, size_t index, tbit_t bit);

/**
 * @brief Retrieve quad with given quad index from key.
 *
 * @param key
 * @param quad_index
 * @param quad Location to store retrieved quad.
 *
 * @return Boolean true on success and false otherwise (e.g. quad_index out-of-bounds).
 */
bool tkey_sparse_quad(tkey_sparse_t *key, size_t quad_index, uint64_t *quad);

/**
 * @brief Determine the relationship between a pair of ternary keys.
 *
 * @param key1 First key.
 * @param key2 Second key.
 *
 * @return Relationship between key1 and key2.
 */
relation_t tkey_sparse_relation(tkey_sparse_t *key1, tkey_sparse_t *key2);

/**
 * @brief Return type of key.
 *
 * @param key Key to retrieve type from.
 *
 * @return Type of key.
 */
ttype_t *tkey_sparse_type(tkey_sparse_t *key);

/**
 * @brief Create ternary key histogram.
 *
 * @param hist Location of histogram to be created.
 * @param size Key size measured in Number of ternary bits.
 * @param init Ternary bit value assigned to all bits on creation (i.e default value).
 *
 * @note size and init must be the same as size and init of the keys in the
 * collection "managed" by the histogram.
 */
void tkey_sparse_hist_create(tkey_sparse_hist_t **hist, ttype_t *type);

/**
 * @brief Destroy ternary key histogram.
 *
 * @param hist Location of histogram to be dstroyed.
 */
void tkey_sparse_hist_destroy(tkey_sparse_hist_t **hist);

/**
 * @brief Add new (i.e. has not been added before) ternary key to histogram.
 *
 * @param hist Histogram.
 * @param key Ternary key.
 *
 * @note The histogram implementation may- or may not keep track of added keys. 
 */
void tkey_sparse_hist_add(tkey_sparse_hist_t *hist, tkey_sparse_t *key);

/**
 * @brief Delete existing (i.e. was added before) ternary key to histogram.
 *
 * @param hist Histogram.
 * @param key Ternary key.
 *
 * @note The histogram implementation may- or may not keep track of added keys. 
 */
void tkey_sparse_hist_del(tkey_sparse_hist_t *hist, tkey_sparse_t *key);

/**
 * @brief Report number of keys added to histogram.
 *
 * @param hist Histogram.
 *
 * @return Number of keys added to histogram.
 */
size_t tkey_sparse_hist_count(tkey_sparse_hist_t *hist);

/**
 * @brief Report distribution of ternary bit values at at given index.
 *
 * @param hist Histogram.
 * @param index Ternary bit index.
 * @param dist Array of three counters dist[TBIT0..TBIT_] where distribution is stored.
 */
void tkey_sparse_hist_dist(tkey_sparse_hist_t *hist, size_t index, size_t *dist);

/**
 * @brief Determine if key set can be reduced and compute best discriminating bit index.
 *
 * @param hist Histogram.
 * @param index Location of ternary bit index.
 *
 * @return Boolean true if key set is reducible and false otherwise.
 */
bool tkey_sparse_hist_disc(tkey_sparse_hist_t *hist, size_t *index);

/**
 * @brief Return type of histogram.
 *
 * @param hist Hostogram to retrieve type from.
 *
 * @return Type of key.
 */
ttype_t *tkey_sparse_hist_type(tkey_sparse_hist_t *hist);

/**
 *
 */
void tkey_sparse_preamble(tkey_sparse_t *key, char *pre);


#endif /* TKEY_SPARSE */
