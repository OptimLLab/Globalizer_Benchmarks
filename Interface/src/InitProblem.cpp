/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//             LOBACHEVSKY STATE UNIVERSITY OF NIZHNY NOVGOROD             //
//                                                                         //
//                       Copyright (c) 2016 by UNN.                        //
//                          All Rights Reserved.                           //
//                                                                         //
//  File:      init_problem.cpp                                            //
//                                                                         //
//  Purpose:   Source file for program                                     //
//                                                                         //
//  Author(s): Sysoyev A., Lebedev I., Sovrasov V.                         //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#include "InitProblem.h"

int InitProblem(ProblemManager& problemManager, IProblem*& problem, std::string libPath)
{
  if (problemManager.LoadProblemLibrary(libPath) != ProblemManager::OK_)
  {
    //сообщение об ошибке печатает manager
    return 1;
  }

  IProblem* baseProblem = problemManager.GetProblem();

  problem = baseProblem;


 
  return 0;
}
// - end of file ----------------------------------------------------------------------------------
