//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Initialisation of static member variables in 
///  utility classes.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///   
///  \author Pat Scott 
///          (patscott@physics.mcgill.ca)
///  \date 2014 Mar
///
///  *********************************************

#ifndef __static_members_hpp__
#define __static_members_hpp__

#include "exceptions.hpp"

namespace Gambit
{

  /// Map of pointers to all instances of the exception class.
  std::map<const char*,exception*> exception::exception_map;

}

#endif //#ifndef __static_members_hpp__

