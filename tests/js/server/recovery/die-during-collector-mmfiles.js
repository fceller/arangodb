/* jshint globalstrict:false, strict:false, unused : false */
/* global assertEqual, assertFalse */
// //////////////////////////////////////////////////////////////////////////////
// / @brief tests for transactions
// /
// / @file
// /
// / DISCLAIMER
// /
// / Copyright 2010-2012 triagens GmbH, Cologne, Germany
// /
// / Licensed under the Apache License, Version 2.0 (the "License")
// / you may not use this file except in compliance with the License.
// / You may obtain a copy of the License at
// /
// /     http://www.apache.org/licenses/LICENSE-2.0
// /
// / Unless required by applicable law or agreed to in writing, software
// / distributed under the License is distributed on an "AS IS" BASIS,
// / WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// / See the License for the specific language governing permissions and
// / limitations under the License.
// /
// / Copyright holder is triAGENS GmbH, Cologne, Germany
// /
// / @author Jan Steemann
// / @author Copyright 2013, triAGENS GmbH, Cologne, Germany
// //////////////////////////////////////////////////////////////////////////////

var db = require('@arangodb').db;
var internal = require('internal');
var jsunity = require('jsunity');

function runSetup () {
  'use strict';
  internal.debugClearFailAt();

  db._drop('UnitTestsRecovery');
  var c = db._create('UnitTestsRecovery'), i;

  for (i = 0; i < 1000; ++i) {
    c.save({ _key: 'test' + i });
  }

  internal.wal.flush(true, true, true);

  for (i = 0; i < 1000; ++i) {
    c.remove('test' + i);
  }
  internal.debugSetFailAt('setCollectionDone');
  internal.wal.flush(true, false, false);
  
  // wait until datafile appears
  while (true) {
    if (c.figures().dead.deletion === 1000) {
      break;
    }
    internal.wait(0.5, false);
  }
  
  c.rotate();

  // wait until compactor has finished
  while (true) {
    if (c.figures().datafiles.count === 0) {
      break;
    }
    internal.wait(0.5, false);
  }

  internal.debugTerminate('crashing server');
}

// //////////////////////////////////////////////////////////////////////////////
// / @brief test suite
// //////////////////////////////////////////////////////////////////////////////

function recoverySuite () {
  'use strict';
  jsunity.jsUnity.attachAssertions();

  return {
    setUp: function () {},
    tearDown: function () {},

    // //////////////////////////////////////////////////////////////////////////////
    // / @brief test whether we can restore the trx data
    // //////////////////////////////////////////////////////////////////////////////

    testDieDuringCollector: function () {
      var c = db._collection('UnitTestsRecovery');

      assertEqual(0, c.count());

      for (var i = 0; i < 1000; ++i) {
        assertFalse(c.exists('test' + i));
      }
    }

  };
}

// //////////////////////////////////////////////////////////////////////////////
// / @brief executes the test suite
// //////////////////////////////////////////////////////////////////////////////

function main (argv) {
  'use strict';
  if (argv[1] === 'setup') {
    runSetup();
    return 0;
  } else {
    jsunity.run(recoverySuite);
    return jsunity.writeDone().status ? 0 : 1;
  }
}
