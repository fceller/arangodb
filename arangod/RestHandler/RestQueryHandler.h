////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
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
/// @author Dr. Frank Celler
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_REST_HANDLER_REST_QUERY_HANDLER_H
#define ARANGOD_REST_HANDLER_REST_QUERY_HANDLER_H 1

#include "Basics/Common.h"
#include "RestHandler/RestVocbaseBaseHandler.h"

namespace arangodb {

////////////////////////////////////////////////////////////////////////////////
/// @brief document request handler
////////////////////////////////////////////////////////////////////////////////

class RestQueryHandler : public RestVocbaseBaseHandler {
 public:
  RestQueryHandler(application_features::ApplicationServer&, GeneralRequest*,
                   GeneralResponse*);

 public:
  char const* name() const override final { return "RestQueryHandler"; }
  RequestLane lane() const override final {
    return _request->requestType() == rest::RequestType::POST
               ? RequestLane::CLIENT_SLOW
               : RequestLane::CLIENT_FAST;
  }
  RestStatus execute() override;

 protected:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief returns the list of properties
  //////////////////////////////////////////////////////////////////////////////

  bool readQueryProperties();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief returns the list of slow queries
  //////////////////////////////////////////////////////////////////////////////

  bool readQuery(bool slow);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief returns AQL query tracking
  //////////////////////////////////////////////////////////////////////////////

  bool readQuery();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief removes the slow log
  //////////////////////////////////////////////////////////////////////////////

  bool deleteQuerySlow();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief interrupts a named query
  //////////////////////////////////////////////////////////////////////////////

  bool deleteQuery(std::string const& name);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief interrupts a query
  //////////////////////////////////////////////////////////////////////////////

  bool deleteQuery();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief changes the settings
  //////////////////////////////////////////////////////////////////////////////

  bool replaceProperties();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief parses a query
  //////////////////////////////////////////////////////////////////////////////

  bool parseQuery();

  virtual std::string forwardingTarget() override;
};
}  // namespace arangodb

#endif
