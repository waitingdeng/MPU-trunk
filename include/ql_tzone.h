#ifndef __QL_TZONE_H__
#define __QL_TZONE_H__

#include <linux/ioctl.h>

#define QL_TRUSTZONE_DEV    "/dev/tzone"

#define QL_TZ_CRYPTO_SVC_IOC_MAGIC  't'
#define QL_TZ_CRYPTO_SVC_IOC_CMD(id) _IOWR(QL_TZ_CRYPTO_SVC_IOC_MAGIC, id, int)


#define QL_CRYPTO_ASYM_KEY_SIZE_MAX     (512+16) /* 4096 bits */
#define QL_CRYPTO_ASYM_IV_LENGTH        (16)     /* AES128 CBC IV */
#define QL_CRYPTO_ASYM_HMAC_LENGTH      (32)     /* SHA2 will be used for HMAC */

typedef enum
{
    QL_TZ_RSA_MODULUS_1024_BITS = 128,
    QL_TZ_RSA_MODULUS_2048_BITS = 256,
} ql_tz_rsa_modulus_size_t;

typedef enum
{
    QL_TZ_RSA_DIGEST_PADDING_NONE  = 0,//not support for sign data
    QL_TZ_RSA_PKCS115_SHA2_256     = 1,
    QL_TZ_RSA_PSS_SHA2_256         = 2,
    QL_TZ_RSA_MAX_VAL              = 0x7FFFFFFF,
} ql_tz_crypto_rsa_digest_pad_t;

/** Random Data Gen PARAS */
typedef struct
{
    unsigned int  data_size;  /* in, req random data size(equal data buff size),  valid range:[1, 512];
                           out, randome data size */
    void    *data_ptr;  //out, random data
} ql_tz_random_data_t;

/** Gen AES Key */
typedef struct
{
    unsigned int  key_size;   //in: only support 256 (bit). out: alway 56 (byte)
    void    *key_ptr;   //buff size must be >= 56 bytes
} ql_tz_aes_generate_key_t;


/** Import AES Key */
typedef struct
{
    unsigned int  input_aes_size; //in, aes key size.only support 32 bytes(256 bits)
    void    *input_aes_ptr; //in, aes key

    unsigned int  key_size;       //in, key buff size; out, if success ,must be equal 56 bytes
    void    *key_ptr;       //out, out tz key blob
} ql_tz_aes_import_key_t;

/** Encrpyt/Decrypt Data */
typedef struct
{
    unsigned int  key_size;           //in, must be 56 (bytes)
    void    *key_ptr;           //in, key
    unsigned int  input_data_size;    //in, input data size0x800,to be test
    void    *input_data_ptr;    //in, input data

    unsigned int  output_data_size;   //in, out data's buff size; out, output data size
    void    *output_data_ptr;   //out, out data
} ql_tz_aes_endecrypt_data_t;


typedef struct
{
    /* Validate the integrity of keyblob content */
    unsigned int  magic_num;

    /* API version number */
    unsigned int  version;

    ql_tz_crypto_rsa_digest_pad_t  digest_padding;

    /* Modulus (N) array [MSB...LSB] */
    unsigned char   modulus[QL_CRYPTO_ASYM_KEY_SIZE_MAX];

    /* Modulus array length */
    unsigned int  modulus_size;

    /* public exponent (e) array [MSB...LSB] */
    unsigned char   public_exponent[QL_CRYPTO_ASYM_KEY_SIZE_MAX];

    /* public exponent array length */
    unsigned int  public_exponent_size;

    /* Initial vector */
    unsigned char   iv[QL_CRYPTO_ASYM_IV_LENGTH];

    /* Encrypted Private Exponent (d) array [MSB...LSB] */
    unsigned char   encrypted_private_exponent[QL_CRYPTO_ASYM_KEY_SIZE_MAX];

    /* Encrypted Private Exponent array length */
    unsigned int  encrypted_private_exponent_size;

    /* HMAC array */
    unsigned char   hmac[QL_CRYPTO_ASYM_HMAC_LENGTH];
} ql_crypto_rsa_key_blob_t;

