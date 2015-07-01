/*++
Copyright (c) 2015 Microsoft Corporation

Module Name:

    predabst_context.h

Abstract:

    Predicate abstraction context.

Author:

    James Lingard (jchl) 2015-06-30.

Revision History:

--*/
#ifndef _PREDABST_CONTEXT_H_
#define _PREDABST_CONTEXT_H_

#include "ast.h"
#include "lbool.h"
#include "statistics.h"
#include "dl_engine_base.h"

namespace predabst {
    class dl_interface : public datalog::engine_base {
        class imp;
        datalog::context& m_ctx;
        scoped_ptr<imp>   m_imp;
    public:
        dl_interface(datalog::context& ctx);
        virtual lbool query(expr* query);
        virtual void cancel();
        virtual void cleanup();
        virtual void reset_statistics();
        virtual void collect_statistics(statistics& st) const;
        virtual void display_certificate(std::ostream& out) const;
        virtual expr_ref get_answer();
        virtual model_ref get_model();
    };
}

#endif /* _PREDABST_CONTEXT_H_ */
