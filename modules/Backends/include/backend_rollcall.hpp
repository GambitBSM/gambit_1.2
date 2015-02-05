//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file                                       
///                                               
///  Compile-time registration of available back- 
///  ends.                                        
///                                               
///  This file was automatically generated by     
///  backend_harvester.py. Do not modify.         
///                                               
///  Do not add to this if you want to add a new  
///  backend -- just add your frontend header to  
///  Backends/include/frontends and rest assured  
///  that backend_harvester.py will make sure it  
///  ends up here.                                
///                                               
///  *********************************************
///                                               
///  Authors (add name and date if you modify):   
///                                               
///  \author The GAMBIT Collaboration            
///  \date 02:19AM on February 05, 2015
///                                               
///  *********************************************
                                                  
#ifndef __backend_rollcall_hpp__                  
#define __backend_rollcall_hpp__                  
                                                  
// Include the backend macro definitions          
#include "backend_macros.hpp"                   
                                                  
// Automatically-generated list of frontends.     
#include "frontends/BOSSMinimalExample_1_2.hpp"
#include "frontends/BOSSMinimalExample_1_0.hpp"
#include "frontends/HiggsBounds.hpp"
#include "frontends/Pythia_8_186.hpp"
#include "frontends/fakeSoftSUSY.hpp"
#include "frontends/HiggsSignals.hpp"
#include "frontends/FastSim.hpp"
#include "frontends/libFarrayTest.hpp"
#include "frontends/libfortran.hpp"
#include "frontends/BOSSMinimalExample_1_1.hpp"
#include "frontends/micromegas.hpp"
#include "frontends/nulike.hpp"
#include "frontends/SuperIso.hpp"
#include "frontends/DarkSUSY.hpp"
#include "frontends/DDCalc0.hpp"
#include "frontends/gamLike.hpp"
#include "frontends/SUSYHIT.hpp"
#include "frontends/FeynHiggs.hpp"
#include "frontends/libfirst_1_0.hpp"
#include "frontends/libfirst_1_1.hpp"

#endif // defined __backend_rollcall_hpp__
