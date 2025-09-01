#pragma once

#include <string>
#include <vector>

#ifndef WIN32
  #ifdef _DEBUG
    #undef _DEBUG
    #include "Python.h"
    #define _DEBUG
  #else
    #include "Python.h"
  #endif
#else
  #ifdef _DEBUG
    #undef _DEBUG
    #include "python.h"
    #define _DEBUG
  #else
    #include "python.h"
  #endif
#endif


class TPythonModuleWrapper
{
protected:
  int mDimension;
  PyObject* mPFunc;
  std::vector<double> mUpperBound;
  std::vector<double> mLowerBound;

public:
  TPythonModuleWrapper(const std::string& module_path);
  int GetDimension() const;
  void GetBounds(std::vector<double>& lower, std::vector<double>& upper) const;
  double EvaluateFunction(const std::vector<double>& y) const;
  ~TPythonModuleWrapper();
};
