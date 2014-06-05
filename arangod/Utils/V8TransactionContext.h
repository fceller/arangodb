////////////////////////////////////////////////////////////////////////////////
/// @brief V8 transaction context
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2004-2013 triAGENS GmbH, Cologne, Germany
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
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Jan Steemann
/// @author Copyright 2011-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef TRIAGENS_UTILS_V8TRANSACTION_CONTEXT_H
#define TRIAGENS_UTILS_V8TRANSACTION_CONTEXT_H 1

#include "Basics/Common.h"

#include <v8.h>

#include "V8/v8-globals.h"

#include "Utils/Transaction.h"

namespace triagens {
  namespace arango {

    class V8TransactionContext : public TransactionBase {

// -----------------------------------------------------------------------------
// --SECTION--                                        class V8TransactionContext
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief create the context
////////////////////////////////////////////////////////////////////////////////

        V8TransactionContext ()
          : _v8g(static_cast<TRI_v8_global_t*>(v8::Isolate::GetCurrent()->GetData())) {
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief destroy the context
////////////////////////////////////////////////////////////////////////////////

        ~V8TransactionContext () {
        }

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief check whether the transaction is embedded
////////////////////////////////////////////////////////////////////////////////

        static inline bool IsEmbedded () {
          TRI_v8_global_t* v8g = static_cast<TRI_v8_global_t*>(v8::Isolate::GetCurrent()->GetData());
          return (v8g->_currentTransaction != nullptr);
        }

// -----------------------------------------------------------------------------
// --SECTION--                                               protected functions
// -----------------------------------------------------------------------------

      protected:

////////////////////////////////////////////////////////////////////////////////
/// @brief get parent transaction (if any)
////////////////////////////////////////////////////////////////////////////////

        inline TRI_transaction_t* getParentTransaction () const {
          if (_v8g->_currentTransaction != nullptr) {
            return (TRI_transaction_t*) _v8g->_currentTransaction;
          }

          return nullptr;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief register the transaction in the context
////////////////////////////////////////////////////////////////////////////////

        inline int registerTransaction (TRI_transaction_t* const trx) const {
          _v8g->_currentTransaction = trx;

          return TRI_ERROR_NO_ERROR;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief unregister the transaction from the context
////////////////////////////////////////////////////////////////////////////////

        inline int unregisterTransaction () const {
          _v8g->_currentTransaction = nullptr;

          return TRI_ERROR_NO_ERROR;
        }

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief v8 global context
////////////////////////////////////////////////////////////////////////////////

        TRI_v8_global_t* _v8g;

    };

  }
}

#endif

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|/// @page\\|// --SECTION--\\|/// @\\}"
// End:
