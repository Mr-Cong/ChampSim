/*
 *    Copyright 2023 The ChampSim Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TRACEREADER_H
#define TRACEREADER_H

#include <cstdio>
#include <deque>
#include <memory>
#include <string_view>
#include <variant>

#if defined(__GNUG__) && !defined(__APPLE__)
#include <ext/stdio_filebuf.h>
#endif

namespace detail
{
void pclose_file(FILE* f);
}

#include "instruction.h"

class tracereader
{
public:
  const std::string_view trace_string;
  tracereader(uint8_t cpu, std::string_view _ts) : trace_string(_ts), cpu(cpu) {}
  virtual ~tracereader() = default;

  virtual ooo_model_instr operator()() = 0;
  bool eof() const;

protected:
  static FILE* get_fptr(std::string_view fname);

  std::unique_ptr<FILE, decltype(&detail::pclose_file)> fp{get_fptr(trace_string), &detail::pclose_file};
#if defined(__GNUG__) && !defined(__APPLE__)
  __gnu_cxx::stdio_filebuf<char> filebuf{fp.get(), std::ios::in};
#endif

  uint8_t cpu;
  bool eof_ = false;

  constexpr static std::size_t buffer_size = 128;
  constexpr static std::size_t refresh_thresh = 1;
  std::deque<ooo_model_instr> instr_buffer;

  template <typename T>
  void refresh_buffer();

  template <typename T>
  ooo_model_instr impl_get();
};

std::unique_ptr<tracereader> get_tracereader(std::string_view fname, uint8_t cpu, bool is_cloudsuite);

#endif
