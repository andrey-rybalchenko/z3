/*++
Copyright (c) 2015 Microsoft Corporation

Module Name:

    predabst_rule.cpp

Abstract:

    Bounded PREDABST (symbolic simulation using Z3) rule.

Author:

    James Lingard (jchl) 2015-06-30.

Revision History:

--*/
#include "predabst_util.h"
#include "predabst_rule.h"

namespace datalog {
    expr_ref_vector rule_info::get_abstracted_args() const {
        if (get_decl()) {
            expr_ref_vector args(m);
            for (unsigned i = 0; i < get_decl()->m_arg_kinds.size(); ++i) {
                if (get_decl()->m_arg_kinds[i] == symbol_info::abstracted_arg) {
                    args.push_back(get_head()->get_arg(i));
                }
            }
            return args;
        }
        else {
            return expr_ref_vector(m);
        }
    }

    expr_ref_vector rule_info::get_abstracted_args(unsigned i) const {
        CASSERT("predabst", get_decl(i));
        expr_ref_vector args(m);
        for (unsigned j = 0; j < get_decl(i)->m_arg_kinds.size(); ++j) {
            if (get_decl(i)->m_arg_kinds[j] == symbol_info::abstracted_arg) {
                args.push_back(get_tail(i)->get_arg(j));
            }
        }
        return args;
    }

    expr_ref_vector rule_info::get_explicit_args() const {
        if (get_decl()) {
            expr_ref_vector args(m);
            for (unsigned i = 0; i < get_decl()->m_arg_kinds.size(); ++i) {
                if (get_decl()->m_arg_kinds[i] == symbol_info::explicit_arg) {
                    args.push_back(get_head()->get_arg(i));
                }
            }
            return args;
        }
        else {
            return expr_ref_vector(m);
        }
    }

    expr_ref_vector rule_info::get_explicit_args(unsigned i) const {
        CASSERT("predabst", get_decl(i));
        expr_ref_vector args(m);
        for (unsigned j = 0; j < get_decl(i)->m_arg_kinds.size(); ++j) {
            if (get_decl(i)->m_arg_kinds[j] == symbol_info::explicit_arg) {
                args.push_back(get_tail(i)->get_arg(j));
            }
        }
        return args;
    }

    used_vars rule_info::get_used_vars() const {
        return ::get_used_vars(m_rule);
    }
}
