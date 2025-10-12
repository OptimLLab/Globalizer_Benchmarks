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

/// Интерфейс для подключения задач на языке python
class TPythonModuleWrapper
{
protected:
  /// Размерность задачи
  int mDimension;
  /// Задача из python
  PyObject* mPFunc;
  /// Верхняя граница области поиска
  std::vector<double> mUpperBound;
  /// Нижняя граница области поиска
  std::vector<double> mLowerBound;

public:
  /// <summary>
  /// 
  /// </summary>
  /// <param name="module_path">путь до папки в которой лежат скрипты</param>
  TPythonModuleWrapper(const std::string& module_path);
  /// Возвращает размерность задачи, можно вызывать после #Initialize
  int GetDimension() const;
  /** Метод возвращает границы области поиска
  */
  void GetBounds(std::vector<double>& lower, std::vector<double>& upper) const;
  /** Метод, вычисляющий функции задачи

  \param[in] y непрерывные координаты точки, в которой необходимо вычислить значение
  \return Значение функции с указанным номером
  */
  double EvaluateFunction(const std::vector<double>& y) const;

  ~TPythonModuleWrapper();
};