typedef struct
{
    unsigned int modulus_size;//in
    unsigned long long public_exponent;//in
    ql_tz_crypto_rsa_digest_pad_t digest_pad_type;//in

    ql_crypto_rsa_key_blob_t *rsa_key_blob;//out
} ql_tz_rsa_generate_key_t;

typedef struct
{
    ql_crypto_rsa_key_blob_t *rsa_key_blob;  //in, rsa key blob
    unsigned int  input_data_size;                //in, input data size. 0x800
    void    *input_data_ptr;                //in, input data
    unsigned int  signature_length;               //out, return sign data size
    void    *signature_ptr;                 //out, return sign data
} ql_tz_rsa_sign_verify_data_t;


typedef struct
{
    unsigned char *in_modulus;
    unsigned int in_modulusSize;
    unsigned char *in_pubExp;
    unsigned int in_pubExpSize;
    unsigned char *in_privExp;
    unsigned int in_privExpSize;
    ql_tz_crypto_rsa_digest_pad_t in_padding;
    ql_crypto_rsa_key_blob_t *rsa_key_blob;//out
} ql_tz_rsa_keypair_import_t;


//not support
typedef struct tz_rsa_pkcs8_import
{
    unsigned int *in_pkcs8_key;     // only support DER format
    unsigned int in_pkcs8_key_len;
    ql_crypto_rsa_key_blob_t *rsa_key_blob;//out
} ql_tz_rsa_pkcs8_import_t;


typedef struct
{
    ql_crypto_rsa_key_blob_t *rsa_key_blob; //in
    unsigned int  modulus_data_size;      //in, modulues data buff size; out, modulus data size
    void    *modulus_data_ptr;      //out, modulus data
    unsigned int  public_exponent_size;   //in, public exponent buff size; out, public exponent size
    void    *public_exponent_ptr;   //out, public exponent
} ql_tz_rsa_public_key_export_t;

typedef struct
{
    unsigned int modulus_size; // bytes
    unsigned char modulus_data[256 + 32];
    unsigned int public_exponent_size; // bytes
    unsigned char public_exponent[32];
} rsa_public_key_t;

typedef enum
{
    QL_TZ_CRYPTO_SVC_RANDOM_DATA_GEN        = QL_TZ_CRYPTO_SVC_IOC_CMD(0),
    QL_TZ_CRYPTO_SVC_AES_KEY_GEN            = QL_TZ_CRYPTO_SVC_IOC_CMD(1),
    QL_TZ_CRYPTO_SVC_AES_KEY_IMPORT         = QL_TZ_CRYPTO_SVC_IOC_CMD(2),
    QL_TZ_CRYPTO_SVC_AES_DATA_ENCRYPT       = QL_TZ_CRYPTO_SVC_IOC_CMD(3),
    QL_TZ_CRYPTO_SVC_AES_DATA_DECRYPT       = QL_TZ_CRYPTO_SVC_IOC_CMD(4),
    QL_TZ_CRYPTO_SVC_RSA_KEYPAIR_GEN        = QL_TZ_CRYPTO_SVC_IOC_CMD(5),
    QL_TZ_CRYPTO_SVC_RSA_DATA_SIGN          = QL_TZ_CRYPTO_SVC_IOC_CMD(6),
    QL_TZ_CRYPTO_SVC_RSA_SINDATA_VERIFY     = QL_TZ_CRYPTO_SVC_IOC_CMD(7),
    QL_TZ_CRYPTO_SVC_RSA_KEYPAIR_IMPORT     = QL_TZ_CRYPTO_SVC_IOC_CMD(8),
    QL_TZ_CRYPTO_SVC_RSA_PKCS8_IMPORT       = QL_TZ_CRYPTO_SVC_IOC_CMD(9),// unsupport
    QL_TZ_CRYPTO_SVC_RSA_PUBKEY_EXPORT      = QL_TZ_CRYPTO_SVC_IOC_CMD(10),
} ql_tz_crypto_svc_t;
#endif
