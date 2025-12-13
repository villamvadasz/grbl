/*
 * Interface of curve P-256 (ECDH and ECDSA)
 *
 * Author: Manuel Pégourié-Gonnard.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef P256_M_H
#define P256_M_H

#include <stdint.h>
#include <stddef.h>

#define E_OK	0
#define E_NOT_OK	1
#define E_BUSY	2


/* Status codes */
#define P256_SUCCESS            0
#define P256_RANDOM_FAILED      -1
#define P256_INVALID_PUBKEY     -2
#define P256_INVALID_PRIVKEY    -3
#define P256_INVALID_SIGNATURE  -4
#define P256_INVALID_STATE  	-5

#ifdef __cplusplus
extern "C" {
#endif

/*
 * RNG function - must be provided externally and be cryptographically secure.
 *
 * in: output - must point to a writable buffer of at least output_size bytes.
 *     output_size - the number of random bytes to write to output.
 * out: output is filled with output_size random bytes.
 *      return 0 on success, non-zero on errors.
 */
extern int p256_generate_random(uint8_t * output, unsigned output_size);

/*
 * ECDH/ECDSA generate key pair
 *
 * [in] draws from p256_generate_random()
 * [out] priv: on success, holds the private key, as a big-endian integer
 * [out] pub: on success, holds the public key, as two big-endian integers
 *
 * return:  P256_SUCCESS on success
 *          P256_RANDOM_FAILED on failure
 */
int p256_gen_keypair_start(void);
int p256_gen_keypair_update(void);
int p256_gen_keypair_stop(uint8_t priv[32], uint8_t pub[64], int *ret);
int p256_gen_keypair(uint8_t priv[32], uint8_t pub[64]);

/*
 * ECDH compute shared secret
 *
 * [out] secret: on success, holds the shared secret, as a big-endian integer
 * [in] priv: our private key as a big-endian integer
 * [in] pub: the peer's public key, as two big-endian integers
 *
 * return:  P256_SUCCESS on success
 *          P256_INVALID_PRIVKEY if priv is invalid
 *          P256_INVALID_PUBKEY if pub is invalid
 */
unsigned int p256_ecdh_shared_secret_start(const uint8_t priv[32], const uint8_t peer[64]);
unsigned int p256_ecdh_shared_secret_update(void);
unsigned int p256_ecdh_shared_secret_stop(uint8_t secret[32], int *ret);
int p256_ecdh_shared_secret(uint8_t secret[32], const uint8_t priv[32], const uint8_t pub[64]);

/*
 * ECDSA sign
 *
 * [in] draws from p256_generate_random()
 * [out] sig: on success, holds the signature, as two big-endian integers
 * [in] priv: our private key as a big-endian integer
 * [in] hash: the hash of the message to be signed
 * [in] hlen: the size of hash in bytes
 *
 * return:  P256_SUCCESS on success
 *          P256_RANDOM_FAILED on failure
 *          P256_INVALID_PRIVKEY if priv is invalid
 */
int p256_ecdsa_sign_start(const uint8_t priv[32], const uint8_t *hash, size_t hlen);
int p256_ecdsa_sign_update(void);
int p256_ecdsa_sign_stop(uint8_t sig[64], int *ret);
int p256_ecdsa_sign(uint8_t sig[64], const uint8_t priv[32], const uint8_t *hash, size_t hlen);

/*
 * ECDSA verify
 *
 * [in] sig: the signature to be verified, as two big-endian integers
 * [in] pub: the associated public key, as two big-endian integers
 * [in] hash: the hash of the message that was signed
 * [in] hlen: the size of hash in bytes
 *
 * return:  P256_SUCCESS on success - the signature was verified as valid
 *          P256_INVALID_PUBKEY if pub is invalid
 *          P256_INVALID_SIGNATURE if the signature was found to be invalid
 */
int p256_ecdsa_verify_start(const uint8_t sig[64], const uint8_t pub[64], const uint8_t *hash, size_t hlen);
int p256_ecdsa_verify_update(void);
int p256_ecdsa_verify_stop(int *ret);
int p256_ecdsa_verify(const uint8_t sig[64], const uint8_t pub[64], const uint8_t *hash, size_t hlen);

#ifdef __cplusplus
}
#endif

#endif /* P256_M_H */
