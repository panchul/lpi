 
/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "lpi_bignums.h"

namespace lpi
{

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


ufixed96q32& ufixed96q32::operator+=(const ufixed96q32& rhs)
{
  add<4>(data, data, rhs.data);
  return *this;
}

ufixed96q32& ufixed96q32::operator-=(const ufixed96q32& rhs)
{
  subtract<4>(data, data, rhs.data);
  return *this;
}

ufixed96q32& ufixed96q32::operator+=(uint32 rhs)
{
  add<4,1,1,0>(data, data, &rhs);
  return *this;
}

ufixed96q32& ufixed96q32::operator-=(uint32 rhs)
{
  subtract<4,1,1,0>(data, data, &rhs);
  return *this;
}

ufixed96q32& ufixed96q32::operator+=(double rhs)
{
  return (*this)+= ufixed96q32(rhs);
}

ufixed96q32& ufixed96q32::operator-=(double rhs)
{
  return (*this)-= ufixed96q32(rhs);
}

void ufixed96q32::addFractional(uint32 rhs)
{
  data[0] += rhs; //data[0], the fractional part, is left untouched
  if(data[0] < rhs) //if it has overflown
  for(int i = 1; i < 4; i++)
  {
    data[i]++;
    if(data[i] != 0U) break; //only if data[i] is 0 (0xFFFFFFFF + 1), it has overflown and we should continue
  }
}

void ufixed96q32::subtractFractional(uint32 rhs)
{
  if(rhs <= data[0])
  {
    data[0] -= rhs; //data[0], the fractional part, is left untouched
  }
  else //overflow case
  {
    data[0] -= rhs; //data[0], the fractional part, is left untouched
    for(int i = 1; i < 4; i++)
    {
      data[i]--;
      if(data[i] != 0xFFFFFFFFU) break; //only if data[i] is 0xFFFFFFFF (0 - 1), it has overflown and we should continue
    }
  }
}

ufixed96q32::operator double() const //this only returns a correct result if the double can hold this value
{
  double result = 0.0;
  
  double multiplier = 1.0 / 4294967296.0;
  for(int i = 0; i < 4; i++)
  {
    result += multiplier * data[i];
    multiplier *= 4294967296.0;
  }
  return result;
}

void ufixed96q32::operator=(double d)
{
  double multiplier = 4294967296.0;
  for(int i = 0; i < 4; i++)
  {
    data[i] = uint32(d * multiplier);
    multiplier /= 4294967296.0;
  }
}

void ufixed96q32::operator=(const fixed96q32& o)
{
  data[0] = o.data[0];
  data[1] = o.data[1];
  data[2] = o.data[2];
  data[3] = o.data[3];
}

ufixed96q32::operator uint32() const //this only returns a correct result if the double can hold this value
{
  return data[1];
}

void ufixed96q32::operator=(uint32 d)
{
  data[0] = 0;
  data[1] = d;
  data[2] = 0;
  data[3] = 0;
}

ufixed96q32& ufixed96q32::operator++()
{
  data[1]++;
  if(data[1] == 0)
  {
    data[2]++;
    if(data[2] == 0)
    {
      data[3]++;
    }
  }
  return *this;
}

ufixed96q32 ufixed96q32::operator++(int)
{
  ufixed96q32 result = *this;
  (*this)++;
  return result;
}

ufixed96q32& ufixed96q32::operator--()
{
  data[1]--;
  if(data[1] == 0xFFFFFFFFU)
  {
    data[2]--;
    if(data[2] == 0xFFFFFFFFU)
    {
      data[3]--;
    }
  }
  return *this;
}

ufixed96q32 ufixed96q32::operator--(int)
{
  ufixed96q32 result = *this;
  (*this)--;
  return result;
}

void ufixed96q32::addLSB()
{
  lpi::addLSB<4>(data);
}

void ufixed96q32::subtractLSB()
{
  lpi::subtractLSB<4>(data);
}

void ufixed96q32::bit_invert()
{
  lpi::bit_invert<4>(data);
}

void ufixed96q32::negate()
{
  lpi::negate<4>(data);
}

bool ufixed96q32::sign() const
{
  return data[3] > 2147483647U;
}

void ufixed96q32::takeSqrt()
{
  /*
  Note: a good and faster way to take the sqrt is to convert to double and then take the sqrt.
  That has less precision, but that is not always a problem since it's always nearly right for even the largest values.
  */
  if(isZero()) return;

  ufixed96q32 result = *this;
  static const ufixed96q32 mask = ~(ufixed96q32(1U) >> 32U); //only the LSB is 0

  uint32 i;
  for(i = 0; result.isZero(); i++) result >>= 1U;
  result = (*this) >> (2 * i);
  for(;;)
  {
    ufixed96q32 temp = ufixed96q32((*this)>>1U) / result - (result>>1U);
    if((temp & mask).isZero()) break;
    else result += temp;
  }
  
  *this = result;
}

////////////////////////////////////////////////////////////////////////////////

ufixed96q32& ufixed96q32::operator/=(const ufixed96q32& rhs)
{
  //divident / dividor = quotient (= *this)
  /*
  Without precaution, the result will exclude the digits behind the point.
  So to solve: the divident is multiplied with 4 billion compared to the dividor
  */
  ufixed96q32 dividor = rhs;
  uint32 dividorMSB = 0U;
  uint32 dividentMSB = data[3]; //MSB's for the divident
  ufixed96q32 divident = ((*this) << 32U);
  
  int shift = 0;
  
  uint32 thisMSB = 0U; //extra uint32 for *this (extra MSB's)
  makeZero();
  
  if(!dividor.isZero()) //avoid division through 0
  {
    /*left shift dividor until it is greater than or equal to divident
    the comparision includes checks for the extra MSBs*/
    while(dividorMSB < dividentMSB || (dividorMSB == dividentMSB && dividor < divident))
    {
      dividorMSB <<= 1; if(dividor.data[3] & 2147483648U) dividorMSB |= 1; else dividorMSB &= 4294967294U;
      dividor <<= 1;
      shift++;
    }

    //if the dividor is now larger than the divident, right shift it again once
    if(dividorMSB > dividentMSB || (dividorMSB == dividentMSB && dividor > divident))
    {
      dividor >>= 1;
      if(dividorMSB & 1U) dividor.data[3] |= 2147483648U; else dividor.data[3] &= 2147483647U;
      dividorMSB >>= 1; dividorMSB &= 2147483647U; //make extra sure the left bit is 0
      
      shift--;
    }
    
    while(shift >= 0)
    {
      //left shift quotient
      thisMSB <<= 1; if(data[3] & 2147483648U) thisMSB |= 1; else thisMSB &= 4294967294U;
      (*this) <<= 1;
      
      if(!(dividorMSB > dividentMSB || (dividorMSB == dividentMSB && dividor > divident))) //if dividor <= divident...
      {
        //subtract dividor from divident, operator- implemented here including the extra MSBs
        {
          bool borrow = false;
          for(int i = 0; i < 4; i++)
          {
            if(borrow)
            {
              borrow = dividor.data[i] >= divident.data[i];
              divident.data[i] -= (dividor.data[i] + 1U);
            }
            else
            {
              borrow = dividor.data[i] > divident.data[i] ;
              divident.data[i] -= dividor.data[i];
            }
          }
          //the MSBs from divident
          if(borrow) dividentMSB -= (dividorMSB + 1U);
          else dividentMSB -= dividorMSB;
        }

        //increment the LSB from the quotient, carrying to the whole left if needed
        addLSB();
        if(isZero()) thisMSB++;
      }
      
      //right shift dividor 
      dividor >>= 1;
      if(dividorMSB & 1U) dividor.data[3] |= 2147483648U; else dividor.data[3] &= 2147483647U;
      dividorMSB >>= 1; dividorMSB &= 2147483647U; //make extra sure the left bit is 0

      shift--;
    }
  }
  
  return *this;
}

ufixed96q32& ufixed96q32::operator>>=(uint32 shift)
{
  rightshift_unsigned<4>(data, shift);
  return *this;
}

ufixed96q32& ufixed96q32::operator<<=(uint32 shift)
{
  leftshift<4>(data, shift);
  return *this;
}

void ufixed96q32::makeZero()
{
  data[0] = data[1] = data[2] = data[3] = 0U;
}

ufixed96q32& ufixed96q32::operator*=(uint32 rhs)
{
  ufixed96q32 a = *this;
  
  makeZero();
  
  for(int i = 0; i < 32; i++)
  {
    if((rhs >> i) & 1) (*this) += a;
    a <<= 1;
  }

  return *this;
}

ufixed96q32& ufixed96q32::operator*=(const ufixed96q32& rhs)
{
  //160 bit precision used, and one right shift at the end
  uint32 result[5];
  
  multiply_unsigned<5, 4, 4>(result, data, rhs.data);
  
  data[0] = result[1];
  data[1] = result[2];
  data[2] = result[3];
  data[3] = result[4];

  return *this;
}

ufixed96q32& ufixed96q32::operator*=(double rhs)
{
  return operator*=((ufixed96q32)rhs);
}

//this one loses the leftmost 32 bits, but shows a simpler version of the multiplication implementation for documentation
void ufixed96q32::crudeMultiply(const ufixed96q32& rhs)
{
  ufixed96q32 a = *this;
  ufixed96q32 b = rhs;
  makeZero();
  for(int i = 0; i < 128; i++)
  {
    if(b.getLSB()) (*this) += a;
    a <<= 1;
    b >>= 1;
  }
  
  (*this) >>= 32; //the part behind the comma is smaller
}

bool ufixed96q32::operator==(const ufixed96q32& rhs) const
{
  return data[0] == rhs.data[0]
      && data[1] == rhs.data[1]
      && data[2] == rhs.data[2]
      && data[3] == rhs.data[3];
}

bool ufixed96q32::operator==(uint32 rhs) const
{
  return data[0] == 0U
      && data[1] == rhs
      && data[2] == 0U
      && data[3] == 0U;
}

bool ufixed96q32::operator>(const ufixed96q32& rhs) const
{
  if(data[3] == rhs.data[3])
  {
    if(data[2] == rhs.data[2])
    {
      if(data[1] == rhs.data[1])
      {
        return data[0] > rhs.data[0];
      }
      else return data[1] > rhs.data[1];
    }
    else return data[2] > rhs.data[2];
  }
  else return data[3] > rhs.data[3];
}

bool ufixed96q32::operator<(const ufixed96q32& rhs) const
{
  if(data[3] == rhs.data[3])
  {
    if(data[2] == rhs.data[2])
    {
      if(data[1] == rhs.data[1])
      {
        return data[0] < rhs.data[0];
      }
      else return data[1] < rhs.data[1];
    }
    else return data[2] < rhs.data[2];
  }
  else return data[3] < rhs.data[3];
}

bool ufixed96q32::operator>=(const ufixed96q32& rhs) const
{
  return !((*this) > rhs);
}

bool ufixed96q32::operator<=(const ufixed96q32& rhs) const
{
  return !((*this) > rhs);
}

ufixed96q32& ufixed96q32::operator&=(const ufixed96q32& rhs)
{
  for(int i = 0; i < 4; i++)
  {
    data[i] &= rhs.data[i];
  }
  
  return *this;
}

ufixed96q32& ufixed96q32::operator|=(const ufixed96q32& rhs)
{
  for(int i = 0; i < 4; i++)
  {
    data[i] |= rhs.data[i];
  }
  
  return *this;
}

ufixed96q32& ufixed96q32::operator^=(const ufixed96q32& rhs)
{
  for(int i = 0; i < 4; i++)
  {
    data[i] ^= rhs.data[i];
  }
  
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

ufixed96q32 operator-(const ufixed96q32& a)
{
  ufixed96q32 result = a;
  result.negate();
  return result;
}


ufixed96q32 operator~(const ufixed96q32& a)
{
  ufixed96q32 result = a;
  result.bit_invert();
  return result;
}


ufixed96q32 operator+(const ufixed96q32& a, const ufixed96q32& b)
{
  ufixed96q32 result = a;
  result += b;
  return result;
}

ufixed96q32 operator-(const ufixed96q32& a, const ufixed96q32& b)
{
  ufixed96q32 result = a;
  result -= b;
  return result;
}

ufixed96q32 operator+(const ufixed96q32& a, uint32& b)
{
  ufixed96q32 result = a;
  result += b;
  return result;
}

ufixed96q32 operator+(uint32& a, const ufixed96q32& b)
{
  ufixed96q32 result = b;
  result += a;
  return result;
}

ufixed96q32 operator-(const ufixed96q32& a, uint32& b)
{
  ufixed96q32 result = a;
  result -= b;
  return result;
}

ufixed96q32 operator>>(const ufixed96q32& a, uint32 b)
{
  ufixed96q32 result = a;
  result >>= b;
  return result;
}

ufixed96q32 operator<<(const ufixed96q32& a, uint32 b)
{
  ufixed96q32 result = a;
  result <<= b;
  return result;
}

ufixed96q32 operator&(const ufixed96q32& a, const ufixed96q32& b)
{
  ufixed96q32 result = a;
  result &= b;
  return result;
}

ufixed96q32 operator|(const ufixed96q32& a, const ufixed96q32& b)
{
  ufixed96q32 result = a;
  result |= b;
  return result;
}

ufixed96q32 operator^(const ufixed96q32& a, const ufixed96q32& b)
{
  ufixed96q32 result = a;
  result ^= b;
  return result;
}

ufixed96q32 operator*(const ufixed96q32& a, const ufixed96q32& b)
{
  ufixed96q32 result = a;
  result *= b;
  return result;
}

ufixed96q32 operator*(const ufixed96q32& a, uint32 b)
{
  ufixed96q32 result = a;
  result *= b;
  return result;
}

ufixed96q32 operator*(uint32 a, const ufixed96q32& b)
{
  ufixed96q32 result = b;
  result *= a;
  return result;
}


ufixed96q32 operator*(const ufixed96q32& a, double b)
{
  ufixed96q32 result = a;
  result *= b;
  return result;
}

ufixed96q32 operator*(double a, const ufixed96q32& b)
{
  ufixed96q32 result = b;
  result *= a;
  return result;
}

ufixed96q32 operator/(const ufixed96q32& a, const ufixed96q32& b)
{
  ufixed96q32 result = a;
  result /= b;
  return result;
}

bool operator==(uint32 a, const ufixed96q32& b)
{
  return b == a;
}

ufixed96q32 sqrt(const ufixed96q32& a)
{
  /*
  Note: a good and faster way to take the sqrt is to convert to double and then take the sqrt.
  That has less precision, but that is not always a problem since it's always nearly right for even the largest values.
  */
  ufixed96q32 result = a;
  result.takeSqrt();
  return result;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

fixed96q32& fixed96q32::operator+=(const fixed96q32& rhs)
{
  add<4>(data, data, rhs.data);
  return *this;
}

fixed96q32& fixed96q32::operator-=(const fixed96q32& rhs)
{
  subtract<4>(data, data, rhs.data);
  return *this;
}

fixed96q32& fixed96q32::operator+=(uint32 rhs)
{
  add<4,1,1,0>(data, data, &rhs);
  return *this;
}

fixed96q32& fixed96q32::operator-=(uint32 rhs)
{
  subtract<4,1,1,0>(data, data, &rhs);
  return *this;
}

fixed96q32& fixed96q32::operator+=(double rhs)
{
  return (*this)+= fixed96q32(rhs);
}

fixed96q32& fixed96q32::operator-=(double rhs)
{
  return (*this)-= fixed96q32(rhs);
}

void fixed96q32::addFractional(uint32 rhs)
{
  data[0] += rhs; //data[0], the fractional part, is left untouched
  if(data[0] < rhs) //if it has overflown
  for(int i = 1; i < 4; i++)
  {
    data[i]++;
    if(data[i] != 0U) break; //only if data[i] is 0 (0xFFFFFFFF + 1), it has overflown and we should continue
  }
}

void fixed96q32::subtractFractional(uint32 rhs)
{
  if(rhs <= data[0])
  {
    data[0] -= rhs; //data[0], the fractional part, is left untouched
  }
  else //overflow case
  {
    data[0] -= rhs; //data[0], the fractional part, is left untouched
    for(int i = 1; i < 4; i++)
    {
      data[i]--;
      if(data[i] != 0xFFFFFFFFU) break; //only if data[i] is 0xFFFFFFFF (0 - 1), it has overflown and we should continue
    }
  }
}

fixed96q32::operator double() const //this only returns a correct result if the double can hold this value
{
  double result = 0.0;
  
  if(sign())
  {
    double multiplier = 1.0 / 4294967296.0;
    for(int i = 0; i < 4; i++)
    {
      result += multiplier * (~data[i]);
      multiplier *= 4294967296.0;
    }
    return -result - 1.0 / 4294967296.0;
  }
  else
  {
    double multiplier = 1.0 / 4294967296.0;
    for(int i = 0; i < 4; i++)
    {
      result += multiplier * data[i];
      multiplier *= 4294967296.0;
    }
    return result;
  }
}

void fixed96q32::operator=(double d)
{
  bool negative = d < 0.0;
  if(negative) d = -d;
  double multiplier = 4294967296.0;
  for(int i = 0; i < 4; i++)
  {
    data[i] = uint32(d * multiplier);
    multiplier /= 4294967296.0;
  }
  if(negative) negate();
}

void fixed96q32::operator=(const ufixed96q32& o)
{
  data[0] = o.data[0];
  data[1] = o.data[1];
  data[2] = o.data[2];
  data[3] = o.data[3];
}

fixed96q32::operator uint32() const //this only returns a correct result if the double can hold this value
{
  return data[1];
}

void fixed96q32::operator=(uint32 d)
{
  data[0] = 0;
  data[1] = d;
  data[2] = 0;
  data[3] = 0;
}

fixed96q32::operator int() const //this only returns a correct result if the double can hold this value
{
  return (int)data[1];
}

void fixed96q32::operator=(int d)
{
  data[0] = 0;
  data[1] = (uint32)d;
  if(d >= 0)
  {
    data[2] = 0;
    data[3] = 0;
  }
  else
  {
    data[2] = 0xFFFFFFFFU;
    data[3] = 0xFFFFFFFFU;
  }
}

fixed96q32& fixed96q32::operator++()
{
  data[1]++;
  if(data[1] == 0)
  {
    data[2]++;
    if(data[2] == 0)
    {
      data[3]++;
    }
  }
  return *this;
}

fixed96q32 fixed96q32::operator++(int)
{
  fixed96q32 result = *this;
  (*this)++;
  return result;
}

fixed96q32& fixed96q32::operator--()
{
  data[1]--;
  if(data[1] == 0xFFFFFFFFU)
  {
    data[2]--;
    if(data[2] == 0xFFFFFFFFU)
    {
      data[3]--;
    }
  }
  return *this;
}

fixed96q32 fixed96q32::operator--(int)
{
  fixed96q32 result = *this;
  (*this)--;
  return result;
}

void fixed96q32::addLSB()
{
  lpi::addLSB<4>(data);
}

void fixed96q32::subtractLSB()
{
  lpi::subtractLSB<4>(data);
}

void fixed96q32::bit_invert()
{
  lpi::bit_invert<4>(data);
}

void fixed96q32::negate()
{
  lpi::negate<4>(data);
}

bool fixed96q32::sign() const
{
  return data[3] > 2147483647U;
}

void fixed96q32::takeSqrt()
{
  /*
  Note: a good and faster way to take the sqrt is to convert to double and then take the sqrt.
  That has less precision, but that is not always a problem since it's always nearly right for even the largest values.
  */
  if(sign() || isZero())
  {
    makeZero();
    return;
  }

  ufixed96q32 result = *this;
  static const ufixed96q32 mask = ~(ufixed96q32(1U) >> 32U); //only the LSB is 0

  uint32 i;
  for(i = 0; result.isZero(); i++) result >>= 1U;
  result = (*this) >> (2 * i);
  for(;;)
  {
    ufixed96q32 temp = ufixed96q32((*this)>>1U) / result - (result>>1U);
    if((temp & mask).isZero()) break;
    else result += temp;
  }
  
  *this = result;
}

////////////////////////////////////////////////////////////////////////////////

fixed96q32& fixed96q32::operator/=(const fixed96q32& rhs)
{
  //divident / dividor = quotient (= *this)
  /*
  Without precaution, the result will exclude the digits behind the point.
  So to solve: the divident is multiplied with 4 billion compared to the dividor
  */
  bool negative = false; //sign
  if(sign()) { negative = !negative; negate(); }
  ufixed96q32 dividor = rhs;
  if(dividor.sign()) { negative = !negative; dividor.negate(); }
  uint32 dividorMSB = 0U;
  uint32 dividentMSB = data[3]; //MSB's for the divident
  ufixed96q32 divident = ((*this) << 32U);
  
  int shift = 0;
  
  uint32 thisMSB = 0U; //extra uint32 for *this (extra MSB's)
  makeZero();
  
  if(!dividor.isZero()) //avoid division through 0
  {
    /*left shift dividor until it is greater than or equal to divident
    the comparision includes checks for the extra MSBs*/
    while(dividorMSB < dividentMSB || (dividorMSB == dividentMSB && dividor < divident))
    {
      dividorMSB <<= 1; if(dividor.data[3] & 2147483648U) dividorMSB |= 1; else dividorMSB &= 4294967294U;
      dividor <<= 1;
      shift++;
    }

    //if the dividor is now larger than the divident, right shift it again once
    if(dividorMSB > dividentMSB || (dividorMSB == dividentMSB && dividor > divident))
    {
      dividor >>= 1;
      if(dividorMSB & 1U) dividor.data[3] |= 2147483648U; else dividor.data[3] &= 2147483647U;
      dividorMSB >>= 1; dividorMSB &= 2147483647U; //make extra sure the left bit is 0
      
      shift--;
    }
    
    while(shift >= 0)
    {
      //left shift quotient
      thisMSB <<= 1; if(data[3] & 2147483648U) thisMSB |= 1; else thisMSB &= 4294967294U;
      (*this) <<= 1;
      
      if(!(dividorMSB > dividentMSB || (dividorMSB == dividentMSB && dividor > divident))) //if dividor <= divident...
      {
        //subtract dividor from divident, operator- implemented here including the extra MSBs
        {
          bool borrow = false;
          for(int i = 0; i < 4; i++)
          {
            if(borrow)
            {
              borrow = dividor.data[i] >= divident.data[i];
              divident.data[i] -= (dividor.data[i] + 1U);
            }
            else
            {
              borrow = dividor.data[i] > divident.data[i] ;
              divident.data[i] -= dividor.data[i];
            }
          }
          //the MSBs from divident
          if(borrow) dividentMSB -= (dividorMSB + 1U);
          else dividentMSB -= dividorMSB;
        }

        //increment the LSB from the quotient, carrying to the whole left if needed
        addLSB();
        if(isZero()) thisMSB++;
      }
      
      //right shift dividor 
      dividor >>= 1;
      if(dividorMSB & 1U) dividor.data[3] |= 2147483648U; else dividor.data[3] &= 2147483647U;
      dividorMSB >>= 1; dividorMSB &= 2147483647U; //make extra sure the left bit is 0

      shift--;
    }

  if(negative) negate();
  }
  
  return *this;
}

fixed96q32& fixed96q32::operator>>=(uint32 shift)
{
  rightshift_signed<4>(data, shift);
  return *this;
}

fixed96q32& fixed96q32::operator<<=(uint32 shift)
{
  leftshift<4>(data, shift);
  return *this;
}

void fixed96q32::makeZero()
{
  data[0] = data[1] = data[2] = data[3] = 0U;
}

fixed96q32& fixed96q32::operator*=(uint32 rhs)
{
  uint32 result[4];
  
  multiply_signed<4, 4, 1>(result, data, &rhs);
  
  data[0] = result[0];
  data[1] = result[1];
  data[2] = result[2];
  data[3] = result[3];

  return *this;
}

fixed96q32& fixed96q32::operator*=(const fixed96q32& rhs)
{
  //160 bit precision used, and one right shift at the end
  uint32 result[5];
  
  multiply_signed<5, 4, 4>(result, data, rhs.data);
  
  data[0] = result[1];
  data[1] = result[2];
  data[2] = result[3];
  data[3] = result[4];

  return *this;
}

fixed96q32& fixed96q32::operator*=(double rhs)
{
  return operator*=((fixed96q32)rhs);
}

//this one loses the leftmost 32 bits, but shows a simpler version of the multiplication implementation for documentation
void fixed96q32::crudeMultiply(const fixed96q32& rhs)
{
  fixed96q32 a = *this;
  fixed96q32 b = rhs;
  makeZero();
  for(int i = 0; i < 128; i++)
  {
    if(b.getLSB()) (*this) += a;
    a <<= 1;
    b >>= 1;
  }
  
  (*this) >>= 32; //the part behind the comma is smaller
}

bool fixed96q32::operator==(const fixed96q32& rhs) const
{
  return data[0] == rhs.data[0]
      && data[1] == rhs.data[1]
      && data[2] == rhs.data[2]
      && data[3] == rhs.data[3];
}

bool fixed96q32::operator==(uint32 rhs) const
{
  return data[0] == 0U
      && data[1] == rhs
      && data[2] == 0U
      && data[3] == 0U;
}

bool fixed96q32::operator>(const fixed96q32& rhs) const
{
  if(sign() && !rhs.sign()) return false;
  if(!sign() && rhs.sign()) return true;
  
  if(data[3] == rhs.data[3])
  {
    if(data[2] == rhs.data[2])
    {
      if(data[1] == rhs.data[1])
      {
        return data[0] > rhs.data[0];
      }
      else return data[1] > rhs.data[1];
    }
    else return data[2] > rhs.data[2];
  }
  else return data[3] > rhs.data[3];
}

bool fixed96q32::operator<(const fixed96q32& rhs) const
{
  if(sign() && !rhs.sign()) return true;
  if(!sign() && rhs.sign()) return false;
  
  if(data[3] == rhs.data[3])
  {
    if(data[2] == rhs.data[2])
    {
      if(data[1] == rhs.data[1])
      {
        return data[0] < rhs.data[0];
      }
      else return data[1] < rhs.data[1];
    }
    else return data[2] < rhs.data[2];
  }
  else return data[3] < rhs.data[3];
}

bool fixed96q32::operator>=(const fixed96q32& rhs) const
{
  return !((*this) > rhs);
}

bool fixed96q32::operator<=(const fixed96q32& rhs) const
{
  return !((*this) > rhs);
}

fixed96q32& fixed96q32::operator&=(const fixed96q32& rhs)
{
  for(int i = 0; i < 4; i++)
  {
    data[i] &= rhs.data[i];
  }
  
  return *this;
}

fixed96q32& fixed96q32::operator|=(const fixed96q32& rhs)
{
  for(int i = 0; i < 4; i++)
  {
    data[i] |= rhs.data[i];
  }
  
  return *this;
}

fixed96q32& fixed96q32::operator^=(const fixed96q32& rhs)
{
  for(int i = 0; i < 4; i++)
  {
    data[i] ^= rhs.data[i];
  }
  
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

fixed96q32 operator-(const fixed96q32& a)
{
  fixed96q32 result = a;
  result.negate();
  return result;
}

fixed96q32 operator~(const fixed96q32& a)
{
  fixed96q32 result = a;
  result.bit_invert();
  return result;
}


fixed96q32 operator+(const fixed96q32& a, const fixed96q32& b)
{
  fixed96q32 result = a;
  result += b;
  return result;
}

fixed96q32 operator-(const fixed96q32& a, const fixed96q32& b)
{
  fixed96q32 result = a;
  result -= b;
  return result;
}

fixed96q32 operator+(const fixed96q32& a, uint32& b)
{
  fixed96q32 result = a;
  result += b;
  return result;
}

fixed96q32 operator+(uint32& a, const fixed96q32& b)
{
  fixed96q32 result = b;
  result += a;
  return result;
}

fixed96q32 operator-(const fixed96q32& a, uint32& b)
{
  fixed96q32 result = a;
  result -= b;
  return result;
}

fixed96q32 operator>>(const fixed96q32& a, uint32 b)
{
  fixed96q32 result = a;
  result >>= b;
  return result;
}

fixed96q32 operator<<(const fixed96q32& a, uint32 b)
{
  fixed96q32 result = a;
  result <<= b;
  return result;
}

fixed96q32 operator&(const fixed96q32& a, const fixed96q32& b)
{
  fixed96q32 result = a;
  result &= b;
  return result;
}

fixed96q32 operator|(const fixed96q32& a, const fixed96q32& b)
{
  fixed96q32 result = a;
  result |= b;
  return result;
}

fixed96q32 operator^(const fixed96q32& a, const fixed96q32& b)
{
  fixed96q32 result = a;
  result ^= b;
  return result;
}

fixed96q32 operator*(const fixed96q32& a, const fixed96q32& b)
{
  fixed96q32 result = a;
  result *= b;
  return result;
}

fixed96q32 operator*(const fixed96q32& a, uint32 b)
{
  fixed96q32 result = a;
  result *= b;
  return result;
}

fixed96q32 operator*(uint32 a, const fixed96q32& b)
{
  fixed96q32 result = b;
  result *= a;
  return result;
}


fixed96q32 operator*(const fixed96q32& a, double b)
{
  fixed96q32 result = a;
  result *= b;
  return result;
}

fixed96q32 operator*(double a, const fixed96q32& b)
{
  fixed96q32 result = b;
  result *= a;
  return result;
}

fixed96q32 operator/(const fixed96q32& a, const fixed96q32& b)
{
  fixed96q32 result = a;
  result /= b;
  return result;
}
bool operator==(uint32 a, const fixed96q32& b)
{
  return b == a;
}

fixed96q32 sqrt(const fixed96q32& a)
{
  /*
  Note: a good and faster way to take the sqrt is to convert to double and then take the sqrt.
  That has less precision, but that is not always a problem since it's always nearly right for even the largest values.
  */
  fixed96q32 result = a;
  result.takeSqrt();
  return result;
}

bool sign(const fixed96q32& a)
{
  return a.sign();
}

fixed96q32 abs(const fixed96q32& a)
{
  if(a.sign()) return -a;
  else return a;
}


} //namespace lpi

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


//test
#if 0
#include <iostream>

int main(){}

class Testfixed96q32
{
  public:
  
  void testaddsub()
  {
    std::cout.precision(20);
    
    std::cout << "testing addition and subtraction" << std::endl;
    
    lpi::fixed96q32 n = -5.0;
    lpi::fixed96q32 p = +5.0;
    
    std::cout<<(double)(p + p)<<std::endl;
    std::cout<<(double)(p + n)<<std::endl;
    std::cout<<(double)(n + p)<<std::endl;
    std::cout<<(double)(n + n)<<std::endl;
    std::cout<<(double)(p - p)<<std::endl;
    std::cout<<(double)(p - n)<<std::endl;
    std::cout<<(double)(n - p)<<std::endl;
    std::cout<<(double)(n - n)<<std::endl;
    
    std::cout << std::endl;
    
    lpi::fixed96q32 f = 5.5;
    lpi::fixed96q32 g = -5.5;
    lpi::uint32 i = 5;
    lpi::uint32 j = 6;

    std::cout<<(double)(f + i)<<std::endl;
    std::cout<<(double)(f - i)<<std::endl;
    std::cout<<(double)(g + i)<<std::endl;
    std::cout<<(double)(g - i)<<std::endl;
    std::cout<<(double)(f + j)<<std::endl;
    std::cout<<(double)(f - j)<<std::endl;
    std::cout<<(double)(g + j)<<std::endl;
    std::cout<<(double)(g - j)<<std::endl;
    
    std::cout<<std::endl;
  }
  
  void testsmallmul()
  {
    std::cout.precision(20);
    
    std::cout << "testing small multiplication" << std::endl;
    
    lpi::fixed96q32 a = 0.015625;
    lpi::fixed96q32 b = 0.5;
    lpi::fixed96q32 c = -0.015625;
    lpi::fixed96q32 d = -0.5;
    
    std::cout<<(double)(a*a)<<std::endl;
    std::cout<<(double)(b*b)<<std::endl;
    std::cout<<(double)(c*c)<<std::endl;
    std::cout<<(double)(d*d)<<std::endl;
    std::cout<<std::endl;
    std::cout<<(double)(a*b)<<std::endl;
    std::cout<<(double)(a*d)<<std::endl;
    std::cout<<(double)(c*b)<<std::endl;
    std::cout<<(double)(c*d)<<std::endl;
    std::cout<<std::endl;
    std::cout<<(double)(b*a)<<std::endl;
    std::cout<<(double)(d*a)<<std::endl;
    std::cout<<(double)(b*c)<<std::endl;
    std::cout<<(double)(d*c)<<std::endl;
    
    std::cout<<std::endl;
    
  }
  
  void testlargemul()
  {
    std::cout.precision(30);
    
    std::cout << "testing large multiplication" << std::endl;
    
    lpi::fixed96q32 a = 8000000000.5;
    lpi::fixed96q32 b = 9000000000.5;
    lpi::fixed96q32 c = -8000000000.5;
    lpi::fixed96q32 d = -9000000000.5;
    
    std::cout<<(double)(a*b)<<std::endl;
    std::cout<<(double)(a*d)<<std::endl;
    std::cout<<(double)(c*b)<<std::endl;
    std::cout<<(double)(c*d)<<std::endl;
    
    lpi::fixed96q32 e = -614.488;
    std::cout<<(double)(e*e)<<std::endl;
    
    std::cout<<std::endl;
  }
  
  void testintmul()
  {
    std::cout.precision(30);
    
    std::cout << "testing multiplication with integer" << std::endl;
    
    lpi::fixed96q32 a = 8000000000.0;
    lpi::fixed96q32 b = -8000000000.0;
    lpi::uint32 c = 5;
    lpi::uint32 d = -5;
    
    
    std::cout<<(double)(a * c)<<std::endl;
    std::cout<<(double)(a * d)<<std::endl;
    std::cout<<(double)(b * c)<<std::endl;
    std::cout<<(double)(b * d)<<std::endl;
    
    std::cout<<std::endl;
  }
  
  void testdoublemul()
  {
    std::cout.precision(30);
    
    std::cout << "testing multiplication with doubles" << std::endl;
    
    lpi::fixed96q32 a = 8000000000.0;
    
    std::cout<<(double)(a * 0.5)<<std::endl;
    std::cout<<(double)(a * -1.5)<<std::endl;
    std::cout<<(double)(a * 1000.0)<<std::endl;
    std::cout<<(double)(a * -1.0)<<std::endl;
    std::cout<<(double)(a * 0.0)<<std::endl;
    
    std::cout << std::endl;
    lpi::fixed96q32 b = -8000000000.0;
    
    std::cout<<(double)(b * 0.5)<<std::endl;
    std::cout<<(double)(b * -1.5)<<std::endl;
    std::cout<<(double)(b * 1000.0)<<std::endl;
    std::cout<<(double)(b * -1.0)<<std::endl;
    std::cout<<(double)(b * 0.0)<<std::endl;
    
    std::cout<<std::endl;
  }
  
  void testltgt()
  {
    std::cout.precision(30);
    
    std::cout << "testing lesser than and greater than" << std::endl;
    
    lpi::fixed96q32 a = +5.0;
    lpi::fixed96q32 b = +6.0;
    lpi::fixed96q32 c = -5.0;
    lpi::fixed96q32 d = -6.0;
    
    std::cout<<(a < b)<<std::endl;
    std::cout<<(a < c)<<std::endl;
    std::cout<<(c < d)<<std::endl;
    std::cout<<(b < a)<<std::endl;
    std::cout<<(c < a)<<std::endl;
    std::cout<<(d < c)<<std::endl;
    std::cout << std::endl;
    std::cout<<(a > b)<<std::endl;
    std::cout<<(a > c)<<std::endl;
    std::cout<<(c > d)<<std::endl;
    std::cout<<(b > a)<<std::endl;
    std::cout<<(c > a)<<std::endl;
    std::cout<<(d > c)<<std::endl;
    std::cout<<std::endl;
  }
  
  void testsqrt()
  {
    std::cout.precision(30);
    
    std::cout << "testing square root" << std::endl;
    
    lpi::fixed96q32 a = 1600000000.0;
    a.takeSqrt();
    std::cout<<(double)a<<std::endl;
    
    lpi::fixed96q32 b = 9.0;
    b.takeSqrt();
    std::cout<<(double)b<<std::endl;
    
    lpi::fixed96q32 c = 2.0;
    c.takeSqrt();
    std::cout<<(double)c<<std::endl;
    
    std::cout<<std::endl;
  }
  
  void testdiv()
  {
    std::cout.precision(30);
    
    std::cout << "testing division" << std::endl;
    
    lpi::fixed96q32 a = 3.0;
    a /= 2.0;
    std::cout<<(double)a<<std::endl;
    
    lpi::fixed96q32 b = -16000000000000.0;
    b /= 2.0;
    std::cout<<(double)b<<std::endl;
    
    lpi::fixed96q32 c = 0.005;
    c /= 2.0;
    std::cout<<(double)c<<std::endl;
    
    
    lpi::fixed96q32 e = 4U;
    e <<= 32U;
    e /= 2.0;
    e >>= 32U;
    std::cout<<(double)e<<std::endl;
    
    lpi::fixed96q32 d = 4U;
    d <<= 64U;
    d /= 2.0;
    d >>= 64U;
    std::cout<<(double)d<<std::endl;
    
    std::cout<<std::endl;
  }
  
  void testint()
  {
    std::cout.precision(30);
    
    std::cout << "testing conversion from/to signed int" << std::endl;
    
    int a = 5;
    lpi::fixed96q32 a128 = a;
    std::cout << (double)a128 << std::endl;
    int b = -5;
    lpi::fixed96q32 b128 = b;
    std::cout << (double)b128 << std::endl;
  }
  
  Testfixed96q32()
  {
    testaddsub();
    testsmallmul();
    testlargemul();
    testintmul();
    testdoublemul();
    testltgt();
    testsqrt();
    testdiv();
    testint();
    
    /*
    The console output of the tests should be:
    
    testing addition and subtraction
    10
    0
    0
    -10
    0
    10
    -10
    0
    
    10.5
    0.5
    -0.5
    -10.5
    11.5
    -0.5
    0.5
    -11.5
    
    testing small multiplication
    0.000244140625
    0.25
    0.000244140625
    0.25
    
    0.0078125
    -0.0078125
    -0.0078125
    0.0078125
    
    0.0078125
    -0.0078125
    -0.0078125
    0.0078125
    
    testing large multiplication
    72000000000000000000
    -72000000000000000000
    -72000000000000000000
    72000000000000000000
    377595.502143871737644076347351
    
    testing multiplication with integer
    40000000000
    -40000000000
    -40000000000
    40000000000
    
    testing multiplication with doubles
    4000000000
    -12000000000
    8000000000000
    -8000000000
    0
    
    -4000000000
    12000000000
    -8000000000000
    8000000000
    0
    
    testing lesser than and greater than
    1
    0
    0
    0
    1
    1
    
    0
    1
    1
    1
    0
    0
    
    testing square root
    40000
    3
    1.41421356215141713619232177734
    
    testing division
    1.5
    -8000000000000
    0.0024999999441206455230712890625
    2
    2
    
    testing conversion from/to signed int
    5
    -5
    */
  }
} testfixed96q32;

#endif

