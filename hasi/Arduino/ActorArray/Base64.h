#pragma once

#ifdef ARDUINO
  #include <Arduino.h>
  typedef String B64_PORTABLE_STRING;
#else
  #include <cstring> // memset, memcpy
  #include <string>
  typedef std::string B64_PORTABLE_STRING;
#endif

class SimpleVec {
public:
  SimpleVec();
  SimpleVec(uint32_t length);
  SimpleVec(const B64_PORTABLE_STRING& s);
  SimpleVec(uint8_t* data, uint32_t dataLength);
  SimpleVec(const SimpleVec& other);
  SimpleVec(const SimpleVec& other, uint32_t newLength, uint8_t fill=0);
  SimpleVec& operator=(const SimpleVec &other);
  
  ~SimpleVec();
  
  void setLength(uint32_t newLength);
  void truncate(uint32_t newLength);
  void clear();
  void append(uint8_t element);
  void remove(uint8_t element);
  uint32_t length() const;
  const uint8_t* constData() const;
  uint8_t* data();
  void subData(uint32_t start, uint32_t len);
  B64_PORTABLE_STRING toString() const;
  
private:
  uint32_t _length;
  uint8_t* _data;
  uint32_t _capacity;
  
};

B64_PORTABLE_STRING base64_encode(uint8_t const* buf, unsigned int bufLen);
void base64_decode(const B64_PORTABLE_STRING& encoded_string, SimpleVec& result);

/*
 The MIT License
 
 Copyright (c) 2016 Jens Krueger
 (based base64.cpp and base64.h by René Nyffenegger)
 
 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.

 Copyright notive from base64.h:
 
 Copyright (C) 2004-2008 René Nyffenegger
 
 This source code is provided 'as-is', without any express or implied
 warranty. In no event will the author be held liable for any damages
 arising from the use of this software.
 
 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:
 
 1. The origin of this source code must not be misrepresented; you must not
 claim that you wrote the original source code. If you use this source code
 in a product, an acknowledgment in the product documentation would be
 appreciated but is not required.
 
 2. Altered source versions must be plainly marked as such, and must not be
 misrepresented as being the original source code.
 
 3. This notice may not be removed or altered from any source distribution.
 
 René Nyffenegger rene.nyffenegger@adp-gmbh.ch
 
 */