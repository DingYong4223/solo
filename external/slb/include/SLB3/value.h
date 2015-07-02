// Copyright (c) 2013, Jose L. Hidalgo Valiño <pplux@pplux.com>
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
#ifndef __SLB3_VALUE__
#define __SLB3_VALUE__

#include "objectheader.h"
#include "info.h"

namespace SLB3 {

  template<class T> struct Value;

  template<class T>
  inline void Push(lua_State *L, T v) { Value<T>::Push(L, v); }

  template<class T>
  inline typename Value<T>::GetType Get(lua_State *L, int index) {
    return Value<T>::Get(L, index);
  }

  template<class T>
  inline void GetInto(lua_State *L, int index, T *arg) {
    *arg = Value<T>::Get(L, index);
  }

  namespace internal {
    template<class T>
    class Class;
    // Given a type T pushes its metatable, it can also trigger the registration
    // of the class if it's the first time this is called for a given type T
    // Registration callback -> Wrapper<T>::Register (which is automatically
    // generated by the macro SLB3_BODY) 
    template<class T>
    void PushMetatable(lua_State *L);
  }
}

#include "internal/value_primitives.h"
#include "internal/value_primitives_ptr.h"
#include "internal/value_arrays.h"
#include "internal/value_instances.h"
#include "internal/functions.h"

#endif
