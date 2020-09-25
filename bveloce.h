#ifndef BVELOCE_H__
#define BVELOCE_H__

#include <inttypes.h>

typedef struct bv_bigint bv_bigint_type;

size_t get_word_num_of_bits(void);

bv_bigint_type *bv_create(void);

void bv_destroy(bv_bigint_type *bv);

uintmax_t *bv_set_word(bv_bigint_type *bv,
                       const size_t idx, const uintmax_t word);

uintmax_t *bv_get_word(bv_bigint_type *bv, const size_t idx);

bv_bigint_type *bv_not(bv_bigint_type *bv);

bv_bigint_type *bv_and_bv(bv_bigint_type *bv1, const bv_bigint_type *bv2);

bv_bigint_type *bv_or_bv(bv_bigint_type *bv1, const bv_bigint_type *bv2);

bv_bigint_type *bv_xor_bv(bv_bigint_type *bv1, const bv_bigint_type *bv2);

bv_bigint_type *bv_shr(bv_bigint_type *bv, const size_t n);

bv_bigint_type *bv_shl(bv_bigint_type *bv, const size_t n);

#endif
