////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2017 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Daniel H. Larkin
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_CACHE_METADATA_H
#define ARANGODB_CACHE_METADATA_H

#include "Basics/Common.h"
#include "Basics/ReadWriteSpinLock.h"

#include <atomic>
#include <cstdint>

namespace arangodb {
namespace cache {

class Cache;  // forward declaration

////////////////////////////////////////////////////////////////////////////////
/// @brief Metadata object to facilitate information sharing between individual
/// Cache instances and Manager.
////////////////////////////////////////////////////////////////////////////////
struct Metadata {
  // info about allocated memory
  uint64_t fixedSize;
  uint64_t tableSize;
  uint64_t maxSize;
  uint64_t allocatedSize;
  uint64_t deservedSize;

  // vital information about memory usage
  std::atomic<uint64_t> usage;
  uint64_t softUsageLimit;
  uint64_t hardUsageLimit;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Default constructor for placeholder objects.
  //////////////////////////////////////////////////////////////////////////////
  Metadata();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Initializes record with given information.
  //////////////////////////////////////////////////////////////////////////////
  Metadata(uint64_t usage, uint64_t fixed, uint64_t table, uint64_t max);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Initializes record from an existing record.
  //////////////////////////////////////////////////////////////////////////////
  Metadata(Metadata&& other);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Initializes record from an existing record.
  //////////////////////////////////////////////////////////////////////////////
  Metadata& operator=(Metadata&& other);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Locks the record for reading
  //////////////////////////////////////////////////////////////////////////////
#ifdef ARANGODB_ENABLE_MAINTAINER_MODE
  void readLock() const noexcept { _lock.readLock(__FILE__, __LINE__); }
#else
  void readLock() const noexcept { _lock.readLock(); }
#endif

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Locks the record for writing
  //////////////////////////////////////////////////////////////////////////////
#ifdef ARANGODB_ENABLE_MAINTAINER_MODE
  void writeLock() const noexcept { _lock.writeLock(__FILE__, __LINE__); }
#else
  void writeLock() const noexcept { _lock.writeLock(); }
#endif

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Unlocks the record. Requires record to be read-locked.
  //////////////////////////////////////////////////////////////////////////////
  void readUnlock() const {
    TRI_ASSERT(isLocked());
    _lock.readUnlock();
  }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Unlocks the record. Requires record to be write-locked.
  //////////////////////////////////////////////////////////////////////////////
  void writeUnlock() const noexcept {
    TRI_ASSERT(isWriteLocked());
    _lock.writeUnlock();
  }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Returns true if the record is locked, false otherwise.
  //////////////////////////////////////////////////////////////////////////////
  bool isLocked() const noexcept { return _lock.isLocked(); }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Returns true if the record is write-locked, false otherwise.
  //////////////////////////////////////////////////////////////////////////////
  bool isWriteLocked() const noexcept { return _lock.isWriteLocked(); }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Adjusts usage by the specified amount if it will not violate
  /// limits. Requires record to be read-locked.
  ///
  /// Returns true if adjusted, false otherwise. Used by caches to check-and-set
  /// in a single operation to determine whether they can afford to store a new
  /// value.
  //////////////////////////////////////////////////////////////////////////////
  bool adjustUsageIfAllowed(int64_t usageChange) noexcept;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Sets the soft and hard usage limits. Requires record to be
  /// write-locked.
  //////////////////////////////////////////////////////////////////////////////
  bool adjustLimits(uint64_t softLimit, uint64_t hardLimit) noexcept;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Sets the deserved size. Requires record to be write-locked.
  //////////////////////////////////////////////////////////////////////////////
  uint64_t adjustDeserved(uint64_t deserved) noexcept;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Calculates the new usage limit based on deserved size and other
  /// values. Requires record to be read-locked.
  //////////////////////////////////////////////////////////////////////////////
  uint64_t newLimit() const noexcept;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Checks feasibility of new table size prior to migration. Requires
  /// record to be read-locked.
  ///
  /// If migrating to table of new size would exceed either deserved or maximum
  /// size, then returns false.
  //////////////////////////////////////////////////////////////////////////////
  bool migrationAllowed(uint64_t newTableSize) noexcept;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Sets the table size after migration. Requires record to be
  /// write-locked.
  //////////////////////////////////////////////////////////////////////////////
  void changeTable(uint64_t newTableSize) noexcept;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Checks if cache is migrating. Requires record to be read-locked.
  //////////////////////////////////////////////////////////////////////////////
  bool isMigrating() const noexcept { return _migrating; }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Checks if the cache is resizing. Requires record to be read-locked.
  //////////////////////////////////////////////////////////////////////////////
  bool isResizing() const noexcept { return _resizing; }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Toggles the migrating flag. Requires record to be write-locked.
  //////////////////////////////////////////////////////////////////////////////
  void toggleMigrating() noexcept { _migrating = !_migrating; }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Toggles the resizing flag. Requires record to be write-locked.
  //////////////////////////////////////////////////////////////////////////////
  void toggleResizing() noexcept { _resizing = !_resizing; }

 private:
  mutable basics::ReadWriteSpinLock _lock;
  bool _migrating;
  bool _resizing;
};

};  // end namespace cache
};  // end namespace arangodb

#endif
