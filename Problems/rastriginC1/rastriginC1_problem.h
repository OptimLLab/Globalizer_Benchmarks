#ifndef __RASTRIGINC1PROBLEM_H__
#define __RASTRIGINC1PROBLEM_H__

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "IGlobalOptimizationProblem.h"

/// Задача Растригина
class RastriginC1Problem : public IGlobalOptimizationProblem
{
protected:

  /// Размерность задачи
  int mDimension;
  /// Инициализирована ли задача
  bool mIsInitialized;
  /// Максималбная допустимая размерность
  static const int mMaxDimension = 50;

public:

  RastriginC1Problem();

  /** Метод задаёт размерность задачи

  Данный метод должен вызываться перед #Initialize. Размерность должна быть в
  списке поддерживаемых.
  \param[in] dimension размерность задачи
  \return Код ошибки
  */
  virtual int SetDimension(int dimension);
  /// Возвращает размерность задачи, можно вызывать после #Initialize
  virtual int GetDimension() const;
  ///Инициализация задачи
  virtual int Initialize();

  /** Метод возвращает границы области поиска
  */
  virtual void GetBounds(std::vector<double>& lower, std::vector<double>& upper);
  /** Метод возвращает значение целевой функции в точке глобального минимума
  \param[out] value оптимальное значение
  \return Код ошибки (#PROBLEM_OK или #UNDEFINED)
  */
  virtual int GetOptimumValue(double& value) const;
  /** Метод возвращает координаты точки глобального минимума целевой функции
  \param[out] y точка, в которой достигается оптимальное значение
  \return Код ошибки (#PROBLEM_OK или #UNDEFINED)
  */
  virtual int GetOptimumPoint(std::vector<double>&, std::vector<std::string>& u) const;

  /** Метод возвращает число общее функций в задаче (оно равно число ограничений + число критериев)
  \return Число функций
  */
  virtual int GetNumberOfFunctions() const;
  /** Метод возвращает число ограничений в задаче
  \return Число ограничений
  */
  virtual int GetNumberOfConstraints() const;
  /** Метод возвращает число критериев в задаче
  \return Число критериев
  */
  virtual int GetNumberOfCriterions() const;

  /** Метод, вычисляющий функции задачи

  \param[in] y непрерывные координаты точки, в которой необходимо вычислить значение
  \param[in] u целочисленые координаты точки, в которой необходимо вычислить значение
  \param[in] fNumber Номер вычисляемой функции. 0 соответствует первому ограничению,
  #GetNumberOfFunctions() - 1 -- последнему критерию
  \return Значение функции с указанным номером
  */
  virtual double CalculateFunctionals(const std::vector<double>& x, std::vector<std::string>& u, int fNumber);

  /** Метод возвращает точку из допустимой области
  \param[out] y непрерывные координаты точки
  \param[out] u целочисленые координаты точки
  \param[out] values значение в этой точке
  \return Код ошибки (#PROBLEM_OK или #UNDEFINED)
  */
  virtual int GetStartTrial(std::vector<double>& y, std::vector<std::string>& u, std::vector<double>& values);

  ~RastriginC1Problem();
};

extern "C" LIB_EXPORT_API IGlobalOptimizationProblem* create();
extern "C" LIB_EXPORT_API void destroy(IGlobalOptimizationProblem* ptr);
#endif
// - end of file ----------------------------------------------------------------------------------
