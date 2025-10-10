#pragma once

#include <string>
#include <vector>
#include <variant>

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

using IOptVariantType = std::variant<int, double, std::string>;

class TPythonModuleWrapper
{
protected:
  int mDimension;
  PyObject* mPFunc;
  std::vector<double> mUpperBound;
  std::vector<double> mLowerBound;

  PyObject* pInstance;
  PyObject* pClass;
  PyObject* pModule;
  PyObject* funcClass;

  

  PyObject* VectorToTuple(std::vector<IOptVariantType> param);

public:
  TPythonModuleWrapper(const std::string& module_path, std::vector<IOptVariantType> param = { 2 },
    std::string functionScriptName = "rastrigin", std::string functionClassName= "Rastrigin");
  int GetDimension() const;
  void GetBounds(std::vector<double>& lower, std::vector<double>& upper) const;
  double EvaluateFunction(const std::vector<double>& y, int fNumber) const;
  std::vector<double> EvaluateAllFunction(const std::vector<double>& y) const;


  virtual int GetNumberOfFunctions() const;
  virtual int GetNumberOfConstraints() const;
  virtual int GetNumberOfCriterions() const;


  /** ћетод возвращает точку из допустимой области
  \param[out] y непрерывные координаты точки
  \param[out] u целочисленые координаты точки
  \param[out] values значение в этой точке
  \return  од ошибки (#PROBLEM_OK или #UNDEFINED)
  */
  virtual int GetStartTrial(std::vector<double>& y, std::vector<std::string>& u, std::vector<double>& values);

  ~TPythonModuleWrapper();
};
