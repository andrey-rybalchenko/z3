/*++
Copyright (c) 2015 Microsoft Corporation

Module Name:

    predabst_input.h

Abstract:

    Predabst input processing.

Author:

    James Lingard (jchl) 2015-06-30.

Revision History:

--*/
#ifndef _PREDABST_INPUT_H_
#define _PREDABST_INPUT_H_

#include "predabst_rule.h"
#include "ast.h"
#include "fixedpoint_params.hpp"

namespace predabst {
    struct input {
        vector<symbol_info*>   m_symbols;
        vector<template_info*> m_templates;
        vector<rule_info*>     m_rules;
        var_ref_vector         m_template_vars;
        expr_ref               m_template_extras;

        input(ast_manager& m) :
            m_template_vars(m),
            m_template_extras(m) {
        }

        ~input() {
            for (unsigned i = 0; i < m_symbols.size(); ++i) {
                dealloc(m_symbols[i]);
            }
            for (unsigned i = 0; i < m_templates.size(); ++i) {
                dealloc(m_templates[i]);
            }
            for (unsigned i = 0; i < m_rules.size(); ++i) {
                dealloc(m_rules[i]);
            }
        }
    };

    struct input_stats {
        unsigned m_num_symbols;
        unsigned m_num_templates;
        unsigned m_num_rules;
        unsigned m_num_template_params;
        unsigned m_num_explicit_arguments;
        unsigned m_num_named_arguments;
        unsigned m_num_initial_predicates;

        input_stats() { reset(); }
        void reset() { memset(this, 0, sizeof(*this)); }

        void update(statistics& st) {
#define UPDATE_STAT(NAME) st.update(#NAME, m_ ## NAME)
            UPDATE_STAT(num_symbols);
            UPDATE_STAT(num_templates);
            UPDATE_STAT(num_rules);
            UPDATE_STAT(num_template_params);
            UPDATE_STAT(num_explicit_arguments);
            UPDATE_STAT(num_named_arguments);
            UPDATE_STAT(num_initial_predicates);
        }
    };

    input* make_input(datalog::rule_set& rules, input_stats& stats, fixedpoint_params const& fp_params);
}

#endif /* _PREDABST_INPUT_H */
