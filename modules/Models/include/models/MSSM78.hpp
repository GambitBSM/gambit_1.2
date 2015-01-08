//  GAMBIT: Global and Modular BSM Inference Tool
//  *********************************************
//
//  MSSM input parameter definition
//   
//  This is a parameterisation of the MSSM for
//  use running spectrum generators (primarily
//  flexiblesusy) in a generic way.
//  Specialisations of this model are used to
//  define subsets of these parameters which are 
//  more reasonable for scanning.
//
//  *********************************************
//
//  Authors
//  =======
//
//  (add name and date if you modify)
//
//  Ben Farmer
//  2015 Jan
//
//  *********************************************

#ifndef __MSSM78_hpp__
#define __MSSM78_hpp__

/// FlexibleSUSY compatible general (78 parameters plus sign) MSSM parameterisation
#define MODEL MSSM78
  START_MODEL
  DEFINEPARS(TanBeta,SignMu,
             mHu2,mHd2,M1,M2,M3)
  
  DEFINEPARS(mq2_11, mq2_12, mq2_13,
             mq2_21, mq2_22, mq2_23,
             mq2_31, mq2_32, mq2_33)

  DEFINEPARS(ml2_11, ml2_12, ml2_13,
             ml2_21, ml2_22, ml2_23,
             ml2_31, ml2_32, ml2_33)

  DEFINEPARS(md2_11, md2_12, md2_13,
             md2_21, md2_22, md2_23,
             md2_31, md2_32, md2_33)

  DEFINEPARS(mu2_11, mu2_12, mu2_13,
             mu2_21, mu2_22, mu2_23,
             mu2_31, mu2_32, mu2_33)

  DEFINEPARS(me2_11, me2_12, me2_13,
             me2_21, me2_22, me2_23,
             me2_31, me2_32, me2_33)

  DEFINEPARS(Ae_11, Ae_12, Ae_13,
             Ae_21, Ae_22, Ae_23,
             Ae_31, Ae_32, Ae_33)
  
  DEFINEPARS(Ad_11, Ad_12, Ad_13,
             Ad_21, Ad_22, Ad_23,
             Ad_31, Ad_32, Ad_33)

  DEFINEPARS(Au_11, Au_12, Au_13,
             Au_21, Au_22, Au_23,
             Au_31, Au_32, Au_33)
#undef MODEL

#endif
