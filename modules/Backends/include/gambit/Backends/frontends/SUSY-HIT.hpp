//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Frontend for SUSY-HIT 1.4 backend
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///  
/// \author Peter Athron
/// \author Csaba Balazs
/// \author Pat Scott
/// \date 2015 Jan-Apr
///
///  *********************************************

#ifdef BACKENDRENAME
  #define BACKENDNAME BACKENDRENAME
#else
  #define BACKENDNAME SUSY_HIT
#endif
#define VERSION 1.4
#define SAFE_VERSION 1_4

// Let's go.
LOAD_LIBRARY

// Can't do anything non-MSSM with SUSY-HIT
BE_ALLOW_MODELS(MSSM78atQ, MSSM78atMGUT)

// Functions
BE_FUNCTION(sdecay, void, (const int&), "sdecay_", "sdecay")  // Converted SUSY-HIT main routine

// Variables
BE_VARIABLE(susyhitin_type, susyhitin, "susyhitin_", "cb_susyhitin")
BE_VARIABLE(widtha_hdec_type, widtha_hdec, "widtha_hdec_", "cb_widtha_hdec")
BE_VARIABLE(widthhl_hdec_type, widthhl_hdec, "widthhl_hdec_", "cb_widthhl_hdec")
BE_VARIABLE(widthhh_hdec_type, widthhh_hdec, "widthhh_hdec_", "cb_widthhh_hdec")
BE_VARIABLE(widthhc_hdec_type, widthhc_hdec, "widthhc_hdec_", "cb_widthhc_hdec")
BE_VARIABLE(wisusy_hdec_type, wisusy_hdec, "wisusy_hdec_", "cb_wisusy_hdec")
BE_VARIABLE(wisfer_hdec_type, wisfer_hdec, "wisfer_hdec_", "cb_wisfer_hdec")
BE_VARIABLE(hd_golddec_type, hd_golddec, "hd_golddec_", "cb_hd_golddec")
BE_VARIABLE(sd_char2body_type, sd_char2body, "sd_char2body_", "cb_sd_char2body")
BE_VARIABLE(sd_char2bodygrav_type, sd_char2bodygrav, "sd_char2bodygrav_", "cb_sd_char2bodygrav")
BE_VARIABLE(sd_char3body_type, sd_char3body, "sd_char3body_", "cb_sd_char3body")
BE_VARIABLE(sd_charwidth_type, sd_charwidth, "sd_charwidth_", "cb_sd_charwidth")
BE_VARIABLE(sd_neut2body_type, sd_neut2body, "sd_neut2body_", "cb_sd_neut2body")
BE_VARIABLE(sd_neut2bodygrav_type, sd_neut2bodygrav, "sd_neut2bodygrav_", "cb_sd_neut2bodygrav")
BE_VARIABLE(sd_neut3body_type, sd_neut3body, "sd_neut3body_", "cb_sd_neut3body")
BE_VARIABLE(sd_neutloop_type, sd_neutloop, "sd_neutloop_", "cb_sd_neutloop")
BE_VARIABLE(sd_neutwidth_type, sd_neutwidth, "sd_neutwidth_", "cb_sd_neutwidth")
BE_VARIABLE(sd_glui2body_type, sd_glui2body, "sd_glui2body_", "cb_sd_glui2body")
BE_VARIABLE(sd_glui3body_type, sd_glui3body, "sd_glui3body_", "cb_sd_glui3body")
BE_VARIABLE(sd_gluiloop_type, sd_gluiloop, "sd_gluiloop_", "cb_sd_gluiloop")
BE_VARIABLE(sd_gluiwidth_type, sd_gluiwidth, "sd_gluiwidth_", "cb_sd_gluiwidth")
BE_VARIABLE(sd_sup2body_type, sd_sup2body, "sd_sup2body_", "cb_sd_sup2body")
BE_VARIABLE(sd_supwidth_type, sd_supwidth, "sd_supwidth_", "cb_sd_supwidth")
BE_VARIABLE(sd_sdown2body_type, sd_sdown2body, "sd_sdown2body_", "cb_sd_sdown2body")
BE_VARIABLE(sd_sdownwidth_type, sd_sdownwidth, "sd_sdownwidth_", "cb_sd_sdownwidth")
BE_VARIABLE(sd_stop2body_type, sd_stop2body, "sd_stop2body_", "cb_sd_stop2body")
BE_VARIABLE(sd_stop3body_type, sd_stop3body, "sd_stop3body_", "cb_sd_stop3body")
BE_VARIABLE(sd_stoploop_type, sd_stoploop, "sd_stoploop_", "cb_sd_stoploop")
BE_VARIABLE(sd_stop4body_type, sd_stop4body, "sd_stop4body_", "cb_sd_stop4body")
BE_VARIABLE(sd_stopwidth_type, sd_stopwidth, "sd_stopwidth_", "cb_sd_stopwidth")
BE_VARIABLE(sd_sbot2body_type, sd_sbot2body, "sd_sbot2body_", "cb_sd_sbot2body")
BE_VARIABLE(sd_sbot3body_type, sd_sbot3body, "sd_sbot3body_", "cb_sd_sbot3body")
BE_VARIABLE(sd_sbotwidth_type, sd_sbotwidth, "sd_sbotwidth_", "cb_sd_sbotwidth")
BE_VARIABLE(sd_sel2body_type, sd_sel2body, "sd_sel2body_", "cb_sd_sel2body")
BE_VARIABLE(sd_selwidth_type, sd_selwidth, "sd_selwidth_", "cb_sd_selwidth")
BE_VARIABLE(sd_snel2body_type, sd_snel2body, "sd_snel2body_", "cb_sd_snel2body")
BE_VARIABLE(sd_snelwidth_type, sd_snelwidth, "sd_snelwidth_", "cb_sd_snelwidth")
BE_VARIABLE(sd_stau2body_type, sd_stau2body, "sd_stau2body_", "cb_sd_stau2body")
BE_VARIABLE(sd_stau2bodygrav_type, sd_stau2bodygrav, "sd_stau2bodygrav_", "cb_sd_stau2bodygrav")
BE_VARIABLE(sd_stauwidth_type, sd_stauwidth, "sd_stauwidth_", "cb_sd_stauwidth")
BE_VARIABLE(sd_sntau2body_type, sd_sntau2body, "sd_sntau2body_", "cb_sd_sntau2body")
BE_VARIABLE(sd_sntauwidth_type, sd_sntauwidth, "sd_sntauwidth_", "cb_sd_sntauwidth")
BE_VARIABLE(sd_top2body_type, sd_top2body, "sd_top2body_", "cb_sd_top2body")
BE_VARIABLE(sd_topwidth_type, sd_topwidth, "sd_topwidth_", "cb_sd_topwidth")

// Initialisation function (dependencies)
BE_INI_DEPENDENCY(MSSM_spectrum, const SMplusUV*)

// Initialisation function (definition)
BE_INI_FUNCTION
{
  // Scan-level initialisation
  static bool scan_level = true;
  if (scan_level)
  {
    susyhitin->amsin = 0.19;            // MSBAR(1)
    susyhitin->amcin = 1.4;             // MC
    susyhitin->ammuonin = 0.105658389;  // MMUON
    susyhitin->alphin = 137.0359895;    // 1/ALPHA
    susyhitin->gamwin = 2.08;           // GAMW
    susyhitin->gamzin = 2.49;           // GAMZ
    susyhitin->vusin = 0.2205;          // VUS
    susyhitin->vcbin = 0.04;            // VCB
    susyhitin->rvubin = 0.08;           // VUB/VCB    
    sdecay(0); // Skip FV light stop decays
  }
  scan_level = false;
}
DONE

// Undefine macros to avoid conflict with other backends
#include "gambit/Backends/backend_undefs.hpp"

