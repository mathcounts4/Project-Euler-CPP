//
//  LinearProgramming.c
//  
//
//  Created by David Fink on 3/31/16.
//
//

#include <stdio.h>

/*
 General Linear Programming:
 min (sum(i = 1 to n) a_i x_i)
 subject to constrainsts of the form:
 Cj: sum(i = 1 to n) c_(j,i) x_i >= m_j
*/

/*
 Flow Problem:
 vertex set V, start vertex S, end vertex T, capacities c: VxV -> R.
 find flow f : VxV -> R such that
 f(u,v) = -f(v,u)
 f(u,v) <= c(u,v)
 for each v != s, v != t:
 sum(u in V) f(u,v) = 0
 maximize sum(u in V) f(s,v)
 
 using duality, this is equivalent to:
 min s-t cut: any cut is a subset containing s, not t
 value of the cut is sum(u in C, v in V\C) c(u,v)
*/

/*
 Duality:
 
 max cTx
 Ax <= b
 x >= 0
 
 min yTb
 yTA >= cT
 y >= 0
 
 same solution if either is solvable
 also possible: both unfeasible / one unfeasible, one unbounded
 
 equality constraint -> unbounded dual variable
 <= constraint -> dual var >= 0
 
 var >= 0 -> constraint is >=
 var <= 0 -> constrain is <=
 var unbounded -> constraint is =
*/

/*
 Feasibility:
 Exactly one of these two systems is feasible:
 (1) Ax >= b
 (2) yTA = 0, yTb = 1, y >= 0
*/
