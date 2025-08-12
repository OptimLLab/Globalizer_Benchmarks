/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//             LOBACHEVSKY STATE UNIVERSITY OF NIZHNY NOVGOROD             //
//                                                                         //
//                       Copyright (c) 2015 by UNN.                        //
//                          All Rights Reserved.                           //
//                                                                         //
//  File:      Main.cpp                                                    //
//                                                                         //
//  Purpose:   Console version of Globalizer system                        //
//                                                                         //
//  Author(s): Sysoyev A., Barkalov K.                                     //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif



#include <algorithm>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <iostream>

#ifndef WIN32
#include <unistd.h>
#endif

#include "ProblemManager.h"
#include "InitProblem.h"


// ------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  std::cout << "\n\n" << std::endl;
  for (int i = 1; i < argc; i++)
  {
    std::cout << argv[i] << " ";
  }
  std::cout << "\n\n" << std::endl;
  
  ProblemManager manager;
  

  IProblem* problemX2 = 0;
  if (InitProblem(manager, problemX2, "X2.dll"))
  {
    std::cout << "Error during problem initialization\n";
    return 0;
  }
  else
  {
    problemX2->SetDimension(2);
    double y[2] = { 0.5, 0.5 };
    double value = problemX2->CalculateFunctionals(y, 0);
  }

  IProblem* problemRastrigin = 0;
  if (InitProblem(manager, problemRastrigin, "rastrigin.dll"))
  {
    std::cout << "Error during problem initialization\n";
    return 0;
  }
  else
  {
    problemRastrigin->SetDimension(2);
    double y[2] = { 0.5, 0.5 };
    double value = problemRastrigin->CalculateFunctionals(y, 0);
  }

  IProblem* problemRastriginInt = 0;
  if (InitProblem(manager, problemRastriginInt, "rastriginInt.dll"))
  {
    std::cout << "Error during problem initialization\n";
    return 0;
  }
  else
  {
    problemRastriginInt->SetDimension(2);
    double y[2] = { 0.5, 0.5 };
    double value = problemRastriginInt->CalculateFunctionals(y, 0);
  }


  return 0;
}
// - end of file ----------------------------------------------------------------------------------
