////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2018 ArangoDB GmbH, Cologne, Germany
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
/// @author Heiko Kernbach
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_AQL_LIMIT_STATS_H
#define ARANGOD_AQL_LIMIT_STATS_H

#include <cstddef>
#include "ExecutionStats.h"


namespace arangodb {
namespace aql {

class LimitStats {
 public:
  LimitStats() noexcept : _fullCount(0) {}

  LimitStats(LimitStats const&) = default;
  LimitStats& operator=(LimitStats const&) = default;

  // It is relied upon that other._fullcount is zero after the move!
  LimitStats(LimitStats&& other) noexcept : _fullCount(other._fullCount) {
    other._fullCount = 0;
  }
  LimitStats& operator=(LimitStats&& other) noexcept {
    _fullCount = other._fullCount;
    other._fullCount = 0;
    return *this;
  };

  void incrFullCount() noexcept { _fullCount++; }
  void incrFullCountBy(size_t amount) noexcept { _fullCount += amount; }

  std::size_t getFullCount() const noexcept { return _fullCount; }

 private:
  std::size_t _fullCount;
};

inline ExecutionStats& operator+=(ExecutionStats& executionStats,
                           LimitStats const& limitStats) noexcept {
  executionStats.fullCount += limitStats.getFullCount();
  return executionStats;
}

inline LimitStats& operator+=(LimitStats& limitStats, LimitStats const& other) noexcept {
  limitStats.incrFullCountBy(other.getFullCount());
  return limitStats;
}

}
}

#endif // ARANGOD_AQL_LIMIT_STATS_H
