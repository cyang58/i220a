#include "hamming.h"

#include <stdio.h>
#include <math.h>
#include <assert.h>

/**
  All bitIndex'es are numbered starting at the LSB which is given index 1

  ** denotes exponentiation; note that 2**n == (1 << n)
*/

/** Return bit at bitIndex from word. */
static inline unsigned
get_bit(HammingWord word, int bitIndex)
{
  assert(bitIndex > 0);
  //@TODO
  return (word >> (bitIndex-1))%2;
}

/** Return word with bit at bitIndex in word set to bitValue. */
static inline HammingWord
set_bit(HammingWord word, int bitIndex, unsigned bitValue)
{
  assert(bitIndex > 0);
  assert(bitValue == 0 || bitValue == 1);
  //@TODO
  return (bitValue) ? word | (1<<(bitIndex-1)) : word & ~(1<<(bitIndex-1));
}

/** Given a Hamming code with nParityBits, return 2**nParityBits - 1,
 *  i.e. the max # of bits in an encoded word (# data bits + # parity
 *  bits).
 */
static inline unsigned
get_n_encoded_bits(unsigned nParityBits)
{
  //@TODO
  return (2 << nParityBits)-1;
}

/** Return non-zero if bitIndex indexes a bit which will be used for a
 *  Hamming parity bit; i.e. the bit representation of bitIndex
 *  contains only a single 1.
 */
static inline int
is_parity_position(int bitIndex)
{
  assert(bitIndex > 0);
  //@TODO
  for (unsigned power = 1; power > 0; power <<= 1)
  {
	  if (power == bitIndex)
	  {
		  return 1;
	  }
	  if (power > bitIndex)
	  {
		  return 0;
	  }
  }
  return 0;
}
/** Return the parity over the data bits in word specified by the
 *  parity bit bitIndex.  The word contains a total of nBits bits.
 *  Equivalently, return parity over all data bits whose bit-index has
 *  a 1 in the same position as in bitIndex.
 */
static int
compute_parity(HammingWord word, int bitIndex, unsigned nBits)
{
  assert(bitIndex > 0);
  //@TODO
  int r = 0; 
  for(unsigned cbits = 1; cbits <= nBits; cbits++){
    if(!is_parity_position(cbits) && (cbits | bitIndex) == cbits){
	r ^= get_bit(word, cbits);	
    }
  }
  return r;
}

/** Encode data using nParityBits Hamming code parity bits.
 *  Assumes data is within range of values which can be encoded using
 *  nParityBits.
 */
HammingWord
hamming_encode(HammingWord data, unsigned nParityBits)
{
  //@TODO
  HammingWord encoded = 0;
  int counter = 1;
  for (unsigned i = 1; i <= get_n_encoded_bits(nParityBits); i++)
  {
	  if (is_parity_position(i) == 0)
	  {
		  encoded = set_bit(encoded, i, get_bit(data, counter));
		  counter++;
	  }
  }
  for (unsigned i = 1; i <= get_n_encoded_bits(nParityBits); i++)
  {
	  if (is_parity_position(i) == 1)
	  {
		  encoded = set_bit(encoded, i, compute_parity(encoded, i, get_n_encoded_bits(nParityBits)));
	  }
  }
  return encoded;
  return 0;
}

/** Decode encoded using nParityBits Hamming code parity bits.
 *  Set *hasError if an error was corrected.
 *  Assumes that data is within range of values which can be decoded
 *  using nParityBits.
 */
HammingWord
hamming_decode(HammingWord encoded, unsigned nParityBits,
                           int *hasError)
{
  //@TODO
  HammingWord errorsyndrome = 0;
  for(unsigned i = 1; i <= get_n_encoded_bits(nParityBits); i++){
    if(is_parity_position(i)){
		int r = compute_parity(encoded, i, get_n_encoded_bits(nParityBits));
      if(get_bit(encoded, i) != r){
        errorsyndrome |= i;
      }
    }
  }
  if(errorsyndrome != 0){
    set_bit(encoded, errorsyndrome, (get_bit(encoded, errorsyndrome)==1) ? 0 : 1);
    *hasError = 1;
  }
  HammingWord decoded = 0;
  int pos = 1;
  for(unsigned i = 1; i <= get_n_encoded_bits(nParityBits); i++){
    if(!is_parity_position(i)){
      decoded = set_bit(decoded, pos, get_bit(encoded, i));
      pos++;
    }
  }
  return decoded;
}
