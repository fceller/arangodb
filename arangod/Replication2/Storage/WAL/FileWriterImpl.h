////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2024 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Business Source License 1.1 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     https://github.com/arangodb/arangodb/blob/devel/LICENSE
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Manuel Pöter
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Replication2/Storage/WAL/IFileWriter.h"

#include <cstdio>
#include <filesystem>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

namespace arangodb::replication2::storage::wal {

#ifndef _WIN32

struct FileWriterImplPosix final : IFileWriter {
  FileWriterImplPosix(std::filesystem::path path);
  ~FileWriterImplPosix();

  auto path() const -> std::string override { return _path.string(); }

  auto append(std::string_view data) -> Result override;

  void truncate(std::uint64_t size) override;

  void sync() override;

  auto getReader() const -> std::unique_ptr<IFileReader> override;

 private:
  std::filesystem::path _path;
  std::uint64_t _size = 0;
  int _file = 0;
};

using FileWriterImpl = FileWriterImplPosix;

#else  // _WIN32

struct FileWriterImplWindows final : IFileWriter {
  FileWriterImplWindows(std::filesystem::path path);
  ~FileWriterImplWindows();

  auto path() const -> std::string override { return _path.string(); }

  auto append(std::string_view data) -> Result override;

  void truncate(std::uint64_t size) override;

  void sync() override;

  auto getReader() const -> std::unique_ptr<IFileReader> override;

 private:
  std::filesystem::path _path;
  HANDLE _file = INVALID_HANDLE_VALUE;
};
using FileWriterImpl = FileWriterImplWindows;

#endif

}  // namespace arangodb::replication2::storage::wal
