//  GAMBIT: Global and Modular BSM Inference Tool
//  *********************************************
///  \file
///
///  Generic observable and likelihood function 
///  macro definitions, for inclusion from actual
///  module source code.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///   
///  \author Pat Scott
///          (patscott@physics.mcgill.ca)
///  \date 2012 Nov  
///  \date 2013 All year 
///  \date 2014 Foreverrrrr
///
///  \author Abram Krislock
///          (abram.krislock@fysik.su.se)
///  \date 2013 Jan, Feb
//
///  \author Christoph Weniger
///          (c.weniger@uva.nl)
///  \date 2013 Jan, Feb, 2014 Jan
///
///  \author Anders Kvellestad
///          (anders.kvellestad@fys.uio.no)
///  \date 2013 Nov
///  *********************************************

#ifndef __module_macros_inmodule_hpp__
#define __module_macros_inmodule_hpp__

#include "functors.hpp"
#include "exceptions.hpp"
#include "util_macros.hpp"
#include "safety_bucket.hpp"
#include "module_macros_common.hpp"

/// \name Rollcall macros
/// These are called from within rollcall headers in each module to 
/// register module functions, their capabilities, return types, dependencies,
/// and backend requirements.
/// @{
// Redirect the rollcall macros to their in-module variants
#define START_MODULE                                      MODULE_START_MODULE
#define START_CAPABILITY                                  MODULE_START_CAPABILITY(MODULE)
#define LONG_START_CAPABILITY(MODULE, C)                  MODULE_START_CAPABILITY(MODULE)
#define DECLARE_FUNCTION(TYPE, FLAG)                      MODULE_DECLARE_FUNCTION(MODULE, FUNCTION, TYPE, FLAG)
#define LONG_DECLARE_FUNCTION(MODULE, C, FUNCTION, TYPE, FLAG) \
                                                          MODULE_DECLARE_FUNCTION(MODULE, FUNCTION, TYPE, FLAG)
#define DEPENDENCY(DEP, TYPE)                             MODULE_DEPENDENCY(DEP, TYPE)
#define NEEDS_MANAGER_WITH_CAPABILITY(LOOPMAN)            MODULE_NEEDS_MANAGER_WITH_CAPABILITY(LOOPMAN)                                  
#define ALLOWED_MODEL(MODULE,FUNCTION,MODEL)              MODULE_ALLOWED_MODEL(MODULE,FUNCTION,MODEL)
#define ALLOWED_MODEL_ONLY_VIA_GROUPS(MODULE,FUNCTION,MODEL) \
                                                          MODULE_ALLOWED_MODEL(MODULE,FUNCTION,MODEL) 
#define LITTLEGUY_ALLOW_MODEL(PARAMETER,MODEL)            LITTLEGUY_ALLOWED_MODEL(PARAMETER,MODEL)
#define ALLOW_MODEL_COMBINATION(...)                      DUMMYARG(__VA_ARGS__)
#define MODEL_GROUP(GROUPNAME, GROUP)                     DUMMYARG(GROUPNAME, GROUP)

#define BE_GROUP(GROUP)                                   MODULE_BE_GROUP(GROUP)
#define DECLARE_BACKEND_REQ(GROUP, REQUIREMENT, TAGS, TYPE, ARGS, IS_VARIABLE) \
                                                          MODULE_BACKEND_REQ(GROUP, REQUIREMENT, TAGS, TYPE, ARGS, IS_VARIABLE) 
#define ACTIVATE_BACKEND_REQ_FOR_MODELS(MODELS,TAGS)      DUMMYARG(MODELS,TAGS)                   
#define START_CONDITIONAL_DEPENDENCY(TYPE)                MODULE_START_CONDITIONAL_DEPENDENCY(TYPE)
#define ACTIVATE_DEP_BE(BACKEND_REQ, BACKEND, VERSTRING)  DUMMYARG(BACKEND_REQ, BACKEND, VERSTRING)
#define ACTIVATE_FOR_MODELS(...)                          DUMMYARG(__VA_ARGS__)
#define BACKEND_OPTION(BACKEND_AND_VERSIONS,TAGS)         DUMMYARG(BACKEND_AND_VERSIONS,TAGS)
#define FORCE_SAME_BACKEND(...)                           DUMMYARG(__VA_ARGS__)                               
/// @}


