/*++
Copyright (c) 2015 Microsoft Corporation

Module Name:

    predabst_util.h

Abstract:

    Miscellaneous utilities for use by predabst.

Author:

    James Lingard (jchl) 2015-06-30.

Revision History:

--*/
#ifndef _PREDABST_UTIL_H_
#define _PREDABST_UTIL_H_

#include "ast.h"
#include "ast_pp.h"
#include "used_vars.h"
#include "dl_rule.h"
#include "smt_kernel.h"
#include "var_subst.h"

namespace predabst {
	// Returns true if v1 and v2 are equal.
	template<typename V>
	inline bool vector_equals(V const& v1, V const& v2) {
		if (v1.size() != v2.size()) {
			return false;
		}
		for (unsigned i = 0; i < v1.size(); ++i) {
			if (v1.get(i) != v2.get(i)) {
				return false;
			}
		}
		return true;
	}

	// Returns the concatenation of v1 and v2.
	template<typename V>
	inline V vector_concat(V const& v1, V const& v2) {
		V v(v1);
		v.append(v2);
		return v;
	}

	// Returns the concatenation of v1 (specified as a size (n1) and pointer to data (p1)) and v2.
	template<typename V, typename T>
	inline V vector_concat(unsigned n1, T* const* p1, V const& v2) {
		V v(v2.m(), n1, p1);
		v.append(v2);
		return v;
	}

	// Returns the index of the first occurrence of elem in v.
	template<typename V, typename T>
	inline unsigned vector_find(V const& v, T const* elem) {
		for (unsigned i = 0; i < v.size(); ++i) {
			if (v.get(i) == elem) {
				return i;
			}
		}
		UNREACHABLE();
		return UINT_MAX;
	}

	// Returns the intersection of v1 and v2.
	template<typename V>
	inline V vector_intersection(V const& v1, V const& v2) {
		V intersection(v1.m());
		for (unsigned i = 0; i < v1.size(); ++i) {
			if (v2.contains(v1.get(i))) {
				intersection.push_back(v1.get(i));
			}
		}
		return intersection;
	}

	// Returns true if v1 is a (possibly non-strict) subset of v2.
	template<typename V>
	inline bool vector_subset(V const& v1, V const& v2) {
		for (unsigned i = 0; i < v1.size(); ++i) {
			if (!v2.contains(v1.get(i))) {
				return false;
			}
		}
		return true;
	}

	template<typename T>
	inline std::ostream& operator<<(std::ostream& out, vector<T> const& v) {
		for (unsigned i = 0; i < v.size(); ++i) {
			out << v[i];
			if (i + 1 < v.size()) {
				out << ", ";
			}
		}
		return out;
	}

	template<typename T, typename TManager>
	std::ostream& operator<<(std::ostream& out, obj_ref<T, TManager> const& e) {
		params_ref p;
		// Don't use 'let's for sub-expressions.
		p.set_uint("max_depth", UINT_MAX);
		p.set_uint("min_alias_size", UINT_MAX);
		// Don't insert any newlines.
		p.set_bool("single_line", true);
		out << mk_pp(e, e.m(), p);
		return out;
	}

	template<typename T, typename TManager>
	std::ostream& operator<<(std::ostream& out, ref_vector<T, TManager> const& v) {
		TManager& m = v.m();
		for (unsigned i = 0; i < v.size(); ++i) {
			if (v[i]) {
				out << obj_ref<T, TManager>(v[i], m);
			}
			else {
				out << "NULL";
			}
			if (i + 1 < v.size()) {
				out << ", ";
			}
		}
		return out;
	}

	// Converts the type of the argument, which is assumed to contain only variables.
	var_ref_vector to_vars(expr_ref_vector const& exprs);

	// Returns true if the sort of e is bool.
	bool sort_is_bool(expr* e, ast_manager& m);

	// Returns true if the sort of e is int.
	bool sort_is_int(expr* e, ast_manager& m);

	// Returns the list of disjuncts of a (possibly nested) 'or' expression,
	// eliminating those that are literally false.
	expr_ref_vector get_disj_terms(expr_ref const& e);

	// Returns the list of conjuncts of a (possibly nested) 'and' expression,
	// eliminating those that are literally true.
	expr_ref_vector get_conj_terms(expr_ref const& e);

	// Returns the list of addends of a (possibly nested) 'add' expression,
	// eliminating those that are literally zero.
	expr_ref_vector get_additive_terms(expr_ref const& e);

	// Returns the list of multiplicands of a (possibly nested) 'mul' expression,
	// eliminating those that are literally one.
	expr_ref_vector get_multiplicative_factors(expr_ref const& e);

	// Returns not(term), optimizing the case that term is true, false or not(e).
	expr_ref mk_not(expr_ref const& term);

	// Returns or(terms), optimizing the case that terms has 0 or 1 elements.
	expr_ref mk_disj(expr_ref_vector const& terms);

	// Returns and(terms), optimizing the case that terms has 0 or 1 elements.
	expr_ref mk_conj(expr_ref_vector const& terms);

