#include "PROTO.h"
#include "node.h"
#include "grammar.h"
#include "bitset.h"

/* PART ONE -- CONSTRUCT NFA -- Cf. Algorithm 3.2 from [Aho&Ullman 77] */

typedef struct _nfaarc {
	int	ar_label;
	int	ar_arrow;
} nfaarc;

typedef struct _nfastate {
	int	st_narcs;
	nfaarc	*st_arc;
} nfastate;

typedef struct _nfa {
	int		nf_type;
	char		*nf_name;
	int		nf_nstates;
	nfastate	*nf_state;
	int		nf_start, nf_finish;
} nfa;

typedef struct _nfagrammar {
	int		gr_nnfas;
	nfa		**gr_nfa;
	labellist	gr_ll;
} nfagrammar;


/* compile functions */
static compile_item PROTO((labellist *, nfa *, node *, int *, int*));
static compile_alt  PROTO((labellist *, nfa *, node *, int *, int*));
static compile_atom PROTO((labellist *, nfa *, node *, int *, int*));
static compile_rhs  PROTO((labellist *, nfa *, node *, int *, int*));
static compile_rule PROTO((nfagrammar *, node *));
/* =================== */

/* PART TWO -- CONSTRUCT DFA -- Algorithm 3.1 from [Aho&Ullman 77] */

typedef struct _ss_arc {
	bitset	sa_bitset;
	int	sa_arrow;
	int	sa_label;
} ss_arc;

typedef struct _ss_state {
	bitset	ss_ss;
	int	ss_narcs;
	ss_arc	*ss_arc;
	int	ss_deleted;
	int	ss_finish;
	int	ss_rename;
} ss_state;

/* state funcitons */
static convert PROTO((dfa *, int, ss_state *));
static simplify PROTO((int, ss_state *));
static printssdfa PROTO((int, ss_state *, int, labellist *, char *));