//  *******************************************************************************
/// \name In-module rollcall macros
/// @{

/// Redirection of \link START_MODULE() START_MODULE\endlink when 
/// invoked from within a module.
#define MODULE_START_MODULE                                                    \
                                                                               \
  IF_TOKEN_UNDEFINED(MODULE,FAIL("You must define MODULE before calling "      \
   "START_MODULE."))                                                           \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
    namespace MODULE                                                           \
    {                                                                          \
      /* Module errors */                                                      \
      error& CAT(MODULE,_error)();                                             \
      /* Module warnings */                                                    \
      warning& CAT(MODULE,_warning)();                                         \
    }                                                                          \
  }                                                                            \


/// Redirection of \link START_CAPABILITY() START_CAPABILITY\endlink when 
/// invoked from within a module.
#define MODULE_START_CAPABILITY(MODULE)                                        \
  IF_TOKEN_UNDEFINED(MODULE,FAIL("You must define MODULE before calling "      \
   "START_CAPABILITY."))                                                       \

/// Redirection of \link START_FUNCTION() START_FUNCTION\endlink when invoked 
/// from within a module.
#define MODULE_DECLARE_FUNCTION(MODULE, FUNCTION, TYPE, FLAG)                  \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
                                                                               \
    namespace MODULE                                                           \
    {                                                                          \
                                                                               \
      /* Let the module source know that this functor is declared*/            \
      namespace Functown { extern module_functor<TYPE> FUNCTION; }             \
                                                                               \
      /* Set up a helper function to call the iterate method if the functor is \
      able to manage loops. */                                                 \
      namespace Functown                                                       \
      {                                                                        \
        BOOST_PP_IIF(BOOST_PP_EQUAL(FLAG, 1),                                  \
          void CAT(FUNCTION,_iterate)(int it)                                  \
          {                                                                    \
            FUNCTION.iterate(it);                                              \
          }                                                                    \
        ,)                                                                     \
      }                                                                        \
                                                                               \
      namespace Pipes                                                          \
      {                                                                        \
        namespace FUNCTION                                                     \
        {                                                                      \
          /* Declare the parameters safe-pointer map as external. */           \
          extern std::map<str, safe_ptr<double> > Param;                       \
          /* Declare the safe-pointer to the models vector as external. */     \
          extern safe_ptr< std::vector<str> > Models;                          \
          /* Declare the safe pointer to the run options as external. */       \
          extern safe_ptr<Options> runOptions;                                 \
          /* Create a pointer to the single iteration of the loop that can be  \
          executed by this functor */                                          \
          namespace Loop                                                       \
          {                                                                    \
            BOOST_PP_IIF(BOOST_PP_EQUAL(FLAG, 1),                              \
              void (*executeIteration)(int) =                                  \
               &Functown::CAT(FUNCTION,_iterate);                              \
            ,)                                                                 \
          }                                                                    \
        }                                                                      \
      }                                                                        \
                                                                               \
    }                                                                          \
                                                                               \
  }                                                                            \


