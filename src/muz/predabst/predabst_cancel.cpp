/*++
Copyright (c) 2015 Microsoft Corporation

Module Name:

    predabst_cancel.cpp

Abstract:

    Utilities for cancellation of predabst.

Author:

    James Lingard (jchl) 2015-07-01.

Revision History:

--*/
#include "predabst_cancel.h"

// XXX Need to make these ATOMIC blocks atomic with respect to one another.
#define ATOMIC(X) do X while(0)

namespace predabst {
    void cancellation_manager::cancel() {
        ATOMIC({
            m_cancel = true;
            smt::kernel* current_solver = m_current_solver;
            if (current_solver) {
                current_solver->cancel();
            }
        });
    }

    void cancellation_manager::reset_cancel() {
        ATOMIC({
            CASSERT("predabst", !m_current_solver);
            m_cancel = false;
        });
    }

    lbool cancellation_manager::check(smt::kernel* solver, unsigned num_assumptions, expr* const* assumptions) {
        ATOMIC({
            if (m_cancel) {
                STRACE("predabst", tout << "Canceled!\n";);
                throw default_exception("canceled");
            }
            m_current_solver = solver;
        });
        lbool result = solver->check(num_assumptions, assumptions);
        ATOMIC({
            m_current_solver = NULL;
            if (m_cancel) {
                solver->reset_cancel();
                STRACE("predabst", tout << "Canceled!\n";);
                throw default_exception("canceled");
            }
        });
        if (result == l_undef) {
            STRACE("predabst", tout << "Solver failed with " << solver->last_failure_as_string() << "\n";);
            throw default_exception("(underlying-solver " + solver->last_failure_as_string() + ")");
        }
        return result;
    }
}
