/*
 * FIPS-197 AES-128 software library source file
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
 *
 * License
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "aes.h"

/* Global Variables declaration*/

/*  S-box in hexadecimal format */
static unsigned char sbox[256] =   {
	//0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,  //0
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,  //1
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,  //2
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,  //3
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,  //4
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,  //5
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,  //6
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,  //7
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,  //8
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,  //9
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,  //A
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,  //B
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,  //C
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,  //D
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,  //E
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16   //F
};

/* Inverse S-box in hexadecimal format */
static unsigned char inv_sbox[256] = {
	//0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,  //0
	0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,  //1
	0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,  //2
	0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,  //3
	0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,  //4
	0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,  //5
	0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,  //6
	0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,  //7
	0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,  //8
	0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,  //9
	0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,  //A
	0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,  //B
	0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,  //C
	0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,  //D
	0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,  //E
	0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d   //F
};

/* Round constant for Key Expansion */
static unsigned char RCon[11] = {
	0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

static unsigned char xtime(unsigned char value);
static void SubBytes_ShiftRows(unsigned char *state);
static void MixColumnns(unsigned char *state);
static void AddRoundKey(unsigned char *state, unsigned char round, unsigned char * keySchedule);
static void InvShiftRows_SubBytes(unsigned char *state);
static void InvMixColumns(unsigned char *state);

/*
 ********************************************************************************
 * Performs key expansion to generate key schedule from							*
 * initial set of Key of 4 words.												*
 * roundKey  Holds the pointer to store key schedule							*
 *			 First 16 bytes hold the initial key								*
 ********************************************************************************
 */
void KeyExpansion(unsigned char *roundKey) {
unsigned char i, temp;

	// Compute Key schedule of block size for each round
	for ( i = 1; i < (AES_NUM_OF_ROUNDS + 1); i++) {
		temp = roundKey[ i*16 - 4 ];
		roundKey[i*16 +  0] = sbox[ roundKey[i*16 - 3] ] ^ roundKey[(i-1)*16 + 0] ^ RCon[ i ];
		roundKey[i*16 +  1] = sbox[ roundKey[i*16 - 2] ] ^ roundKey[(i-1)*16 + 1];
		roundKey[i*16 +  2] = sbox[ roundKey[i*16 - 1] ] ^ roundKey[(i-1)*16 + 2];
		roundKey[i*16 +  3] = sbox[ temp ]				 ^ roundKey[(i-1)*16 + 3];

		roundKey[i*16 +  4] = roundKey[(i-1)*16 + 4] ^ roundKey[i*16 + 0];
		roundKey[i*16 +  5] = roundKey[(i-1)*16 + 5] ^ roundKey[i*16 + 1];
		roundKey[i*16 +  6] = roundKey[(i-1)*16 + 6] ^ roundKey[i*16 + 2];
		roundKey[i*16 +  7] = roundKey[(i-1)*16 + 7] ^ roundKey[i*16 + 3];

		roundKey[i*16 +  8] = roundKey[(i-1)*16 + 8] ^ roundKey[i*16 + 4];
		roundKey[i*16 +  9] = roundKey[(i-1)*16 + 9] ^ roundKey[i*16 + 5];
		roundKey[i*16 + 10] = roundKey[(i-1)*16 +10] ^ roundKey[i*16 + 6];
		roundKey[i*16 + 11] = roundKey[(i-1)*16 +11] ^ roundKey[i*16 + 7];

		roundKey[i*16 + 12] = roundKey[(i-1)*16 +12] ^ roundKey[i*16 + 8];
		roundKey[i*16 + 13] = roundKey[(i-1)*16 +13] ^ roundKey[i*16 + 9];
		roundKey[i*16 + 14] = roundKey[(i-1)*16 +14] ^ roundKey[i*16 +10];
		roundKey[i*16 + 15] = roundKey[(i-1)*16 +15] ^ roundKey[i*16 +11];
	}	// for ( i = 1; i < (AES_NUM_OF_ROUNDS + 1); i++)
}	// void KeyExpansion(unsigned char *roundKey)

/*
 ********************************************************************************
 * Performs key expansion to generate key schedule from							*
 * initial set of Key of 4 words.												*
 * roundKey  Holds the pointer to store key schedule							*
 * key       Initial Key received from the user									*
 ********************************************************************************
 */
void KeyScheduler(unsigned char *roundKey, unsigned char *key) {
unsigned char i;
	// Retain the initial for round 0
	for (i = 0; i < 16; i++) {
		roundKey[i] = key[i];
	}
	KeyExpansion(roundKey);
}	// void KeyScheduler(unsigned char *roundKey, unsigned char *key)

/*
 ********************************************************************************
 * xtime calculation															*
 * The value is multiplied by 0x02. This is implemented at the byte level as a	*
 * left shift and XOR with 0x1b if the MSB of the value (before) shifting is 1.	*
 * value  Value for which multiplication to be done								*
 ********************************************************************************
 */
static unsigned char xtime(unsigned char value) {
	if ( value >> 7 ) {
		value = value << 1;
		return ( value ^ 0x1b );
	} else
		return value << 1;
}	// unsigned char xtime(unsigned char value)

/*
 ********************************************************************************
 * Performs Sub Byte and Shift rows operation.									*
 * state  variable to store the intermediate result of the algorithm			*
 ********************************************************************************
 */
static void SubBytes_ShiftRows(unsigned char *state) {
unsigned char temp1, temp2;

	// row 0
	state[0]  = sbox[ state[0] ];
	state[4]  = sbox[ state[4] ];
	state[8]  = sbox[ state[8] ];
	state[12] = sbox[ state[12] ];

	// row 1
	temp1	  = sbox[ state[1] ];
	state[1]  = sbox[ state[5] ];
	state[5]  = sbox[ state[9] ];
	state[9]  = sbox[ state[13]];
	state[13] = temp1;

	// row 2
	temp1	  = sbox[ state[2]];
	temp2	  = sbox[ state[6]];
	state[2]  = sbox[ state[10] ];
	state[6]  = sbox[ state[14] ];
	state[10] = temp1;
	state[14] = temp2;

	// row 3
	temp1	  = sbox[ state[15] ];
	state[15] = sbox[ state[11] ];
	state[11] = sbox[ state[7] ];
	state[7]  = sbox[ state[3] ];
	state[3]  = temp1;
}	// void SubBytes_ShiftRows(unsigned char *state)

/*
 ********************************************************************************
 * Performs Mix Columns Operation												*
 * state  variable to store the intermediate result of the algorithm			*
 ********************************************************************************
 */
static void MixColumnns(unsigned char *state) {
unsigned char temp1, temp2, temp3;

	// col 1
	temp1 = state[0] ^ state[1] ^ state[2] ^ state[3];
	temp2 = state[0];
	temp3 = state[0] ^ state[1];
	temp3 = xtime( temp3 );
	state[0] = state[0] ^ temp3 ^ temp1;
	temp3 = state[1] ^ state[2];
	temp3 = xtime( temp3 );
	state[1] = state[1] ^ temp3 ^ temp1;
	temp3 = state[2] ^ state[3];
	temp3 = xtime( temp3 );
	state[2] = state[2] ^ temp3 ^ temp1;
	temp3 = state[3] ^ temp2;
	temp3 = xtime( temp3 );
	state[3] = state[3] ^ temp3 ^ temp1;

	// col 2
	temp1 = state[4] ^ state[5] ^ state[6] ^ state[7];
	temp2 = state[4];
	temp3 = state[4] ^state[5];
	temp3 = xtime( temp3 );
	state[4] = state[4] ^ temp3 ^ temp1;
	temp3 = state[5] ^state[6];
	temp3 = xtime( temp3 );
	state[5] = state[5] ^ temp3 ^ temp1;
	temp3 = state[6] ^ state[7];
	temp3 = xtime( temp3 );
	state[6] = state[6] ^ temp3 ^ temp1;
	temp3 = state[7] ^ temp2;
	temp3 = xtime( temp3 );
	state[7] = state[7] ^ temp3 ^ temp1;

	// col 3
	temp1 = state[8] ^ state[9] ^ state[10] ^ state[11];
	temp2 = state[8];
	temp3 = state[8] ^ state[9];
	temp3 = xtime( temp3 );
	state[8] = state[8] ^ temp3 ^ temp1;
	temp3 = state[9] ^ state[10];
	temp3 = xtime( temp3 );
	state[9] = state[9] ^ temp3 ^ temp1;
	temp3 = state[10]^state[11];
	temp3 = xtime( temp3 );
	state[10] = state[10] ^ temp3 ^ temp1;
	temp3 = state[11] ^ temp2;
	temp3 = xtime( temp3 );
	state[11] = state[11] ^ temp3 ^ temp1;

	// col 4
	temp1 = state[12 ] ^ state[13] ^ state[14] ^ state[15];
	temp2 = state[12];
	temp3 = state[12] ^ state[13];
	temp3 = xtime( temp3 );
	state[12] = state[12] ^ temp3 ^ temp1;
	temp3 = state[13] ^ state[14];
	temp3 = xtime( temp3 );
	state[13] = state[13] ^ temp3 ^ temp1;
	temp3 = state[14] ^ state[15];
	temp3 = xtime( temp3 );
	state[14] = state[14] ^ temp3 ^ temp1;
	temp3 = state[15] ^ temp2;
	temp3 = xtime( temp3 );
	state[15] = state[15] ^ temp3 ^ temp1;
}	// void MixColumnns(unsigned char *state)

/*
 ********************************************************************************
 * Performs Add RoundKey operation.												*
 * state  variable to store the intermediate result of the algorithm			*
 ********************************************************************************
 */
static void AddRoundKey(unsigned char *state, unsigned char round, unsigned char * keySchedule) {
	// row 0
	state[0]  ^= keySchedule[(round * 16)];
	state[4]  ^= keySchedule[(round * 16) + 4];
	state[8]  ^= keySchedule[(round * 16) + 8];
	state[12] ^= keySchedule[(round * 16) + 12];

	// row 1
	state[1]  ^= keySchedule[(round * 16) + 1];
	state[5]  ^= keySchedule[(round * 16) + 5];
	state[9]  ^= keySchedule[(round * 16) + 9];
	state[13] ^= keySchedule[(round * 16) +13];

	// row 2
	state[2]  ^= keySchedule[(round * 16) + 2];
	state[6]  ^= keySchedule[(round * 16) + 6];
	state[10] ^= keySchedule[(round * 16) + 10];
	state[14] ^= keySchedule[(round * 16) + 14];

	// row 3
	state[3]  ^= keySchedule[(round * 16) + 3];
	state[7]  ^= keySchedule[(round * 16) + 7];
	state[11] ^= keySchedule[(round * 16) + 11];
	state[15] ^= keySchedule[(round * 16) + 15];
}	// void AddRoundKey(unsigned char *state, unsigned char round, unsigned char * keySchedule)

/*
 ********************************************************************************
 * Performs Inverse of sub bytes (from inverse sub-box)	and shift rows operation*
 * state  variable to store the intermediate result of the algorithm			*
 ********************************************************************************
 */
static void InvShiftRows_SubBytes(unsigned char *state) {
unsigned char temp1, temp2;

	// row 0
	state[0]  = inv_sbox[ state[0] ];
	state[4]  = inv_sbox[ state[4] ];
	state[8]  = inv_sbox[ state[8] ];
	state[12] = inv_sbox[ state[12] ];

	// row 1
	temp1	  = inv_sbox[ state[13] ];
	state[13] = inv_sbox[ state[9] ];
	state[9]  = inv_sbox[ state[5] ];
	state[5]  = inv_sbox[ state[1] ];
	state[1]  = temp1;

	// row 2
	temp1	  = inv_sbox[ state[2] ] ;
	temp2	  = inv_sbox[ state[6] ] ;
	state[2]  = inv_sbox[ state[10] ] ;
	state[6]  = inv_sbox[ state[14] ] ;
	state[10] = temp1;
	state[14] = temp2;

	// row 3
	temp1	  = inv_sbox[ state[3] ];
	state[3]  = inv_sbox[ state[7] ];
	state[7]  = inv_sbox[ state[11] ] ;
	state[11] = inv_sbox[ state[15] ] ;
	state[15] = temp1;
}	// void InvShiftRows_SubBytes(unsigned char *state)

/*
 ********************************************************************************
 * Performs Inverse  Mix Columns operation										*
 * state  variable to store the intermediate result of the algorithm			*
 * Number of current ongoing round in AES algorithm								*
 ********************************************************************************
 */
static void InvMixColumns(unsigned char *state) {
unsigned char temp1, temp2, temp3;

	//col1
	temp1 = xtime( xtime( state[0] ^ state[2] ) );
	temp2 = xtime( xtime( state[1] ^ state[3] ) );
	state[0] ^= temp1;
	state[1] ^= temp2;
	state[2] ^= temp1;
	state[3] ^= temp2;

	//col2
	temp1 = xtime( xtime( state[4] ^ state[6] ) );
	temp2 = xtime( xtime( state[5] ^ state[7] ) );
	state[4] ^= temp1;
	state[5] ^= temp2;
	state[6] ^= temp1;
	state[7] ^= temp2;

	//col3
	temp1 = xtime( xtime( state[8] ^ state[10] ) );
	temp2 = xtime( xtime( state[9] ^ state[11] ) );
	state[8]  ^= temp1;
	state[9]  ^= temp2;
	state[10] ^= temp1;
	state[11] ^= temp2;

	//col4
	temp1 = xtime( xtime( state[12] ^ state[14] ) );
	temp2 = xtime( xtime( state[13] ^ state[15] ) );
	state[12] ^= temp1;
	state[13] ^= temp2;
	state[14] ^= temp1;
	state[15] ^= temp2;

	// col1
	temp1 = state[0] ^ state[1] ^ state[2] ^ state[3];
	temp2 = state[0];
	temp3 = state[0] ^ state[1];
	temp3 = xtime( temp3 );
	state[0] = state[0] ^ temp3 ^ temp1;
	temp3 = state[1] ^ state[2];
	temp3 = xtime( temp3 );
	state[1] = state[1] ^ temp3 ^ temp1;
	temp3 = state[2] ^ state[3];
	temp3 = xtime( temp3 );
	state[2] = state[2] ^ temp3 ^ temp1;
	temp3 = state[3] ^ temp2;
	temp3 = xtime( temp3 );
	state[3] = state[3] ^ temp3 ^ temp1;

	// col2
	temp1 = state[4] ^ state[5] ^ state[6] ^ state[7];
	temp2 = state[4];
	temp3 = state[4] ^ state[5];
	temp3 = xtime( temp3 );
	state[4] = state[4] ^ temp3 ^ temp1;
	temp3 = state[5] ^ state[6];
	temp3 = xtime( temp3 );
	state[5] = state[5] ^ temp3 ^ temp1;
	temp3 = state[6] ^ state[7];
	temp3 = xtime( temp3 );
	state[6] = state[6] ^ temp3 ^ temp1;
	temp3 = state[7] ^ temp2;
	temp3 = xtime( temp3 );
	state[7] = state[7] ^ temp3 ^ temp1;

	// col3
	temp1 = state[8] ^ state[9] ^ state[10] ^ state[11];
	temp2 = state[8];
	temp3 = state[8] ^state[9];
	temp3 = xtime( temp3 );
	state[8] = state[8] ^ temp3 ^ temp1;
	temp3 = state[9] ^ state[10];
	temp3 = xtime( temp3 );
	state[9] = state[9] ^ temp3 ^ temp1;
	temp3 = state[10] ^ state[11];
	temp3 = xtime( temp3 );
	state[10] = state[10] ^ temp3 ^ temp1;
	temp3 = state[11] ^ temp2;
	temp3 = xtime( temp3 );
	state[11] = state[11] ^ temp3 ^ temp1;

	// col4
	temp1 = state[12] ^ state[13] ^ state[14] ^ state[15];
	temp2 = state[12];
	temp3 = state[12] ^ state[13];
	temp3 = xtime( temp3 );
	state[12] = state[12] ^ temp3 ^ temp1;
	temp3 = state[13]^state[14];
	temp3 = xtime( temp3 );
	state[13] = state[13] ^ temp3 ^ temp1;
	temp3 = state[14] ^ state[15];
	temp3 = xtime( temp3 );
	state[14] = state[14] ^ temp3 ^ temp1;
	temp3 = state[15] ^ temp2;
	temp3 = xtime( temp3 );
	state[15] = state[15] ^ temp3 ^ temp1;
}	// void InvMixColumns(unsigned char *state)

/*
 ********************************************************************************
 * Performs AES-128 Encryption													*
 * plainText  Input plain text block											*
 * state Variable to store the intermediate result								*
 ********************************************************************************
 */
void CipherAES(unsigned char *plainText, unsigned char *state, unsigned char * keySchedule) {
unsigned char	byte_count;
unsigned char	round;

	if (state != plainText)
		// Copy input buffer to state to form the cipher state
		for (byte_count = 0; byte_count < AES_BLOCK_SIZE; byte_count++)
			state[byte_count] = plainText[byte_count];

	AddRoundKey(state, 0, keySchedule);
	//Round 1 to 9
	for (round = 1; round < 10; round ++){
		/* Sub Bytes and Shift Rows */
		SubBytes_ShiftRows(state);
		/* Mix Columns */
		MixColumnns(state);
		/* Add RoundKey */
		AddRoundKey(state, round, keySchedule);
	}

	// 10th round without mix columns

	/* Sub Bytes and Shift Rows */
	SubBytes_ShiftRows(state);
	/* Add RoundKey */
	AddRoundKey(state, round, keySchedule);
}	// void CipherAES(unsigned char *plainText, unsigned char *state, unsigned char * keySchedule)

/*
 ********************************************************************************
 * Performs AES-128 Decryption													*
 * cipherText  Input cipher text block											*
 * state Variable to store the intermediate result								*
 ********************************************************************************
 */
void InverseCipherAES (unsigned char *cipherText, unsigned char *state, unsigned char * keySchedule) {

	unsigned char round = 0;
	unsigned char byte_count = 0;

	// Copy input buffer to state to form the plain text
	if (state != cipherText)
		for (byte_count = 0; byte_count < AES_BLOCK_SIZE; byte_count++)
			state[byte_count] = cipherText[byte_count];

	// Initial addroundkey
	AddRoundKey(state, 10, keySchedule);
	// Round 9 to 1
	for(round = 9; round > 0; round--){
		/* Inverse Sub Bytes and Shift Rows */
		InvShiftRows_SubBytes(state);
		/* Inverse Add Round Key */
		AddRoundKey(state, round, keySchedule);
		/* Inverse Mix Columns */
		InvMixColumns(state);
	}

	// Round 0 without mixcols
	/* Inverse Sub Bytes and Shift Rows */
	InvShiftRows_SubBytes(state);
	/* Inverse Add Round Key */
	AddRoundKey(state, 0, keySchedule);

}	// void InverseCipherAES (unsigned char *cipherText, unsigned char *state, unsigned char * keySchedule)

