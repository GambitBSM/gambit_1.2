//  GAMBIT: Global and Modular BSM Inference Tool
//  *********************************************
///  \file
///
///  ScannerBit executable-level header file.
///  Include this in your main program that calls
///  ScannerBit.
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

#ifndef __ScannerBit_hpp__
#define __ScannerBit_hpp__

#include "exception_map.hpp"
#include "scanner_utils.hpp"
#include "scan.hpp"

namespace Gambit
{

  namespace Scanner
  {

    namespace Ini
    {

      void register_ScannerBit_handlers()
      {
        error e = scan_error();
        warning w = scan_warning();
      }

      ini_code ScannerBit_handlers(&register_ScannerBit_handlers);

    }

  }

}

#endif //#ifndef __ScannerBit_hpp__
