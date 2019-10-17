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
/// @author Markus Pfeiffer
////////////////////////////////////////////////////////////////////////////////

#include "Mocks/Servers.h"
#include "QueryHelper.h"
#include "gtest/gtest.h"

#include "Aql/Query.h"
#include "RestServer/QueryRegistryFeature.h"

#include <velocypack/Buffer.h>
#include <velocypack/Builder.h>
#include <velocypack/Iterator.h>
#include <velocypack/Slice.h>
#include <velocypack/velocypack-aliases.h>

#include "../IResearch/IResearchQueryCommon.h"

#include "Logger/LogMacros.h"

using namespace arangodb;
using namespace arangodb::aql;

namespace arangodb {
namespace tests {
namespace aql {

class RemoveExecutorTest : public ::testing::Test {
 protected:
  mocks::MockAqlServer server;
  TRI_vocbase_t& vocbase;

  std::string const collectionName = "UnitTestCollection";

 public:
  RemoveExecutorTest() : vocbase(server.getSystemDatabase()) {}

  void SetUp() override {
    SCOPED_TRACE("SetUp");
    auto info = VPackParser::fromJson("{\"name\": \"" + collectionName + "\"}");
    auto collection = vocbase.createCollection(info->slice());
    ASSERT_NE(collection.get(), nullptr) << "Failed to create collection";

    std::string createQuery =
        "FOR i IN 1..1000 INSERT { _key: TO_STRING(i), value: i, sortvalue: i "
        "} IN " +
        collectionName;
    AssertQueryHasResult(vocbase, createQuery, VPackSlice::emptyArraySlice());
  }
};

class RemoveExecutorTestPatterns
    : public RemoveExecutorTest,
      public ::testing::WithParamInterface<std::vector<size_t>> {};

TEST_F(RemoveExecutorTest, remove_all_without_return) {
  std::string query =
      std::string("FOR d IN " + collectionName + " REMOVE d IN " + collectionName);

  AssertQueryHasResult(vocbase, query, VPackSlice::emptyArraySlice());

  std::string checkQuery = "FOR i IN " + collectionName + " RETURN i.value";
  AssertQueryHasResult(vocbase, checkQuery, VPackSlice::emptyArraySlice());
}

TEST_F(RemoveExecutorTest, remove_all_with_return) {
  auto const bindParameters = VPackParser::fromJson("{ }");
  std::string allQuery = std::string("FOR d IN " + collectionName + " RETURN d");

  auto allDocs = executeQuery(vocbase, allQuery, bindParameters);
  ASSERT_TRUE(allDocs.ok());

  std::string query =
      std::string("FOR d IN " + collectionName + " REMOVE d IN " +
                  collectionName + " RETURN OLD");
  AssertQueryHasResult(vocbase, query, allDocs.data->slice());
}

TEST_F(RemoveExecutorTest, remove_every_third_without_return) {
  std::string query = std::string("FOR d IN " + collectionName +
                                  " FILTER d.value % 3 == 0 REMOVE d IN " + collectionName);

  AssertQueryHasResult(vocbase, query, VPackSlice::emptyArraySlice());

  std::string checkQuery =
      "FOR d IN " + collectionName + " FILTER d.value % 3 == 0 RETURN d.value";
  AssertQueryHasResult(vocbase, checkQuery, VPackSlice::emptyArraySlice());
  // TODO: check that everything else is still ther?
}

TEST_F(RemoveExecutorTest, remove_every_third_with_return) {
  auto const bindParameters = VPackParser::fromJson("{ }");
  std::string allQuery = std::string("FOR d IN " + collectionName +
                                     " FILTER d.value % 3 == 0 RETURN d");

  auto allDocs = executeQuery(vocbase, allQuery, bindParameters);
  ASSERT_TRUE(allDocs.ok());

  std::string query = std::string("FOR d IN " + collectionName +
                                  " FILTER d.value % 3 == 0 REMOVE d IN " +
                                  collectionName + " RETURN OLD");
  AssertQueryHasResult(vocbase, query, allDocs.data->slice());
}

TEST_F(RemoveExecutorTest, remove_with_key) {
  auto const bindParameters = VPackParser::fromJson("{ }");
  std::string allQuery = std::string("FOR d IN " + collectionName +
                                     " FILTER d.value <= 100 RETURN d");

  auto allDocs = executeQuery(vocbase, allQuery, bindParameters);
  ASSERT_TRUE(allDocs.ok());

  std::string query =
      std::string("FOR i IN 1..100 REMOVE { _key: TO_STRING(i) } IN " +
                  collectionName + " RETURN OLD");
  AssertQueryHasResult(vocbase, query, allDocs.data->slice());
}

}  // namespace aql
}  // namespace tests
}  // namespace arangodb
