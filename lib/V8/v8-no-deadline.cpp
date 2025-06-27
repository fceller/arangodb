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
/// @author Wilfried Goesgens
////////////////////////////////////////////////////////////////////////////////

#ifndef USE_V8
#error this file is not supposed to be used in builds with -DUSE_V8=Off
#endif

#include <optional>
#include "v8-deadline.h"

namespace arangodb {

static std::optional<ExternalProcessStatus> stub_getHistoricStatus(
    TRI_pid_t, arangodb::application_features::ApplicationServer&) {
  return std::nullopt;
}

 // arangod dummy implementation doing nothing
ExecutionDeadlineApi g_deadlineApi = {
    .getHistoricStatus = &stub_getHistoricStatus,
    .isExecutionDeadlineReached = []() { return false; },
    .isExecutionDeadlineReachedIsolate = [](v8::Isolate*) { return false; },
    .correctTimeoutToExecutionDeadlineS = [](double timeout) { return timeout; },
    .correctTimeoutToExecutionDeadlineMs = [](std::chrono::milliseconds t) { return t; },
    .correctTimeoutToExecutionDeadlineU32 = [](uint32_t t) { return t; },
    .TRI_InitV8Deadline = [](v8::Isolate*, uint32_t) {},
    .triggerV8DeadlineNow = [](bool) {}
};

void registerExecutionDeadlineApi(const ExecutionDeadlineApi& api) {
  g_deadlineApi = api;
}

}  // namespace arangodb

bool isExecutionDeadlineReached() { return arangodb::g_deadlineApi.isExecutionDeadlineReached(); }
bool isExecutionDeadlineReached(v8::Isolate* i) { return arangodb::g_deadlineApi.isExecutionDeadlineReachedIsolate(i); }

double correctTimeoutToExecutionDeadlineS(double timeoutSeconds) {
  return arangodb::g_deadlineApi.correctTimeoutToExecutionDeadlineS(timeoutSeconds);
} 
std::chrono::milliseconds correctTimeoutToExecutionDeadline(
    std::chrono::milliseconds timeout) {
  return arangodb::g_deadlineApi.correctTimeoutToExecutionDeadlineMs(timeout);
}
uint32_t correctTimeoutToExecutionDeadline(uint32_t timeoutMS) {
  return arangodb::g_deadlineApi.correctTimeoutToExecutionDeadlineU32(timeoutMS);
}
void triggerV8DeadlineNow(bool fromSignal) {
  arangodb::g_deadlineApi.triggerV8DeadlineNow(fromSignal);
}
void TRI_InitV8Deadline(v8::Isolate* isolate, uint32_t timeout) {
  arangodb::g_deadlineApi.TRI_InitV8Deadline(isolate, timeout);
}