////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2019 ArangoDB GmbH, Cologne, Germany
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
/// @author Simon Grätzer
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_LIFEBOAT_LOG_MANAGER_H
#define ARANGOD_LIFEBOAT_LOG_MANAGER_H 1

#include <deque>

#include "Lifeboat/LogEntry.h"

namespace arangodb {
namespace lifeboat {
  
class LogManager final {
  
public:
  
  LogManager();
  
public:
  
  void sendAppendMessages();
  void triggerSnapshotSync();
  
private:
  
  std::deque<LogEntry> _log;
};
  
} // namespace lifeboat
} // namespace arangodb

#endif

