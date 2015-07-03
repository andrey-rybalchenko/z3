/*++
Copyright (c) 2015 Microsoft Corporation

Module Name:

    predabst_rule.h

Abstract:

    Predicate abstraction rule.

Author:

    James Lingard (jchl) 2015-06-30.

Revision History:

--*/
#ifndef _PREDABST_RULE_H_
#define _PREDABST_RULE_H_

#include "ast.h"
#include "dl_rule.h"
#include "predabst_util.h"

namespace predabst {
    using datalog::rule;
    class rule_info;

    // Represents a predicate symbol, either templated or not.
    struct fdecl_info {
        func_decl* const m_fdecl;

        fdecl_info(func_decl* fdecl) :
            m_fdecl(fdecl) {
        }

        unsigned hash() const {
            return m_fdecl->hash();
        }

        friend std::ostream& operator<<(std::ostream& out, fdecl_info const* fi) {
            if (fi) {
                out << fi->m_fdecl->get_name() << "/" << fi->m_fdecl->get_arity();
            }
            else {
                out << "<none>";
            }
            return out;
        }
    };

    // Represents a non-templated predicate symbol.
    struct symbol_info : public fdecl_info {
        enum arg_kind { abstracted_arg, explicit_arg };
        
        bool                     const m_is_dwf;
        expr_ref_vector          m_initial_preds;
        expr_ref_vector          m_preds;
        vector<arg_kind>         m_arg_kinds;
        func_decl_ref_vector     m_var_names;
        var_ref_vector           m_abstracted_vars;
        var_ref_vector           m_explicit_vars;
        vector<rule_info const*> m_users;

        symbol_info(func_decl* fdecl, bool is_dwf, ast_manager& m) :
            fdecl_info(fdecl),
            m_is_dwf(is_dwf),
            m_initial_preds(m),
            m_preds(m),
            m_var_names(m),
            m_abstracted_vars(m),
            m_explicit_vars(m) {
            m_arg_kinds.reserve(m_fdecl->get_arity());
            m_var_names.reserve(m_fdecl->get_arity());
        }

        // Returns a list of fresh constants of the correct types to be the abstracted arguments to m_fdecl.
        expr_ref_vector get_fresh_abstracted_args(char const* prefix) const {
            ast_manager& m = m_preds.m();
            expr_ref_vector args = get_arg_fresh_consts(m_fdecl, prefix, m);
            expr_ref_vector abstracted_args(m);
            for (unsigned i = 0; i < m_arg_kinds.size(); ++i) {
                if (m_arg_kinds[i] == abstracted_arg) {
                    abstracted_args.push_back(args.get(i));
                }
            }
            return abstracted_args;
        }
    };

    // Represents a templated predicate symbol.
    struct template_info : public fdecl_info {
        var_ref_vector const m_vars;
        expr_ref       const m_body;

        template_info(func_decl* fdecl, var_ref_vector const& vars, expr_ref const& body) :
            fdecl_info(fdecl),
            m_vars(vars),
            m_body(body) {
        }

        // Returns the body of the template, having substituted args for the parameters to m_fdecl.
        expr_ref get_body_from_args(expr_ref_vector const& args, subst_util& subst) const {
            CASSERT("predabst", args.size() == m_vars.size());
            return subst.apply(m_body, subst.build(m_vars, args));
        }

        // Returns the body of the template, having substituted extras for the extra template parameters.
        expr_ref get_body_from_extras(expr_ref_vector const& extras, subst_util& subst) const {
            return inv_shift(subst.apply(m_body, extras), extras.size());
        }
    };

    // Represents a rule as used by predabst.  Note that:
    // (a) the API exposed by rule_info hides the use of templated predicate symbol by the
    //     rule: the corresponding templates are incorporated into the interpreted body;
    // (b) predabst may decompose a datalog::rule into more than one rule_info, each with
    //     the same set of predicate symbols but with different interpreted bodies, in
    //     order to ensure that the interpreted body of each rule is free of disjunctions.
    class rule_info {
        unsigned             const m_id;
        rule*                const m_rule;
        expr_ref_vector      const m_body;
        symbol_info*         const m_head_symbol;
        vector<symbol_info*> const m_tail_symbols;
        vector<unsigned>     const m_symbol_pos;
        ast_manager&         m;

    public:
        rule_info(unsigned id, rule* r, expr_ref_vector const& body, symbol_info* head_symbol, vector<symbol_info*> const& tail_symbols, vector<unsigned> const& symbol_pos, ast_manager& m) :
            m_id(id),
            m_rule(r),
            m_body(body),
            m_head_symbol(head_symbol),
            m_tail_symbols(tail_symbols),
            m_symbol_pos(symbol_pos),
            m(m) {
        }

        // Returns the number of predicate symbols in the tail of the rule.
        unsigned get_tail_size() const {
            return m_tail_symbols.size();
        }

        // Returns the predicate symbol at the head of the rule, or NULL if
        // the head of the rule is false.
        symbol_info* get_decl() const {
            return m_head_symbol;
        }

        // Returns the i'th predicate symbol in the tail of the rule.
        symbol_info* get_decl(unsigned i) const {
            CASSERT("predabst", i < m_tail_symbols.size());
            return m_tail_symbols[i];
        }

        // Returns the list of actual abstracted arguments to the predicate symbol
        // at the head of the rule, or the empty list if the head of the rule is false.
        expr_ref_vector get_abstracted_args() const;

        // Returns the list of actual explicit arguments to the predicate symbol
        // at the head of the rule, or the empty list if the head of the rule is false.
        expr_ref_vector get_explicit_args() const;

        // Returns the list of actual abstracted arguments to the i'th predicate symbol
        // in the tail of the rule.
        expr_ref_vector get_abstracted_args(unsigned i) const;

        // Returns the list of actual explicit arguments to the i'th predicate symbol
        // in the tail of the rule.
        expr_ref_vector get_explicit_args(unsigned i) const;

        // Returns the interpreted body of the rule, as a list of conjuncts in NNF.
        expr_ref_vector get_body(expr_ref_vector const& template_params, subst_util const& subst) const;

        // Returns the variables used by the rule.
        used_vars get_used_vars() const;

        unsigned hash() const {
            return m_id;
        }

        friend std::ostream& operator<<(std::ostream& out, rule_info const* ri) {
            if (ri) {
                out << ri->m_id;
            }
            else {
                out << "<none>";
            }
            return out;
        }

    private:
        app* get_head() const {
            return m_rule->get_head();
        }

        app* get_tail(unsigned i) const {
            CASSERT("predabst", i < m_symbol_pos.size());
            return m_rule->get_tail(m_symbol_pos[i]);
        }
    };
}

#endif /* _PREDABST_RULE_H_ */
