#include <stdlib.h>
#include <inttypes.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "bveloce.h"

struct bv_bigint {
    size_t word_count;
    uintmax_t *words;
};

size_t get_word_len_bits(void)
{
    static size_t word_len_bits = 0;
    static bool done = false;

    if (done) {
        return word_len_bits;
    }

    size_t w = 0;
    for (uintmax_t n = UINTMAX_MAX; n > 0; n >>= 1) {
        ++w;
    }
    word_len_bits = w;
    done = true;

    return word_len_bits;
}

bv_bigint_type *bv_create(void)
{
    bv_bigint_type *new = malloc(sizeof *new);

    if (!new) {
        return NULL;
    }

    if (!(new->words = calloc(new->word_count = 1, sizeof *new->words))) {
        free(new);
        return NULL;
    }

    return new;
}

void bv_destroy(bv_bigint_type *bv)
{
    if (bv) {
        free(bv->words);
        free(bv);
    }
}

size_t bv_get_word_count(bv_bigint_type *bv)
{
    return bv->word_count;
}

uintmax_t *bv_set_word(bv_bigint_type *bv,
                       const size_t idx, const uintmax_t word)
{
    uintmax_t *p = NULL;

    if (idx == SIZE_MAX) {
        return NULL;
    }

    if (idx >= bv->word_count) {
        if (!(p = realloc(bv->words, (idx + 1) * sizeof *p))) {
            return NULL;
        }

        bv->word_count = idx + 1;
        bv->words = p;
    }

    *(p = bv->words + idx) = word;
    return p;
}

uintmax_t *bv_get_word(bv_bigint_type *bv, const size_t idx)
{
    if (idx >= bv->word_count) {
        return NULL;
    }

    return bv->words + idx;
}

bv_bigint_type *bv_not(bv_bigint_type *bv)
{
    size_t word_count = bv->word_count;

    for (size_t i = 0; i < word_count; ++i) {
        bv->words[i] = ~(bv->words[i]);
    }

    return bv;
}

bv_bigint_type *bv_and_bv(bv_bigint_type *bv1, const bv_bigint_type *bv2)
{
    size_t word_count = bv1->word_count > bv2->word_count
                        ? bv2->word_count
                        : bv1->word_count;

    for (size_t i = 0; i < word_count; ++i) {
        bv1->words[i] &= bv2->words[i];
    }

    return bv1;
}

bv_bigint_type *bv_or_bv(bv_bigint_type *bv1, const bv_bigint_type *bv2)
{
    size_t max_word_count = bv1->word_count > bv2->word_count
                            ? bv1->word_count
                            : bv2->word_count;
    if (bv2->word_count > bv1->word_count) {
        if (!(bv_set_word(bv1, max_word_count - 1,
                          bv2->words[max_word_count - 1]))) {
            return NULL;
        }

        --max_word_count;
    }

    for (size_t i = 0; i < max_word_count; ++i) {
        bv1->words[i] |= bv2->words[i];
    }

    return bv1;
}

bv_bigint_type *bv_xor_bv(bv_bigint_type *bv1, const bv_bigint_type *bv2)
{
    size_t max_word_count = bv1->word_count > bv2->word_count
                            ? bv1->word_count
                            : bv2->word_count;

    if (bv2->word_count > bv1->word_count) {
        if (!(bv_set_word(bv1, max_word_count - 1,
                          bv2->words[max_word_count - 1]))) {
            return NULL;
        }

        --max_word_count;
    }

    for (size_t i = 0; i < max_word_count; ++i) {
        bv1->words[i] ^= bv2->words[i];
    }

    return bv1;
}

bv_bigint_type *bv_shr(bv_bigint_type *bv, const size_t n)
{
    size_t word_len_bits = get_word_len_bits();
    uintmax_t *b = bv->words;

    /* if shifting by more bits available, just zero out */
    if (n >= bv->word_count * word_len_bits) {
        memset(b, 0, bv->word_count * sizeof *b);
        return bv;
    }

    size_t w = n / word_len_bits;
    size_t u = bv->word_count - w;
    size_t i = 0;
    while (i < bv->word_count) {
        *b = i++ < u ? *(b + w) : 0;
        ++b;
    }

    if (u == 0) {
        return bv;
    }

    printf("178: %016jx ", bv->words[1]);
    printf("%016jx\n", bv->words[0]);

    size_t modl = n % word_len_bits;
    if (modl == 0) {
        return bv;
    }

    size_t modh = word_len_bits - modl;
    for (i = 0, b = bv->words + w; i < u - 1; ++i, ++b) {
        *b >>= modl;
        *b |= *(b + 1) << modh;
    }
    *b = 0;

    return bv;
}

bv_bigint_type *bv_shl(bv_bigint_type *bv, const size_t n)
{
    size_t word_len_bits = get_word_len_bits();
    size_t m = n % word_len_bits;
    size_t w = n / word_len_bits;
    size_t u = bv->word_count;

    if (!(bv_set_word(bv, bv->word_count + w + 1, 0))) {
        return NULL;
    }

    u -= w;
    memmove(bv->words + w, bv->words, u);
    memset(bv->words, 0, w);

    uintmax_t mask = ~(UINTMAX_MAX >> m);
    for (size_t i = u; i >= w; --i) {
        bv->words[i] = (bv->words[i - 1] & mask) >> m |
                       (bv->words[i] << m);

        if (w == 0) {
            break;
        }
    }

    return bv;
}

#ifdef BVELOCE_TEST__
int main(void)
{
    int ret_val = EXIT_FAILURE;
    bv_bigint_type *bv = bv_create();
    bv_bigint_type *bv_op = bv_create();

    if (!bv || !bv_op) {
        fputs("Failed creating objects\n", stderr);
        goto bail_out;
    }

    if (!bv_set_word(bv, 0, UINTMAX_MAX)
        || !bv_set_word(bv, 1, UINTMAX_MAX >> 2)) {
        fputs("Failed setting word\n", stderr);
        goto bail_out;
    }

    printf("%016jx ", *bv_get_word(bv, 1));
    printf("%016jx\n", *bv_get_word(bv, 0));
    bv_shr(bv, 63);
    printf("%016jx ", *bv_get_word(bv, 1));
    printf("%016jx\n", *bv_get_word(bv, 0));

    ret_val = EXIT_SUCCESS;

    bail_out:
    bv_destroy(bv_op);
    bv_destroy(bv);

    return ret_val;
}
#endif
