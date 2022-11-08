#include <hf-risc.h>
// #include "DES.h"

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

// 3DES encryption algorithm

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

int main(void){
	uint32_t cycles;
	uint32_t _3des_key[6] = {0xf0e1d2c3, 0xb4a59687, 0x78695a4b, 0x3c2d1e0f, 0xb4a59687, 0xf0e1d2c3};
	uint32_t msg[2] = {0x12345678, 0x90123456};

	/* devemos manter?
	des_ctx dc; // Key schedule structure
   	unsigned char *cp;
   	unsigned char data[8]   = {0x12, 0x34, 0x56, 0x78, 0x90, 0x12, 0x34, 0x56};

   	unsigned char key[8]   = {0xf0,0xe1,0xd2,0xc3,0xb4,0xa5,0x96,0x87};
   	unsigned char key1[8]  = {0x78,0x69,0x5a,0x4b,0x3c,0x2d,0x1e,0x0f};
   	unsigned char key2[8]  = {0xb4,0xa5,0x96,0x87,0xf0,0xe1,0xd2,0xc3};
	cp = data;

	printf("message: %8x%8x\n", msg[0], msg[1]);
	cycles = TIMER0;
	TripleDES_ENC( &dc, cp, 1, key, key1, key2);   // 3DES Encrypt
	cycles = TIMER0 - cycles;
	printf("encipher: %02x%02x%02x%02x%02x%02x%02x%02x, %d cycles\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], cycles);
	cycles = TIMER0;
	TripleDES_DEC( &dc, cp, 1, key, key1, key2);   // 3DES Decrypt
	cycles = TIMER0 - cycles;
	printf("decipher: %02x%02x%02x%02x%02x%02x%02x%02x, %d cycles\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], cycles);
	*/

	printf("message: %8x%8x\n", msg[0], msg[1]);
	
	init_3des(_3DES_ENCRYPT);
	set_key(_3des_key);
	
	cycles = TIMER0;
	encrypt(msg);
	cycles = TIMER0 - cycles;
	printf("encipher: %8x%8x, %d cycles\n", msg[0], msg[1], cycles);

	init_3des(_3DES_DECRYPT);
	set_key(_3des_key);
	
	cycles = TIMER0;
	encrypt(msg);
	cycles = TIMER0 - cycles;
	printf("decipher: %8x%8x, %d cycles\n", msg[0], msg[1], cycles);

	return 0;
}
