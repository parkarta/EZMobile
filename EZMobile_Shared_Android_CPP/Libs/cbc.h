/*
 * FIPS SP800-38A software library header file
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
/*
 * Revision history:
 * V1.0.0 (19.09.2017) Initial release
 *		1) Renamed from crypt.h to cbc.h
 *		2) Only CBC mode is used
 *		3) Functions cbc_encrypt() and cbc_decript() are renamed to EncryptCBC_AES() and DecryptCBC_AES()
 */

#ifndef CBC_H
#define CBC_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************************
 * This function performs CBC encryption on input plain text blocks.					*
 * plainText   pointer to the array containing the plain text bytes						*
 * cipherText  pointer to the array where the encrypted cipher output has to be stored	*
 * init_vector pointer to the array containing the unique initialization vector			*
 * size        size of the input plain text												*
 ****************************************************************************************
 */
void EncryptCBC_AES (
					  unsigned char *plainText
					, unsigned char *cipherText
					, unsigned char *keySchedule
					, unsigned char *init_vector
					, unsigned size);

/*
 ****************************************************************************************
 * This function performs CBC decryption on input cipher text blocks.					*
 * cipherText  pointer to the array containing the cipher text bytes					*
 * plainText   pointer to the array where the decrypted plain text has to be stored		*
 * init_vector pointer to the array containing the unique initialization vector			*
 * size        size of the input plain text												*
 ****************************************************************************************
 */
void DecryptCBC_AES (
					  unsigned char *cipherText
					, unsigned char *plainText
					, unsigned char *keySchedule
					, unsigned char *init_vector
					, unsigned size);

#ifdef __cplusplus
}
#endif

#endif
