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
#define _3DES_READY			(1 << 3)

/*
3DES encryption algorithm

based on reference code released into the public domain by David Wheeler and Roger Needham
the code takes 64 bits of data in v[0] and v[1] and 128 bits of key in key[0] - key[3]

recommended number of rounds is 32 (2 Feistel-network rounds are performed on each iteration).
*/

const uint32_t _3des_key[6] = {0xf0e1d2c3, 0xb4a59687, 0x78695a4b, 0x3c2d1e0f, 0xb4a59687, 0xf0e1d2c3};

void encipher(uint32_t num_rounds, uint32_t v[2], uint32_t const key[4]){
	uint32_t i;
	uint32_t v0 = v[0], v1 = v[1], sum = 0, delta = 0x9E3779B9;

	for (i = 0; i < num_rounds; i++){
		v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
		sum += delta;
		v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
	}
	v[0] = v0; v[1] = v1;
}

void decipher(uint32_t num_rounds, uint32_t v[2], uint32_t const key[4]){
	uint32_t i;
	uint32_t v0 = v[0], v1 = v[1], delta = 0x9E3779B9, sum = delta * num_rounds;

	for (i = 0; i < num_rounds; i++){
		v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
		sum -= delta;
		v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
	}
	v[0] = v0; v[1] = v1;
}

int main(void){
	uint32_t msg[2] = {0x12345678, 0x90123456};
	uint32_t cycles;

	printf("message: %8x%8x\n", msg[0], msg[1]);
	cycles = TIMER0;
	encipher(32, msg, _3des_key);
	cycles = TIMER0 - cycles;
	printf("encipher: %8x%8x, %d cycles\n", msg[0], msg[1], cycles);
	cycles = TIMER0;
	decipher(32, msg, _3des_key);
	cycles = TIMER0 - cycles;
	printf("decipher: %8x%8x, %d cycles\n", msg[0], msg[1], cycles);

	printf("message: %8x%8x\n", msg[0], msg[1]);

	_3DES_CONTROL = _3DES_ENCRYPT;

	_3DES_KEY0 = _3des_key[0];
	_3DES_KEY1 = _3des_key[1];
	_3DES_KEY2 = _3des_key[2];
	_3DES_KEY3 = _3des_key[3];
	_3DES_KEY4 = _3des_key[4];
	_3DES_KEY5 = _3des_key[5];
	_3DES_CONTROL |= _3DES_KEY_READY;

	cycles = TIMER0;
	_3DES_IN0 = msg[0];
	_3DES_IN1 = msg[1];
	_3DES_CONTROL |= _3DES_DATA_READY;

	while (!(_3DES_CONTROL & _3DES_READY)); // esperando

	_3DES_CONTROL &= ~_3DES_DATA_READY; // desligou

	cycles = TIMER0 - cycles;

	_3DES_CONTROL = _3DES_DECRYPT;
	
	msg[0] = _3DES_OUT0;
	msg[1] = _3DES_OUT1;
	_3DES_CONTROL |= _3DES_DATA_READY;

	printf("encipher: %8x%8x, %d cycles\n", msg[0], msg[1], cycles);

	cycles = TIMER0;
	_3DES_IN0 = msg[0];
	_3DES_IN1 = msg[1];
	while (!(_3DES_CONTROL & _3DES_READY));

	_3DES_CONTROL &= ~_3DES_KEY_READY;
	_3DES_CONTROL &= ~_3DES_DATA_READY; // desligou

	cycles = TIMER0 - cycles;

	msg[0] = _3DES_OUT0;
	msg[1] = _3DES_OUT1;

	printf("decipher: %8x%8x, %d cycles\n", msg[0], msg[1], cycles);

	return 0;
}