	// Returns add(terms), optimizing the case that terms has 0 or 1 elements.
	expr_ref mk_sum(expr_ref_vector const& terms);

	// Returns mul(terms), optimizing the case that terms has 0 or 1 elements.
	expr_ref mk_prod(expr_ref_vector const& terms);

	// Returns e with each member of vars replaced by the corresponding member of args.
	expr_ref replace_args(expr_ref const& e, var_ref_vector const& vars, expr_ref_vector const& args);

	// Returns a list of the variables and uninterpreted constants that appear in e.
	expr_ref_vector get_all_vars(expr_ref const& e);

	// Returns the list of arguments of the application as an expr_ref_vector.
	expr_ref_vector get_args_vector(app* a, ast_manager& m);

	// Returns a list of variables of the correct types to be arguments to fdecl.
	var_ref_vector get_arg_vars(func_decl* fdecl, ast_manager& m);

	// Returns a list of fresh constants of the correct types to be arguments to fdecl.
	expr_ref_vector get_arg_fresh_consts(func_decl* fdecl, char const* prefix, ast_manager& m);

	// Returns exprs with the index of all variables increased by n.
	expr_ref_vector shift(expr_ref_vector const& exprs, unsigned n);

	// Returns vars with the index of all variables increased by n.
	var_ref_vector shift(var_ref_vector const& vars, unsigned n);

	// Returns e with the index of all variables decreased by n.
	expr_ref inv_shift(expr_ref const& e, unsigned n);

	// Returns exprs with the index of all variables decreased by n.
	expr_ref_vector inv_shift(expr_ref_vector const& exprs, unsigned n);

	// Uses qe_lite to attempt to eliminate from e all uninterpreted constants
	// other than those in vars.
	void quantifier_elimination(expr_ref_vector const& vars, expr_ref& e);

	// Returns fml converted to Negation Normal Form (NNF), as well as with
	// negation of integer (in)equalities eliminated.
	expr_ref to_nnf(expr_ref const& fml);

	// Returns fml converted to Disjunctive Normal Form (DNF), as well as with
	// negation of integer (in)equalities eliminated.
	expr_ref to_dnf(expr_ref const& fml);

	// Returns the set of variables used by r.
	used_vars get_used_vars(datalog::rule const* r);

    // Asserts each of the expressions in exprs.
    void assert_exprs(smt::kernel& solver, expr_ref_vector const& exprs);

	class subst_util {
		ast_manager&      m;
		mutable var_subst m_var_subst;

	public:
		subst_util(ast_manager& m) :
			m(m),
			m_var_subst(m, false) {
		}

		// Apply a substitution vector to an expression, returning the result.
		expr_ref apply(expr* expr, expr_ref_vector const& subst) const {
			expr_ref expr2(m);
			m_var_subst(expr, subst.size(), subst.c_ptr(), expr2);
			return expr2;
		}

		// Apply a substitution vector to an application expression, returning the result.
		app_ref apply(app* app, expr_ref_vector const& subst) const {
			expr_ref expr2(m);
			m_var_subst(app, subst.size(), subst.c_ptr(), expr2);
			return app_ref(to_app(expr2), m);
		}

		// Apply a substitution vector to each expression in a vector of
		// expressions, returning the result.
		expr_ref_vector apply(expr_ref_vector const& exprs, expr_ref_vector const& subst) const {
			expr_ref_vector exprs2(m);
			exprs2.reserve(exprs.size());
			for (unsigned i = 0; i < exprs.size(); ++i) {
				exprs2[i] = apply(exprs[i], subst);
			}
			return exprs2;
		}

		// Returns a substitution vector that maps each variable in vars to the
		// corresponding expression in exprs.
		expr_ref_vector build(unsigned n, var* const* vars, expr* const* exprs) const {
			expr_ref_vector inst(m);
			inst.reserve(n); // note that this is not necessarily the final size of inst
			for (unsigned i = 0; i < n; ++i) {
				unsigned idx = vars[i]->get_idx();
				inst.reserve(idx + 1);
				CASSERT("predabst", !inst.get(idx));
				inst[idx] = exprs[i];
			}
			return inst;
		}

		expr_ref_vector build(var* const* vars, expr_ref_vector const& exprs) const {
			return build(exprs.size(), vars, exprs.c_ptr());
		}

		expr_ref_vector build(var_ref_vector const& vars, expr* const* exprs) const {
			return build(vars.size(), vars.c_ptr(), exprs);
		}

		expr_ref_vector build(var_ref_vector const& vars, expr_ref_vector const& exprs) const {
			CASSERT("predabst", vars.size() == exprs.size());
			return build(vars.size(), vars.c_ptr(), exprs.c_ptr());
		}

		expr_ref_vector build(var_ref_vector const& vars, var_ref_vector const& exprs) const {
			CASSERT("predabst", vars.size() == exprs.size());
			return build(vars.size(), vars.c_ptr(), (expr* const*)exprs.c_ptr());
		}
	};
}

#endif /* _PREDABST_UTIL_H */
