/* $Id$
 *
 * Provides routines for encoding and decoding the extended Golay
 * (24,12,8) code.
 *
 * This implementation will detect up to 4 errors in a codeword (without
 * being able to correct them); it will correct up to 3 errors.
 *
 * We use uint32_t's to hold the 24-bit codewords, with the data part in
 * the bottom 12 bits and the parity in bits 12-23.
 *
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*!
 * @file golay.h
 * @author StevenKnudsen
 * @date Aug 7, 2022
 *
 * @details Changes to original code and documentation.
 *
 * @copyright See original file information above.
 *
 * @license GPL, see above
 */

#ifndef EX2_MAC_GOLAY_H__
#define EX2_MAC_GOLAY_H__

#include <stdint.h>

/*!
 * @brief Encodes a 12-bit word to a 24-bit codeword
 * @param[in] message The 12-bit message word
 * @return The 24-bit encoded message word aka codeword
 */
uint32_t golay_encode(uint16_t message);

/*!
 * @brief Return a mask showing the bits which are in error in a received 24-bit
 * codeword, or -1 if 4, 6, 8, 10, or 12 errors were detected. If the number of
 * errors is odd, a mask is returned, but it likely not valid so don't rely on it.
 * @param[in] codeword The Golay-encoded 12 message bits
 * @return A mask showing the bits that are in error in the received 24-bit
 * codeword
 */
int32_t golay_errors(uint32_t codeword);

/*!
 * @brief Decode the input codeword. Up to 3 errors are corrected for; 4
   errors are detected as uncorrectable (return -1); 5 or more errors
   cause an incorrect correction, that is, the result cannot be relied on and
   some other check should be done.
 * @param[in] codeword The Golay-encoded message
 * @return
 */
int16_t golay_decode(uint32_t codeword);

#endif // EX2_MAC_GOLAY_H__
