﻿/////////////////////////////////////////////////////////////////////////////
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

#include "GlobalOptimizationProblemManager.h"


// ------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  std::cout << "\n\n" << std::endl;
  for (int i = 1; i < argc; i++)
  {
    std::cout << argv[i] << " ";
  }
  std::cout << "\n\n" << std::endl;
  
  GlobalOptimizationProblemManager manager;
  

  IGlobalOptimizationProblem* problemX2 = 0;
  if (InitGlobalOptimizationProblem(manager, problemX2, "X2.dll"))
  {
    std::cout << "Error during problem initialization\n";
    return 0;
  }
  else
  {
    problemX2->SetDimension(2);
    std::vector<double> y = { 0.5, 0.5 };
    std::vector <std::string> u;
    double value = problemX2->CalculateFunctionals(y, u, 0);
  }

  IGlobalOptimizationProblem* problemRastrigin = 0;
  if (InitGlobalOptimizationProblem(manager, problemRastrigin, "rastrigin.dll"))
  {
    std::cout << "Error during problem initialization\n";
    return 0;
  }
  else
  {
    problemRastrigin->SetDimension(2);
    std::vector<double> y = { 0.5, 0.5 };
    std::vector <std::string> u;
    double value = problemRastrigin->CalculateFunctionals(y, u, 0);
  }

  IGlobalOptimizationProblem* problemRastriginC1 = 0;
  if (InitGlobalOptimizationProblem(manager, problemRastriginC1, "rastriginC1.dll"))
  {
    std::cout << "Error during problem initialization\n";
    return 0;
  }
  else
  {
    std::vector<double> y;
    std::vector <std::string> u;
    std::vector<double> val;

    problemRastriginC1->GetStartTrial(y, u, val);

    double value = problemRastriginC1->CalculateFunctionals(y, u, 0);
  }

  IGlobalOptimizationProblem* problemRastriginInt = 0;
  if (InitGlobalOptimizationProblem(manager, problemRastriginInt, "rastriginInt.dll"))
  {
    std::cout << "Error during problem initialization\n";
    return 0;
  }
  else
  {
    problemRastriginInt->SetDimension(2);
    std::vector<double> y = { 0.5};
    std::vector <std::string> u = { "B" };
    double value = problemRastriginInt->CalculateFunctionals(y, u, 0);
  }

  IGlobalOptimizationProblem* problePythonObjective = 0;
  if (InitGlobalOptimizationProblem(manager, problePythonObjective, "python_objective.dll"))
  {
    std::cout << "Error during problem initialization\n";
    return 0;
  }
  else
  {
    //problePythonObjective->SetDimension(2);
    std::vector<double> y (problePythonObjective->GetDimension(), 0.5);
    std::vector <std::string> u = { "B" };
    double value = problePythonObjective->CalculateFunctionals(y, u, 0);
  }

  IGlobalOptimizationProblem* iOptProblem = 0;
  if (InitGlobalOptimizationProblem(manager, iOptProblem, "iOptProblem.dll"))
  {
    std::cout << "Error during problem initialization\n";
    return 0;
  }
  else
  {
    //problePythonObjective->SetDimension(2);
    std::vector<double> y(iOptProblem->GetDimension(), 0.5);
    std::vector <std::string> u = { "B" };
    double value = iOptProblem->CalculateFunctionals(y, u, 0);

    std::vector<std::string> names;
    std::vector<std::string> values;
    iOptProblem->GetParameters(names, values);
    for (int i = 0; i < names.size(); i++)
    {
        std::cout << names[i] << "\t=\t" << values[i] << std::endl;
    }
  }

  IGlobalOptimizationProblem* problemStronginC3 = 0;
  if (InitGlobalOptimizationProblem(manager, problemStronginC3, "stronginC3.dll"))
  {
    std::cout << "Error during problem initialization\n";
    return 0;
  }
  else
  {
    problemStronginC3->SetDimension(2);
    std::vector<double> y = { 0.5, 0.5 };
    std::vector <std::string> u;
    double value = problemStronginC3->CalculateFunctionals(y, u, 0);
  }


  return 0;
}
// - end of file ----------------------------------------------------------------------------------
