/*
 * FIPS SP800-38A software library source file
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

/*
 ****************************************************************************************
 * This function performs CBC encryption on input plain text blocks.					*
 * plainText   pointer to the array containing the plain text bytes						*
 * cipherText  pointer to the array where the encrypted cipher output has to be stored	*
 * init_vector pointer to the array containing the unique initialization vector			*
 * size        size of the input plain text												*
 *																						*
 * Note: 'size' must be a multiple of AES_BLOCK_SIZE									*
 * Side effects: Modifies iv[] vector (ready for the next encryption)					*
 ****************************************************************************************
 */
void EncryptCBC_AES (
					  unsigned char *plainText
					, unsigned char *cipherText
					, unsigned char *keySchedule
					, unsigned char *iv
					, unsigned size)
{
unsigned		blockCount, inputBlocks;
unsigned char	byteCount;

	// Calculate input block size
	inputBlocks = ((size + 15) / AES_BLOCK_SIZE);		// Block must be multiple of 16

	for (blockCount = 0; blockCount < inputBlocks; blockCount++) {

		for (byteCount = 0; byteCount < AES_BLOCK_SIZE; byteCount++)
			// XOR plain text with initialization vector to form first input block
			plainText[byteCount + blockCount * AES_BLOCK_SIZE] ^= iv[byteCount];
		// Sending initialization vector to the cipher function to get further output blocks
		CipherAES ( plainText  + (blockCount * AES_BLOCK_SIZE),
					cipherText + (blockCount * AES_BLOCK_SIZE),
					keySchedule);
		for (byteCount = 0; byteCount < AES_BLOCK_SIZE; byteCount++)
			iv[byteCount] = cipherText[byteCount + blockCount * AES_BLOCK_SIZE];

	}	// for (block_count = 0; block_count < input_block_size; block_count++)

}	// void EncryptCBC_AES (unsigned char *plainText, unsigned char *cipherText, unsigned char * iv, uint32_t size)

/*
 ****************************************************************************************
 * This function performs CBC decryption on input cipher text blocks.					*
 * cipherText  pointer to the array containing the cipher text bytes					*
 * plainText   pointer to the array where the decrypted plain text has to be stored		*
 * init_vector pointer to the array containing the unique initialization vector			*
 * size        size of the input plain text												*
 *																						*
 * Note: 'size' must be a multiple of AES_BLOCK_SIZE									*
 * Side effects: Modifies iv[] vector (ready for the next decryption)					*
 ****************************************************************************************
 */
void DecryptCBC_AES (
					  unsigned char *cipherText
					, unsigned char *plainText
					, unsigned char *keySchedule
					, unsigned char *iv
					, unsigned size)
{
unsigned		blockCount, inputBlocks;
unsigned char	byteCount;
unsigned char	ivt[AES_BLOCK_SIZE];

	// Calculate input block size
	inputBlocks = (size / AES_BLOCK_SIZE );

	for (blockCount = 0; blockCount < inputBlocks; blockCount++) {
		for (byteCount = 0; byteCount < AES_BLOCK_SIZE; byteCount++)
			ivt[byteCount] = cipherText[byteCount + blockCount * AES_BLOCK_SIZE];
		// Perform inverse cipher for the input blocks
		InverseCipherAES(cipherText + (blockCount * AES_BLOCK_SIZE),
						 plainText  + (blockCount * AES_BLOCK_SIZE),
						 keySchedule);

		for (byteCount = 0; byteCount < AES_BLOCK_SIZE; byteCount++) {
			// XOR output of previous inverse cipher function with previous cipher text to
			// form the input for next block
			plainText[byteCount + (blockCount * AES_BLOCK_SIZE)] ^= iv[byteCount];
			iv[byteCount] = ivt[byteCount];
		}
	}	// for (block_count = 1; block_count < input_block_size; block_count++)

}	// void DecryptCBC_AES (unsigned char *cipherText, unsigned char *plainText, unsigned char *iv, uint32_t size)