/// Redirection of NEEDS_MANAGER_WITH_CAPABILITY(LOOPMAN) when invoked from 
/// within a module.
#define MODULE_NEEDS_MANAGER_WITH_CAPABILITY(LOOPMAN)                          \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
    namespace MODULE                                                           \
    {                                                                          \
      namespace Pipes                                                          \
      {                                                                        \
        namespace FUNCTION                                                     \
        {                                                                      \
          namespace Loop                                                       \
          {                                                                    \
            /* Declare the safe pointer to the iteration number of the loop    \
            this functor is running within, as external. */                    \
            extern omp_safe_ptr<int> iteration;                                \
          }                                                                    \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  }                                                                            \
                                                                               \

/// Redirection of DEPENDENCY(DEP, TYPE) when invoked from within a module.
#define MODULE_DEPENDENCY(DEP, TYPE)                                           \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
                                                                               \
    namespace MODULE                                                           \
    {                                                                          \
                                                                               \
      /* Given that TYPE is not void, create a safety_bucket for the           \
      dependency result. To be initialized automatically at runtime            \
      when the dependency is resolved. */                                      \
      namespace Pipes                                                          \
      {                                                                        \
        namespace FUNCTION                                                     \
        {                                                                      \
          BOOST_PP_IIF(IS_TYPE(void,TYPE),,                                    \
            namespace Dep { extern dep_bucket<TYPE> DEP; } )                   \
        }                                                                      \
                                                                               \
      }                                                                        \
                                                                               \
    }                                                                          \
                                                                               \
  }                                                                            \


/// Redirection of ALLOW_MODEL when invoked from within a module.
#define MODULE_ALLOWED_MODEL(MODULE,FUNCTION,MODEL)                            \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
                                                                               \
    namespace MODULE                                                           \
    {                                                                          \
                                                                               \
      /* Create a safe pointer to the model parameters result. To be filled    \
      automatically at runtime when the dependency is resolved. */             \
      namespace Pipes                                                          \
      {                                                                        \
        namespace FUNCTION                                                     \
        {                                                                      \
          namespace Dep {extern dep_bucket<ModelParameters>                    \
           CAT(MODEL,_parameters); }                                           \
        }                                                                      \
      }                                                                        \
                                                                               \
    }                                                                          \
                                                                               \
  }                                                                            \

//"Littleguys" version of allowed_model
#define LITTLEGUY_ALLOWED_MODEL(FUNCTION,MODEL)                                \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
   namespace Models                                                            \
   {                                                                           \
    namespace MODEL                                                            \
    {                                                                          \
                                                                               \
      /* Create a safe pointer to the model parameters result. To be filled    \
      automatically at runtime when the dependency is resolved. */             \
      namespace Pipes                                                          \
      {                                                                        \
        namespace FUNCTION                                                     \
        {                                                                      \
          namespace Dep {extern dep_bucket<ModelParameters>                    \
           CAT(MODEL,_parameters); }                                           \
        }                                                                      \
      }                                                                        \
                                                                               \
    }                                                                          \
   }                                                                           \
  }                                                                            \


/// Redirection of BACKEND_GROUP(GROUP) when invoked from within a module.
#define MODULE_BE_GROUP(GROUP)                                                 \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
    namespace MODULE                                                           \
    {                                                                          \
      namespace Pipes                                                          \
      {                                                                        \
        namespace FUNCTION                                                     \
        {                                                                      \
          namespace BEgroup                                                    \
          {                                                                    \
            /* Declare a safe pointer to the functor's internal register of    \
            which backend requirement is activated from this group. */         \
            extern safe_ptr<str> GROUP;                                        \
          }                                                                    \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  }                                                                            \


/// Redirection of BACKEND_REQ(GROUP, REQUIREMENT, (TAGS), TYPE, [(ARGS)]) 
/// for declaring backend requirements when invoked from within a module.
#define MODULE_BACKEND_REQ(GROUP, REQ, TAGS, TYPE, ARGS, IS_VARIABLE)          \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
    namespace MODULE                                                           \
    {                                                                          \
      namespace Pipes                                                          \
      {                                                                        \
        namespace FUNCTION                                                     \
        {                                                                      \
          namespace BEreq                                                      \
          {                                                                    \
            /* Create a safety_bucket for the backend variable/function.       \
            To be initialized by the dependency resolver at runtime. */        \
            typedef BEvariable_bucket<TYPE> CAT(REQ,var);                      \
            typedef BEfunction_bucket<TYPE INSERT_NONEMPTY(ARGS)>              \
             CAT(REQ,func);                                                    \
            extern CAT(REQ,BOOST_PP_IIF(IS_VARIABLE,var,func)) REQ;            \
          }                                                                    \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  }                                                                            \


/// Redirection of START_CONDITIONAL_DEPENDENCY(TYPE) when invoked from within 
/// a module.
#define MODULE_START_CONDITIONAL_DEPENDENCY(TYPE)                              \
  MODULE_DEPENDENCY(CONDITIONAL_DEPENDENCY, TYPE)                              \

/// @}

#endif // defined __module_macros_inmodule_hpp__ 

