/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//             LOBACHEVSKY STATE UNIVERSITY OF NIZHNY NOVGOROD             //
//                                                                         //
//                       Copyright (c) 2016 by UNN.                        //
//                          All Rights Reserved.                           //
//                                                                         //
//  File:      problem_manager.h                                           //
//                                                                         //
//  Purpose:   Header file for dynamic libraries manager class             //
//                                                                         //
//                                                                         //
//  Author(s): Sovrasov V.                                                 //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

/**
\file problem_manager.h

\authors Соврасов В.
\date 2016
\copyright ННГУ им. Н.И. Лобачевского

\brief Объявление класса #ProblemManager

*/

#ifndef __GLOBAL_OPTIMIZATION_PROBLEM_MANAGER_H__
#define __GLOBAL_OPTIMIZATION_PROBLEM_MANAGER_H__

#include "IGlobalOptimizationProblem.h"
#include <string>

#ifdef WIN32
  #include <direct.h>
  #include <windows.h>
#else
  #include <sys/types.h>
  #include <dlfcn.h>
#endif

/**
Класс, реализующий загрузку и выгрузку подключамеых библиотек с задачами

В классе #ProblemManager реализованы основные функции, для загрузки\выгрузки библиотек с задачами
*/
class GlobalOptimizationProblemManager
{
protected:

  ///Указатель на дескриптор загруженной библиотеки
  #ifdef WIN32
    HINSTANCE mLibHandle;
  #else
    void *mLibHandle;
  #endif
  ///Указатель на созданный объект, описывающий задачу
  IGlobalOptimizationProblem* mProblem;
  ///Указатель на функцию-фабрику задач
  createProblem* mCreate;
  ///Указатель на функцию-деструктор задач
  destroyProblem* mDestroy;

  /// Метод, освобождающий загруженную библиотеку. Будет вызван в деструкторе
  int FreeProblemLibrary();

  ///Служебный метод, освобождающий #mLibHandle
  void FreeLibHandler();

public:

  /**
  Код ошибки, возвращаемый методами #LoadProblemLibrary и #FreeProblemLibrary
  при успешном выполнении операций
  */
  static const int OK_ = 0;
  /**
  Код ошибки, возвращаемый методами #LoadProblemLibrary и #FreeProblemLibrary
  при ошибке во время выполнении операций
  */
  static const int ERROR_ = -2;

  ///Конструктор
  GlobalOptimizationProblemManager();
  ///Деструктор, в нём вызывается #FreeProblemLibrary
  ~GlobalOptimizationProblemManager();

  /** Метод, загружающий библиотеку, находящуюся по указанному пути

  Метод загружает библиотеку, пытается импортировать из неё функции,
  создающие и уничтожающие задачу, а затем создаёт задачу

  \param[in] libPath Путь к загружаемой библиотеке
  \return Код ошибки
  */
  int LoadProblemLibrary(const std::string& libPath);

  /** Метод возвращает указатель #mProblem
  */
  IGlobalOptimizationProblem* GetProblem() const;
};

int InitGlobalOptimizationProblem(GlobalOptimizationProblemManager& problemManager, IGlobalOptimizationProblem*& problem, std::string libPath);

#endif
// - end of file ----------------------------------------------------------------------------------
