#ifndef __RASTRIGINPROBLEM_H__
#define __RASTRIGINPROBLEM_H__

#include "IGlobalOptimizationProblem.h"

/// Задача Растригина с частично целочисленными параметрами
class RastriginIntProblem : public IGlobalOptimizationProblem
{


protected:
  /// Число непрерывных параметров
  int countContinuousVariables;

  /// Нижняя граница области поиска
  std::vector<double> A;
  /// Верхняя граница области поиска
  std::vector<double> B;
  /// Координаты точки оптимума непрерывной части задачи
  std::vector<double> optPoint;
  /// Приведенные к int координаты дискретных параметров
  std::vector<double> discreteValues;
  /// коэфициент для сдвига функции при добовление целочисленной части функции
  double multKoef;
  /// коэфициент сдвига значения целевой функции
  double optMultKoef;

  /// Размерность задачи
  int mDimension;
  /// Инициализирована ли задача
  bool mIsInitialized;
  /// Максимальная допустимая размерность задачи
  int mMaxDimension;
  /// минимальная  допустимая размерность задачи
  int mMinDimension;
  /// число критериев
  int mNumberOfCriterions;
  /// число ограничений
  int mNumberOfConstraints;
  /// Максимальная нижняя граница области поиска
  double mLeftBorder;
  /// Минимальная верхняя граница области поиска
  double mRightBorder;
  /// Минимальное число значений дискретных параметров по каждой размерности
  int mDefNumberOfValues;
  /// используется ли сдвиг
  bool IsMultInt;
  /// Число значений дискретных параметров по каждой размерности
  std::vector<int> mNumberOfValues;

  /// Очищает номер текущего значения для дискретного параметра
  virtual void ClearCurrentDiscreteValueIndex(int** mCurrentDiscreteValueIndex)
  {
    if (GetNumberOfDiscreteVariable() > 0)
    {
      if (*mCurrentDiscreteValueIndex != 0)
        delete[] * mCurrentDiscreteValueIndex;

      *mCurrentDiscreteValueIndex = new int[GetNumberOfDiscreteVariable()];
      for (int i = 0; i < GetNumberOfDiscreteVariable(); i++)
        (*mCurrentDiscreteValueIndex)[i] = 0;
    }
  }

public:

  RastriginIntProblem();
  /** Метод возвращает значение целевой функции в точке глобального минимума
  \param[out] value оптимальное значение
  \return Код ошибки (#PROBLEM_OK или #UNDEFINED)
  */
  virtual int GetOptimumValue(double& value) const;
  /** Метод возвращает координаты точки глобального минимума целевой функции
  \param[out] y точка, в которой достигается оптимальное значение
  \return Код ошибки (#PROBLEM_OK или #UNDEFINED)
  */
  virtual int GetOptimumPoint(std::vector<double>& x, std::vector<std::string>& u) const;
  /** Метод, вычисляющий функции задачи

  \param[in] y непрерывные координаты точки, в которой необходимо вычислить значение
  \param[in] u целочисленые координаты точки, в которой необходимо вычислить значение
  \param[in] fNumber Номер вычисляемой функции. 0 соответствует первому ограничению,
  #GetNumberOfFunctions() - 1 -- последнему критерию
  \return Значение функции с указанным номером
  */
  virtual double CalculateFunctionals(const std::vector<double>& x, std::vector<std::string>& u, int fNumber);

  /** Метод задаёт размерность задачи

  Данный метод должен вызываться перед #Initialize. Размерность должна быть в
  списке поддерживаемых.
  \param[in] dimension размерность задачи
  \return Код ошибки
  */
  virtual int SetDimension(int dimension);
  /// Возвращает размерность задачи, можно вызывать после #Initialize
  virtual int GetDimension() const;

  /** Метод возвращает границы области поиска
  */
  virtual void GetBounds(std::vector<double>& lower, std::vector<double>& upper);

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

  ~RastriginIntProblem();


  /// <summary>
  /// Вычисление множителя
  /// </summary>
  /// <param name="x"></param>
  /// <returns></returns>
  double MultFunc(const std::vector<double>& x);

  ///Инициализация задачи
  virtual int Initialize();

  /// Метод возвращает число дискретных параметров, дискретные параметры всегда последние в векторе y
  virtual int GetNumberOfDiscreteVariable() const;

  /** Метод задает число дискретных параметров, дискретные параметры всегда последние в векторе y,
  только для задач с частично целочисленными параметрами
  \param[in] numberOfDiscreteVariable число дискретных параметров
  \return Код ошибки
  */
  virtual int SetNumberOfDiscreteVariable(int numberOfDiscreteVariable);


  /** Метод возвращает число целочисленных переменных
  \return Число целочисленных переменных
  */
  virtual int GetDiscreteVariableValues(std::vector< std::vector<std::string>>& values) const;

};

extern "C" LIB_EXPORT_API IGlobalOptimizationProblem* create();
extern "C" LIB_EXPORT_API void destroy(IGlobalOptimizationProblem* ptr);

#endif
// - end of file ----------------------------------------------------------------------------------
