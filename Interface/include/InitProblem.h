/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//             LOBACHEVSKY STATE UNIVERSITY OF NIZHNY NOVGOROD             //
//                                                                         //
//                       Copyright (c) 2016 by UNN.                        //
//                          All Rights Reserved.                           //
//                                                                         //
//  File:      init_problem.h                                              //
//                                                                         //
//  Purpose:   Header file for program                                     //
//                                                                         //
//  Author(s): Sysoyev A., Lebedev I., Sovrasov V.                         //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef __INIT_PROBLEM_H__
#define __INIT_PROBLEM_H__


#include "ProblemManager.h"


int InitProblem(ProblemManager& problemManager, IProblem*& problem, std::string libPath);

#endif //__INIT_PROBLEM_H__
// - end of file ----------------------------------------------------------------------------------
