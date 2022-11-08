#include <hf-risc.h>

#define _3DES_BASE			0xe7000000
#define _3DES_KEY0			(*(volatile uint32_t *)(_3DES_BASE + 0x000))
#define _3DES_KEY1			(*(volatile uint32_t *)(_3DES_BASE + 0x010))
#define _3DES_KEY2			(*(volatile uint32_t *)(_3DES_BASE + 0x020))
#define _3DES_KEY3			(*(volatile uint32_t *)(_3DES_BASE + 0x030))
#define _3DES_KEY4			(*(volatile uint32_t *)(_3DES_BASE + 0x040))
#define _3DES_KEY5			(*(volatile uint32_t *)(_3DES_BASE + 0x050))
#define _3DES_IN0			(*(volatile uint32_t *)(_3DES_BASE + 0x060))
#define _3DES_IN1			(*(volatile uint32_t *)(_3DES_BASE + 0x070))
#define _3DES_CONTROL		(*(volatile uint32_t *)(_3DES_BASE + 0x080))
#define _3DES_OUT0			(*(volatile uint32_t *)(_3DES_BASE + 0x090))
#define _3DES_OUT1			(*(volatile uint32_t *)(_3DES_BASE + 0x0a0))

#define _3DES_ENCRYPT		(1 << 0)
#define _3DES_DECRYPT		(0 << 0)
#define _3DES_DATA_READY	(1 << 1)
#define _3DES_KEY_READY		(1 << 2)
#define _3DES_RESET			(1 << 3)
#define _3DES_READY			(1 << 4)


void set_key(const uint32_t key[6]) {
	_3DES_KEY0 = key[0];
	_3DES_KEY1 = key[1];
	_3DES_KEY2 = key[2];
	_3DES_KEY3 = key[3];
	_3DES_KEY4 = key[4];
	_3DES_KEY5 = key[5];
	_3DES_CONTROL |= _3DES_KEY_READY;
	_3DES_CONTROL &= ~_3DES_KEY_READY;
}

void encrypt(uint32_t v[2]) {

	_3DES_IN0 = v[0];
	_3DES_IN1 = v[1];
	_3DES_CONTROL |= _3DES_DATA_READY;

	while (!(_3DES_CONTROL & _3DES_READY));
	_3DES_CONTROL &= ~_3DES_DATA_READY;

	v[0] = _3DES_OUT0;
	v[1] = _3DES_OUT1;
}

void init_3des(int mode){
	_3DES_CONTROL = mode;
	_3DES_CONTROL |= _3DES_RESET;
	_3DES_CONTROL &= ~_3DES_RESET;
}


/* 3DES stream cipher, CBC mode
 * CBC mode based on https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation
 */

#define BLOCKLEN	8		// in bytes

void _3des_cbc_encrypt(uint8_t *out, uint8_t *in, uint32_t len, const uint32_t iv[2])
{
	uint32_t i, rem, block[2], tiv[2];
	
	rem = len % BLOCKLEN;
	tiv[0] = iv[0];
	tiv[1] = iv[1];
	for (i = 0; i < len; i += BLOCKLEN) {
		memcpy((char *)block, in, BLOCKLEN);
		block[0] ^= tiv[0];
		block[1] ^= tiv[1];
		encrypt(block);
		tiv[0] = block[0];
		tiv[1] = block[1];
		memcpy(out, (char *)block, BLOCKLEN);
		in += BLOCKLEN;
		out += BLOCKLEN;
	}
	if (rem) {
		memcpy((char *)block, in, BLOCKLEN - rem);
		memset((char *)block + rem, 0, BLOCKLEN - rem);
		block[0] ^= tiv[0];
		block[1] ^= tiv[1];
		encrypt(block);
		memcpy(out, (char *)block, BLOCKLEN - rem);
	}
}


void _3des_cbc_decrypt(uint8_t *out, uint8_t *in, uint32_t len, const uint32_t iv[2])
{
	uint32_t i, rem, block[2], block2[2], tiv[2];
	
	rem = len % BLOCKLEN;
	tiv[0] = iv[0];
	tiv[1] = iv[1];
	for (i = 0; i < len; i += BLOCKLEN) {
		memcpy((char *)block, in, BLOCKLEN);
		block2[0] = block[0];
		block2[1] = block[1];
		encrypt(block);
		block[0] ^= tiv[0];
		block[1] ^= tiv[1];
		tiv[0] = block2[0];
		tiv[1] = block2[1];
		memcpy(out, (char *)block, BLOCKLEN);
		in += BLOCKLEN;
		out += BLOCKLEN;
	}
	if (rem) {
		memcpy((char *)block, in, BLOCKLEN - rem);
		memset((char *)block + rem, 0, BLOCKLEN - rem);
		tiv[0] = block[0];
		tiv[1] = block[1];
		encrypt(block);
		block[0] ^= tiv[0];
		block[1] ^= tiv[1];
		memcpy(out, (char *)block, BLOCKLEN - rem);
	}
}

/* 3DES stream cipher, CTR mode
 * CTR mode based on https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation
 */
#define BLOCKLEN	8		// in bytes

void _3des_ctr_crypt(uint8_t *out, uint8_t *in, uint32_t len, const uint32_t nonce[2])
{
	uint32_t i, k, rem;
	uint32_t data[2];
	uint8_t *data2 = (uint8_t *)&data;
	uint64_t counter = 0;
	uint32_t *in_p = (uint32_t *)in;
	uint32_t *out_p = (uint32_t *)out;
	
	rem = len % BLOCKLEN;
	for (i = 0; i < len; i += BLOCKLEN) {
		data[0] = nonce[0] ^ (counter & 0xffffffff);
		data[1] = nonce[1] ^ (counter >> 32);
		encrypt(data);
		out_p[0] = in_p[0] ^ data[0];
		out_p[1] = in_p[1] ^ data[1];
		in_p += 2;
		out_p += 2;
		counter++;
	}
	
	in = (uint8_t *)in_p;
	out = (uint8_t *)out_p;
	
	if (rem) {
		data[0] = nonce[0] ^ (counter & 0xffffffff);
		data[1] = nonce[1] ^ (counter >> 32);		
		encrypt(data);
		for (k = 0; k < rem; k++)
			out[k] = in[k] ^ data2[k];
	}
}

int main(void){
	uint8_t message[] = "the quick brown fox jumps over the lazy dog";
	uint32_t _3des_key[6] = {0xf0e1d2c3, 0xb4a59687, 0x78695a4b, 0x3c2d1e0f, 0xb4a59687, 0xf0e1d2c3};
	uint32_t iv[2] = {0x11223344, 0x55667788};
	
	printf("\nmessage:");
	hexdump((char *)message, sizeof(message));
	
	init_3des(_3DES_ENCRYPT);
	set_key(_3des_key);
	_3des_cbc_encrypt(message, message, sizeof(message), iv);
	printf("\nencoded message (CBC mode):");
	hexdump((char *)message, sizeof(message));

	init_3des(_3DES_DECRYPT);
	set_key(_3des_key);
	_3des_cbc_decrypt(message, message, sizeof(message), iv);
	printf("\ndecoded message (CBC mode):");
	hexdump((char *)message, sizeof(message));

	
	init_3des(_3DES_ENCRYPT);
	set_key(_3des_key);
	_3des_ctr_crypt(message, message, sizeof(message), iv);
	printf("\nencoded message (CTR mode):");
	hexdump((char *)message, sizeof(message));

	_3des_ctr_crypt(message, message, sizeof(message), iv);
	printf("\ndecoded message (CTR mode):");
	hexdump((char *)message, sizeof(message));

	return 0;
}
