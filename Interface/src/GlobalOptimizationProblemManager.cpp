/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//             LOBACHEVSKY STATE UNIVERSITY OF NIZHNY NOVGOROD             //
//                                                                         //
//                       Copyright (c) 2016 by UNN.                        //
//                          All Rights Reserved.                           //
//                                                                         //
//  File:      problem_manager.cpp                                         //
//                                                                         //
//  Purpose:   Source file for dynamic libraries manager class             //
//                                                                         //
//                                                                         //
//  Author(s): Sovrasov V.                                                 //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#include "GlobalOptimizationProblemManager.h"
#include <iostream>

// ------------------------------------------------------------------------------------------------
GlobalOptimizationProblemManager::GlobalOptimizationProblemManager() : mLibHandle(NULL), mProblem(NULL),
  mCreate(NULL), mDestroy(NULL)
{

}

// ------------------------------------------------------------------------------------------------
GlobalOptimizationProblemManager::~GlobalOptimizationProblemManager()
{
  FreeProblemLibrary();
}

// ------------------------------------------------------------------------------------------------
int GlobalOptimizationProblemManager::LoadProblemLibrary(const std::string& libPath)
{
  if (mLibHandle)
    FreeProblemLibrary();
  #ifdef WIN32
    mLibHandle = LoadLibrary(TEXT(libPath.c_str()));
    if (!mLibHandle)
    {
        std::cerr << "Cannot load library: " << TEXT(libPath.c_str()) << std::endl;
        return GlobalOptimizationProblemManager::ERROR_;
    }
  #else
    mLibHandle = dlopen(libPath.c_str(), RTLD_LAZY);
    if (!mLibHandle)
    {
        std::cerr << dlerror() << std::endl;
        return GlobalOptimizationProblemManager::ERROR_;
    }
  #endif
  #ifdef WIN32
    mCreate = (createProblem*) GetProcAddress(mLibHandle, "create");
    mDestroy = (destroyProblem*) GetProcAddress(mLibHandle, "destroy");
    if (!mCreate || !mDestroy)
    {
      std::cerr << "Cannot load symbols: " << GetLastError() << std::endl;
      FreeLibHandler();
      return GlobalOptimizationProblemManager::ERROR_;
    }
  #else
    dlerror();
    mCreate = (createProblem*) dlsym(mLibHandle, "create");
    char* dlsym_error = dlerror();
    if (dlsym_error)
    {
      mCreate = NULL;
      std::cerr << dlsym_error << std::endl;
      FreeLibHandler();
      return GlobalOptimizationProblemManager::ERROR_;
    }
    mDestroy = (destroyProblem*) dlsym(mLibHandle, "destroy");
    dlsym_error = dlerror();
    if (dlsym_error)
    {
      mCreate = NULL;
      mDestroy = NULL;
      std::cerr << dlsym_error << std::endl;
      FreeLibHandler();
      return GlobalOptimizationProblemManager::ERROR_;
    }
  #endif

  mProblem = mCreate();
  if (!mProblem)
  {
    FreeLibHandler();
    mCreate = NULL;
    mDestroy = NULL;
    std::cerr << "Cannot create problem instance" << std::endl;
  }

  return GlobalOptimizationProblemManager::OK_;
}

// ------------------------------------------------------------------------------------------------
void GlobalOptimizationProblemManager::FreeLibHandler()
{
  #ifdef WIN32
    FreeLibrary(mLibHandle);
  #else
    dlclose(mLibHandle);
  #endif
  mLibHandle = NULL;
}

// ------------------------------------------------------------------------------------------------
int GlobalOptimizationProblemManager::FreeProblemLibrary()
{
  if (mProblem)
    mDestroy(mProblem);
  if (mLibHandle)
    FreeLibHandler();
  mLibHandle = NULL;
  mProblem = NULL;
  mCreate = NULL;
  mDestroy = NULL;
  return GlobalOptimizationProblemManager::OK_;
}

// ------------------------------------------------------------------------------------------------
IGlobalOptimizationProblem* GlobalOptimizationProblemManager::GetProblem() const
{
  if (mProblem)
    return mProblem;
  else
    return NULL;
}

// ------------------------------------------------------------------------------------------------
int InitGlobalOptimizationProblem(GlobalOptimizationProblemManager& problemManager, IGlobalOptimizationProblem*& problem, std::string libPath)
{
  if (problemManager.LoadProblemLibrary(libPath) != GlobalOptimizationProblemManager::OK_)
  {
    //сообщение об ошибке печатает manager
    return 1;
  }

  IGlobalOptimizationProblem* baseProblem = problemManager.GetProblem();
  baseProblem->Initialize();

  problem = baseProblem; 
  return 0;
}
// - end of file ----------------------------------------------------------------------------------
