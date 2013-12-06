


#ifndef H_AES_LEGACY
#define H_AES_LEGACY

#define CUSTOM_AES_256              "CUSTOM_AES_256"
#define AES_KEY_SIZE                (32) // bytes
#define CIPHER_BLOCK_SIZE           (16)

extern int aes_legacy_enc(unsigned char* in_buf,  unsigned int in_len, unsigned char* out_buf, unsigned int out_len);
extern int aes_legacy_dec(unsigned char* in_buf,  unsigned int in_len, unsigned char* out_buf, unsigned int out_len);
extern int aes_legacy_init_key(unsigned char* key_buf,  unsigned int key_len);
extern int aes_legacy_init_vector(void);

#endif
