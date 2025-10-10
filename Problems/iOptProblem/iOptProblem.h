#ifndef __IOPT_PROBLEM_H__
#define __IOPT_PROBLEM_H__

#include <memory>

#include "IGlobalOptimizationProblem.h"
#include "pymodule_wrapper.h"

class iOptProblem : public IGlobalOptimizationProblem
{
protected:


  int mDimension;
  std::string mPyFilePath;
  std::string functionScriptName;
  std::string functionClassName;

  bool mIsInitialized;

  std::shared_ptr<TPythonModuleWrapper> mFunction;
  void* mLibpython_handle;

public:

  iOptProblem();

  virtual int SetConfigPath(const std::string& configPath);
  virtual int SetDimension(int dimension);
  virtual int GetDimension() const;
  virtual int Initialize();

  virtual void GetBounds(std::vector<double>& upper, std::vector<double>& lower);
  virtual int GetOptimumValue(double& value) const;
  virtual int GetOptimumPoint(std::vector<double>& x, std::vector<std::string>& u) const;

  virtual int GetNumberOfFunctions() const;
  virtual int GetNumberOfConstraints() const;
  virtual int GetNumberOfCriterions() const;

  virtual double CalculateFunctionals(const std::vector<double>& y, std::vector<std::string>& u, int fNumber);
  /** Метод, вычисляющий все функции задачи

\param[in] y непрерывные координаты точки, в которой необходимо вычислить значение
\param[in] u целочисленые координаты точки, в которой необходимо вычислить значение
\return Значение функций
*/
  virtual std::vector<double> CalculateAllFunctionals(const std::vector<double>& y, std::vector<std::string>& u);

  /** ћетод возвращает точку из допустимой области
  \param[out] y непрерывные координаты точки
  \param[out] u целочисленые координаты точки
  \param[out] values значение в этой точке
  \return  од ошибки (#PROBLEM_OK или #UNDEFINED)
  */
  virtual int GetStartTrial(std::vector<double>& y, std::vector<std::string>& u, std::vector<double>& values);

  /** Метод задает параметры задачи
\param[in] name имя параметра
\param[in] value значение параметра
\return Код ошибки (#PROBLEM_OK или #UNDEFINED)
*/
  virtual int SetParameter(std::string name, std::string value);

  /** Метод задает параметры задачи
  \param[in] name имя параметра
  \param[in] value значение параметра
  \return Код ошибки (#PROBLEM_OK или #UNDEFINED)
  */
  //virtual int SetParameter(std::string name, std::any value);

  /** Метод задает параметры задачи
  \param[in] name имя параметра
  \param[in] value значение параметра
  \return Код ошибки (#PROBLEM_OK или #UNDEFINED)
  */
  virtual int SetParameter(std::string name, void* value);

  /** Метод возвращает параметры задачи
  \param[out] names имена параметраметров
  \param[out] values значениея параметров
  \return Код ошибки (#PROBLEM_OK или #UNDEFINED)
  */
  virtual void GetParameters(std::vector<std::string>& names, std::vector<std::string>& values);

  ~iOptProblem();
};

extern "C" LIB_EXPORT_API IGlobalOptimizationProblem* create();
extern "C" LIB_EXPORT_API void destroy(IGlobalOptimizationProblem* ptr);

#endif
// - end of file ----------------------------------------------------------------------------------
