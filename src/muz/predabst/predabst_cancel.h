/*++
Copyright (c) 2015 Microsoft Corporation

Module Name:

    predabst_cancel.h

Abstract:

    Utilities for cancellation of predabst.

Author:

    James Lingard (jchl) 2015-07-01.

Revision History:

--*/
#ifndef _PREDABST_CANCEL_H_
#define _PREDABST_CANCEL_H_

#include "smt_kernel.h"

namespace predabst {
    class cancellation_manager {
        smt::kernel* volatile m_current_solver;
        bool volatile         m_cancel;

    public:
        cancellation_manager() :
            m_current_solver(NULL),
            m_cancel(false) {
        }

        void cancel();
        void reset_cancel();
        lbool check(smt::kernel* solver, unsigned num_assumptions = 0, expr* const* assumptions = 0);
    };
}

#endif /* _PREDABST_CANCEL_H_ */
