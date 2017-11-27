#include <vector>
#include <cmath>
#include <memory>
#include <iomanip>

#include "gambit/ColliderBit/analyses/BaseAnalysis.hpp"
#include "gambit/ColliderBit/ATLASEfficiencies.hpp"
#include "gambit/ColliderBit/mt2_bisect.h"

#include "RestFrames/RestFrames.hh"
#include "TLorentzVector.h"

using namespace std;

/* The ATLAS 13 TeV 3 lepton low mass recursive jigsaw search

   Based on code kindly supplied by Abhishek Sharma

   Data numbers are made up for now to do some basic sanity checks.

   Note that use of ROOT is compulsory for the RestFrames package

   Based on: arXiv link N/A at present
  
   Code adapted by Martin White

   KNOWN ISSUES

   1) Need to check overlap removal step when the paper comes out. For now, have assumed it is the same as the stop analysis.

*/

namespace Gambit {
  namespace ColliderBit {

    bool sortByPT_RJ3L(HEPUtils::Jet* jet1, HEPUtils::Jet* jet2) { return (jet1->pT() > jet2->pT()); }
    //bool sortByMass(HEPUtils::Jet* jet1, HEPUtils::Jet* jet2) { return (jet1->mass() > jet2->mass()); }

    bool SortLeptons(const pair<TLorentzVector,int> lv1, const pair<TLorentzVector,int> lv2)
    //bool VariableConstruction::SortLeptons(const lep lv1, const lep lv2) 
    {
      return lv1.first.Pt() > lv2.first.Pt();
    }

    bool SortJets(const TLorentzVector jv1, const TLorentzVector jv2)
    {
      return jv1.Pt() > jv2.Pt();
    }
    
    // Class to randomly select m elements from an n-d vector
    template<class BidiIter >
    BidiIter random_unique(BidiIter begin, BidiIter end, size_t num_random) {
      size_t left = std::distance(begin, end);
      while (num_random--) {
        BidiIter r = begin;
        std::advance(r, rand()%left);
        std::swap(*begin, *r);
        ++begin;
        --left;
      }
      return begin;
    }
    
    
    class Analysis_ATLAS_13TeV_RJ3L_lowmass_36invfb : public HEPUtilsAnalysis {
    private:

      // Numbers passing cuts
      int _num2L2JHIGH;
      int _num2L2JINT;
      int _num2L2JLOW;
      int _num2L2JCOMP;
      int _num3LHIGH;
      int _num3LINT;
      int _num3LLOW;
      int _num3LCOMP;

      vector<int> cutFlowVector;
      vector<string> cutFlowVector_str;
      int NCUTS; //=16;

      // Recursive jigsaw objects (using RestFrames)

      ///////////////////////////////////////////////////////////////////////
      /// 1. Create RJigsaw for C1N2 -> WZN1N1 -> 2L+2J+MET High mass region
      ///////////////////////////////////////////////////////////////////////
      
      RestFrames::LabRecoFrame*       LAB_2L2J;
      RestFrames::DecayRecoFrame*     C1N2_2L2J;
      RestFrames::DecayRecoFrame*     C1a_2L2J;
      RestFrames::DecayRecoFrame*     N2b_2L2J;
      
      RestFrames::DecayRecoFrame*     Wa_2L2J;
      RestFrames::DecayRecoFrame*     Zb_2L2J;
      
      RestFrames::VisibleRecoFrame*   J1_2L2J;
      RestFrames::VisibleRecoFrame*   J2_2L2J;
      RestFrames::VisibleRecoFrame*   L1_2L2J;
      RestFrames::VisibleRecoFrame*   L2_2L2J;
      
      RestFrames::InvisibleRecoFrame* X1a_2L2J;
      RestFrames::InvisibleRecoFrame* X1b_2L2J;
      
      RestFrames::InvisibleGroup*    INV_2L2J;
      
      RestFrames::SetMassInvJigsaw*     X1_mass_2L2J;
      RestFrames::SetRapidityInvJigsaw* X1_eta_2L2J;
      
      RestFrames::ContraBoostInvJigsaw* X1X1_contra_2L2J;

       ///////////////////////////////////////////////////////////////////////
      /// 2. Create RJigsaw for C1N2 -> WZN1N1 -> 3L + MET High mass region
      ///////////////////////////////////////////////////////////////////////
      
      RestFrames::LabRecoFrame*       LAB_3L;
      RestFrames::DecayRecoFrame*     C1N2_3L;
      RestFrames::DecayRecoFrame*     C1a_3L;
      RestFrames::DecayRecoFrame*     N2b_3L;
      
      RestFrames::DecayRecoFrame*     Wa_3L;
      RestFrames::DecayRecoFrame*     Zb_3L;
      
      RestFrames::VisibleRecoFrame*   L1a_3L;
      RestFrames::VisibleRecoFrame*   L1b_3L;
      RestFrames::VisibleRecoFrame*   L2b_3L;
      
      RestFrames::InvisibleRecoFrame* X1a_3L;
      RestFrames::InvisibleRecoFrame* X1b_3L;
      
      RestFrames::InvisibleGroup*    INV_3L;
      
      RestFrames::SetMassInvJigsaw*     X1_mass_3L;
      RestFrames::SetRapidityInvJigsaw* X1_eta_3L;
      
      RestFrames::ContraBoostInvJigsaw* X1X1_contra_3L;
      
      // combinatoric (transverse) tree
      // for jet assignment
      RestFrames::LabRecoFrame*        LAB_comb;
      RestFrames::DecayRecoFrame*      CM_comb;
      RestFrames::DecayRecoFrame*      S_comb;
      RestFrames::VisibleRecoFrame*    ISR_comb;
      RestFrames::VisibleRecoFrame*    J_comb;
      RestFrames::VisibleRecoFrame*    L_comb;
      RestFrames::InvisibleRecoFrame*  I_comb;
      RestFrames::InvisibleGroup*      INV_comb;
      RestFrames::SetMassInvJigsaw*    InvMass_comb;
      RestFrames::CombinatoricGroup*   JETS_comb;
      RestFrames::MinMassesCombJigsaw* SplitJETS_comb;
      
      // 2L+NJ tree (Z->ll + W/Z->qq)
      RestFrames::LabRecoFrame*        LAB_2LNJ;
      RestFrames::DecayRecoFrame*      CM_2LNJ;
      RestFrames::DecayRecoFrame*      S_2LNJ;
      RestFrames::VisibleRecoFrame*    ISR_2LNJ;
      
      RestFrames::DecayRecoFrame*      Ca_2LNJ;
      RestFrames::DecayRecoFrame*      Z_2LNJ;
      RestFrames::VisibleRecoFrame*    L1_2LNJ;
      RestFrames::VisibleRecoFrame*    L2_2LNJ;
      
      RestFrames::DecayRecoFrame*          Cb_2LNJ;
      RestFrames::SelfAssemblingRecoFrame* JSA_2LNJ;
      RestFrames::VisibleRecoFrame*        J_2LNJ;
      
      RestFrames::InvisibleRecoFrame*  Ia_2LNJ;
      RestFrames::InvisibleRecoFrame*  Ib_2LNJ;
      
      RestFrames::InvisibleGroup*       INV_2LNJ;
      RestFrames::SetMassInvJigsaw*     InvMass_2LNJ;
      RestFrames::SetRapidityInvJigsaw* InvRapidity_2LNJ;
      RestFrames::ContraBoostInvJigsaw* SplitINV_2LNJ;
      RestFrames::CombinatoricGroup*    JETS_2LNJ;
      
      // 2L+1L tree (Z->ll + Z/W->l)
      RestFrames::LabRecoFrame*        LAB_2L1L;
      RestFrames::DecayRecoFrame*      CM_2L1L;
      RestFrames::DecayRecoFrame*      S_2L1L;
      RestFrames::VisibleRecoFrame*    ISR_2L1L;
      
      RestFrames::DecayRecoFrame*      Ca_2L1L;
      RestFrames::DecayRecoFrame*      Z_2L1L;
      RestFrames::VisibleRecoFrame*    L1_2L1L;
      RestFrames::VisibleRecoFrame*    L2_2L1L;
      
      RestFrames::DecayRecoFrame*      Cb_2L1L;
      RestFrames::VisibleRecoFrame*    Lb_2L1L;
      
      RestFrames::InvisibleRecoFrame*  Ia_2L1L;
      RestFrames::InvisibleRecoFrame*  Ib_2L1L;
      
      RestFrames::InvisibleGroup*       INV_2L1L;
      RestFrames::SetMassInvJigsaw*     InvMass_2L1L;
      RestFrames::SetRapidityInvJigsaw* InvRapidity_2L1L;
      RestFrames::ContraBoostInvJigsaw* SplitINV_2L1L;
      
      // Debug histos

      void JetLeptonOverlapRemoval(vector<HEPUtils::Jet*> &jetvec, vector<HEPUtils::Particle*> &lepvec, double DeltaRMax) {
        //Routine to do jet-lepton check
        //Discards jets if they are within DeltaRMax of a lepton

        vector<HEPUtils::Jet*> Survivors;

        for(unsigned int itjet = 0; itjet < jetvec.size(); itjet++) {
          bool overlap = false;
          HEPUtils::P4 jetmom=jetvec.at(itjet)->mom();
          for(unsigned int itlep = 0; itlep < lepvec.size(); itlep++) {
            HEPUtils::P4 lepmom=lepvec.at(itlep)->mom();
            double dR;

            dR=jetmom.deltaR_eta(lepmom);

            if(fabs(dR) <= DeltaRMax) overlap=true;
          }
          if(overlap) continue;
          Survivors.push_back(jetvec.at(itjet));
        }
        jetvec=Survivors;

        return;
      }

      void LeptonJetOverlapRemoval(vector<HEPUtils::Particle*> &lepvec, vector<HEPUtils::Jet*> &jetvec, double DeltaRMax) {
        //Routine to do lepton-jet check
        //Discards leptons if they are within DeltaRMax of a jet

        vector<HEPUtils::Particle*> Survivors;

        for(unsigned int itlep = 0; itlep < lepvec.size(); itlep++) {
          bool overlap = false;
          HEPUtils::P4 lepmom=lepvec.at(itlep)->mom();
          for(unsigned int itjet= 0; itjet < jetvec.size(); itjet++) {
            HEPUtils::P4 jetmom=jetvec.at(itjet)->mom();
            double dR;

            dR=jetmom.deltaR_eta(lepmom);

            if(fabs(dR) <= DeltaRMax) overlap=true;
          }
          if(overlap) continue;
          Survivors.push_back(lepvec.at(itlep));
        }
        lepvec=Survivors;

        return;
      }


    public:

      Analysis_ATLAS_13TeV_RJ3L_lowmass_36invfb() {

	_num2L2JHIGH=0;
	_num2L2JINT=0;
	_num2L2JLOW=0;
	_num2L2JCOMP=0;
	_num3LHIGH=0;
	_num3LINT=0;
	_num3LLOW=0;
	_num3LCOMP=0;
		
        NCUTS=10;
        set_luminosity(36.);

        for(int i=0;i<NCUTS;i++){
          cutFlowVector.push_back(0);
          cutFlowVector_str.push_back("");
        }
	
	// Recursive jigsaw stuff
	LAB_2L2J     = new RestFrames::LabRecoFrame("LAB_2L2J","lab2L2J");
	C1N2_2L2J    = new RestFrames::DecayRecoFrame("C1N2_2L2J","#tilde{#chi}^{ #pm}_{1} #tilde{#chi}^{ 0}_{2}");
	C1a_2L2J     = new RestFrames::DecayRecoFrame("C1a_2L2J","#tilde{#chi}^{ #pm}_{1}");
	N2b_2L2J     = new RestFrames::DecayRecoFrame("N2b_2L2J","#tilde{#chi}^{ 0}_{2}");
	
	Wa_2L2J      = new RestFrames::DecayRecoFrame("Wa_2L2J","W_{a}");
	Zb_2L2J      = new RestFrames::DecayRecoFrame("Zb_2L2J","Z_{b}");
	
	J1_2L2J      = new RestFrames::VisibleRecoFrame("J1_2L2J","#it{j}_{1}");
	J2_2L2J      = new RestFrames::VisibleRecoFrame("J2_2L2J","#it{j}_{2}");
	L1_2L2J      = new RestFrames::VisibleRecoFrame("L1_2L2J","#it{l}_{1}");
	L2_2L2J      = new RestFrames::VisibleRecoFrame("L2_2L2J","#it{l}_{2}");
	
	X1a_2L2J     = new RestFrames::InvisibleRecoFrame("X1a_2L2J","#tilde{#chi}^{ 0}_{1 a}");
	X1b_2L2J     = new RestFrames::InvisibleRecoFrame("X1b_2L2J","#tilde{#chi}^{ 0}_{1 b}");
	
	
	LAB_2L2J->SetChildFrame(*C1N2_2L2J);
	
	C1N2_2L2J->AddChildFrame(*C1a_2L2J);
	C1N2_2L2J->AddChildFrame(*N2b_2L2J);
	
	C1a_2L2J->AddChildFrame(*Wa_2L2J);
	C1a_2L2J->AddChildFrame(*X1a_2L2J);
	
	N2b_2L2J->AddChildFrame(*Zb_2L2J);
	N2b_2L2J->AddChildFrame(*X1b_2L2J);
	
	Wa_2L2J->AddChildFrame(*J1_2L2J);
	Wa_2L2J->AddChildFrame(*J2_2L2J);
	
	Zb_2L2J->AddChildFrame(*L1_2L2J);
	Zb_2L2J->AddChildFrame(*L2_2L2J);
	
	
	if(LAB_2L2J->InitializeTree())
	  std::cout << "...2L2J Successfully initialized reconstruction trees" << std::endl;
	else
	  std::cout << "...2L2J Failed initializing reconstruction trees" << std::endl;
	
	
	//////////////////////////////
	//Setting the invisible
	//////////////////////////////
	INV_2L2J = new RestFrames::InvisibleGroup("INV_2L2J","#tilde{#chi}_{1}^{ 0} Jigsaws");
	INV_2L2J->AddFrame(*X1a_2L2J); 
	INV_2L2J->AddFrame(*X1b_2L2J);
	
	// Set di-LSP mass to minimum Lorentz-invariant expression
	X1_mass_2L2J = new RestFrames::SetMassInvJigsaw("X1_mass_2L2J", "Set M_{#tilde{#chi}_{1}^{ 0} #tilde{#chi}_{1}^{ 0}} to minimum");
	INV_2L2J->AddJigsaw(*X1_mass_2L2J);
	
	// Set di-LSP rapidity to that of visible particles
	X1_eta_2L2J = new RestFrames::SetRapidityInvJigsaw("X1_eta_2L2J", "#eta_{#tilde{#chi}_{1}^{ 0} #tilde{#chi}_{1}^{ 0}} = #eta_{2jet+2#it{l}}");
	INV_2L2J->AddJigsaw(*X1_eta_2L2J);
	X1_eta_2L2J->AddVisibleFrames(C1N2_2L2J->GetListVisibleFrames());
	
	
	X1X1_contra_2L2J = new RestFrames::ContraBoostInvJigsaw("X1X1_contra_2L2J","Contraboost invariant Jigsaw");
	INV_2L2J->AddJigsaw(*X1X1_contra_2L2J);
	X1X1_contra_2L2J->AddVisibleFrames(C1a_2L2J->GetListVisibleFrames(), 0);
	X1X1_contra_2L2J->AddVisibleFrames(N2b_2L2J->GetListVisibleFrames(), 1);
	X1X1_contra_2L2J->AddInvisibleFrame(*X1a_2L2J, 0);
	X1X1_contra_2L2J->AddInvisibleFrame(*X1b_2L2J, 1);
		
	LAB_2L2J->InitializeAnalysis(); 
		
	LAB_3L     = new RestFrames::LabRecoFrame("LAB_3L","lab");
	C1N2_3L    = new RestFrames::DecayRecoFrame("C1N2_3L","#tilde{#chi}^{ #pm}_{1} #tilde{#chi}^{ 0}_{2}");
	C1a_3L     = new RestFrames::DecayRecoFrame("C1a_3L","#tilde{#chi}^{ #pm}_{1}");
	N2b_3L     = new RestFrames::DecayRecoFrame("N2b_3L","#tilde{#chi}^{ 0}_{2}");
	
	L1a_3L      = new RestFrames::VisibleRecoFrame("L1a_3L","#it{l}_{1a}");
	L1b_3L      = new RestFrames::VisibleRecoFrame("L1b_3L","#it{l}_{1b}");
	L2b_3L      = new RestFrames::VisibleRecoFrame("L2b_3L","#it{l}_{2b}");
	
	X1a_3L      = new RestFrames::InvisibleRecoFrame("X1a_3L","#tilde{#chi}^{ 0}_{1 a} + #nu_{a}");
	X1b_3L      = new RestFrames::InvisibleRecoFrame("X1b_3L","#tilde{#chi}^{ 0}_{1 b}");
	
	
	LAB_3L->SetChildFrame(*C1N2_3L);
	
	C1N2_3L->AddChildFrame(*C1a_3L);
	C1N2_3L->AddChildFrame(*N2b_3L);
	
	C1a_3L->AddChildFrame(*L1a_3L);
	C1a_3L->AddChildFrame(*X1a_3L);
	
	N2b_3L->AddChildFrame(*L1b_3L);
	N2b_3L->AddChildFrame(*L2b_3L);
	N2b_3L->AddChildFrame(*X1b_3L);
	
	
	if(LAB_3L->InitializeTree())
	  std::cout << "...Contructor::3L Successfully initialized reconstruction trees" << std::endl;
	else
	  std::cout << "...Constructor::3L Failed initializing reconstruction trees" << std::endl;
	
	//setting the invisible components
	INV_3L = new RestFrames::InvisibleGroup("INV_3L","Invisible system LSP mass Jigsaw");
	INV_3L->AddFrame(*X1a_3L); 
	INV_3L->AddFrame(*X1b_3L);
	
	
	// Set di-LSP mass to minimum Lorentz-invariant expression
	X1_mass_3L = new RestFrames::SetMassInvJigsaw("X1_mass_3L", "Set M_{#tilde{#chi}_{1}^{ 0} #tilde{#chi}_{1}^{ 0}} to minimum");
	INV_3L->AddJigsaw(*X1_mass_3L);
	
	// Set di-LSP rapidity to that of visible particles and neutrino
	X1_eta_3L = new RestFrames::SetRapidityInvJigsaw("X1_eta_3L", "#eta_{#tilde{#chi}_{1}^{ 0} #tilde{#chi}_{1}^{ 0}} = #eta_{3#it{l}}");
	INV_3L->AddJigsaw(*X1_eta_3L);
	X1_eta_3L->AddVisibleFrames(C1N2_3L->GetListVisibleFrames());
	
	
	X1X1_contra_3L = new RestFrames::ContraBoostInvJigsaw("X1X1_contra_3L","Contraboost invariant Jigsaw");
	INV_3L->AddJigsaw(*X1X1_contra_3L);
	X1X1_contra_3L->AddVisibleFrames(C1a_3L->GetListVisibleFrames(),0);
	X1X1_contra_3L->AddVisibleFrames(N2b_3L->GetListVisibleFrames(),1);
	X1X1_contra_3L->AddInvisibleFrames(C1a_3L->GetListInvisibleFrames(),0);
	X1X1_contra_3L->AddInvisibleFrames(N2b_3L->GetListInvisibleFrames(),1);
	
	LAB_3L->InitializeAnalysis();
    	
      }



      void analyze(const HEPUtils::Event* event) {
        HEPUtilsAnalysis::analyze(event);

        // Missing energy
        HEPUtils::P4 ptot = event->missingmom();
    	TVector3 ETMiss;
	ETMiss.SetXYZ(ptot.px(),ptot.py(),0.0);

        // Baseline lepton objects
        vector<HEPUtils::Particle*> baselineElectrons, baselineMuons, baselineTaus;
	
        for (HEPUtils::Particle* electron : event->electrons()) {
          if (electron->pT() > 10. && electron->abseta() < 2.47) baselineElectrons.push_back(electron);
        }
        for (HEPUtils::Particle* muon : event->muons()) {
          if (muon->pT() > 10. && muon->abseta() < 2.4) baselineMuons.push_back(muon);
        }
	
	// Photons
	vector<HEPUtils::Particle*> signalPhotons;
	for (HEPUtils::Particle* photon : event->photons()) {
	  signalPhotons.push_back(photon);
        }
	
	
	// No taus used in 13 TeV analysis?
	//for (HEPUtils::Particle* tau : event->taus()) {
	//if (tau->pT() > 10. && tau->abseta() < 2.47) baselineTaus.push_back(tau);
        //}
        //ATLAS::applyTauEfficiencyR1(baselineTaus);


        // Jets
        vector<HEPUtils::Jet*> bJets;
        vector<HEPUtils::Jet*> nonBJets;
        vector<HEPUtils::Jet*> trueBJets; //for debugging

        // Get b jets
        /// @note We assume that b jets have previously been 100% tagged
        const std::vector<double>  a = {0,10.};
        const std::vector<double>  b = {0,10000.};
        const std::vector<double> c = {0.77}; // set b-tag efficiency to 77%
        HEPUtils::BinnedFn2D<double> _eff2d(a,b,c);
        for (HEPUtils::Jet* jet : event->jets()) {
          bool hasTag=has_tag(_eff2d, jet->eta(), jet->pT());
          if (jet->pT() > 20. && fabs(jet->eta()) < 4.5) {
            if(jet->btag() && hasTag && fabs(jet->eta()) < 2.4 && jet->pT() > 20.){
              bJets.push_back(jet);
            } else {
              nonBJets.push_back(jet);
            }
          }
        }


        // Overlap removal
        vector<HEPUtils::Particle*> signalElectrons;
        vector<HEPUtils::Particle*> signalMuons;
	vector<HEPUtils::Particle*> signalLeptons;
        vector<HEPUtils::Particle*> electronsForVeto;
        vector<HEPUtils::Particle*> muonsForVeto;

        vector<HEPUtils::Jet*> signalJets;
        vector<HEPUtils::Jet*> signalBJets;
        vector<HEPUtils::Jet*> signalNonBJets;

	// Overlap removal is the same as the 8 TeV analysis
        JetLeptonOverlapRemoval(nonBJets,baselineElectrons,0.2);
        LeptonJetOverlapRemoval(baselineElectrons,nonBJets,0.4);
        LeptonJetOverlapRemoval(baselineElectrons,bJets,0.4);
        LeptonJetOverlapRemoval(baselineMuons,nonBJets,0.4);
        LeptonJetOverlapRemoval(baselineMuons,bJets,0.4);

	
	// Also we have already sorted jets by their b tag properties, so reset the b tag variable for each jet to the right category
	// i.e. this was previously 100% true for true b jets then the efficiency map was applied above
        for (HEPUtils::Jet* jet : bJets) {
	  jet->set_btag(true);
	  signalJets.push_back(jet);
	  signalBJets.push_back(jet);
        }
	
        for (HEPUtils::Jet* jet : nonBJets) {
	  if(jet->pT() > 20. && fabs(jet->eta()) < 2.4) {
	    jet->set_btag(false);
	    signalJets.push_back(jet);
	    signalNonBJets.push_back(jet);
	  }
	}

        //Put signal jets in pT order
        std::sort(signalJets.begin(), signalJets.end(), sortByPT_RJ3L);
        std::sort(signalBJets.begin(), signalBJets.end(), sortByPT_RJ3L);
        std::sort(signalNonBJets.begin(), signalNonBJets.end(), sortByPT_RJ3L);

	for (HEPUtils::Particle* electron : baselineElectrons) {
          signalElectrons.push_back(electron);
	  signalLeptons.push_back(electron);
        }

        for (HEPUtils::Particle* muon : baselineMuons) {
          signalMuons.push_back(muon);
	  signalLeptons.push_back(muon);
        }
	
        // We now have the signal electrons, muons, jets and b jets- move on to the analysis
	bool m_is2Lep=false;
	bool m_is2Lep2Jet=false;
	bool m_is2L2JInt=false;
	
	bool m_is3Lep=false;
	bool m_is3LInt=false;
	bool m_is3Lep2Jet=false;
	bool m_is3Lep3Jet=false;
	
	bool m_is4Lep=false;
	bool m_is4Lep2Jet=false;
	bool m_is4Lep3Jet=false;

	bool m_foundSFOS=false;
	
	bool m_H2PP_visible = -999.;
	bool m_H2PP_invisible = -999.;
	bool m_IaPP = -999.;
	bool m_IbPP = -999.;
	bool m_IaPa = -999.;
	bool m_IbPb = -999.;
	bool m_IaLAB = -999;
	bool m_IbLAB = -999;
	bool m_H4PP_Lept1A = -999.;
	bool m_H4PP_Lept1B = -999.;
	bool m_H4PP_Lept2B = -999.;
	bool m_mu = -999;
	bool m_pileUp_weight = -999;
	
	
	//////Initialize variables
	bool m_nBaselineLeptons = -999;
	bool m_nSignalLeptons   = -999;
	
	bool m_lept1Pt  = -999;
	bool m_lept1Eta = -999;
	bool m_lept1Phi =-999;
	bool m_lept1sign=-999;
	bool m_lept1origin = -999;
	bool m_lept1type = -999;
	
	bool m_lept2Pt =-999;
	bool m_lept2Eta=-999;
	bool m_lept2Phi =-999;
	bool m_lept2sign =-999;
	bool m_lept2origin = -999;
	bool m_lept2type = -999;
	
	bool m_lept3Pt =-999;
	bool m_lept3Eta =-999;
	bool m_lept3Phi =-999;
	bool m_lept3sign =-999;
	bool m_lept3origin = -999;
	bool m_lept3type = -999;
	
	bool m_lept4Pt =-999;
	bool m_lept4Eta =-999;
	bool m_lept4Phi =-999;
	bool m_lept4sign =-999;
	bool m_lept4origin = -999;
	bool m_lept4type = -999;
	bool m_Zlep1Pt = -999;
	bool m_Zlep1Phi = -999;
	bool m_Zlep1Eta = -999;
	bool m_Zlep1No = -999;
	bool m_Zlep1sign = -999;
	
	bool m_Zlep2Pt = -999;
	bool m_Zlep2sign = -999;
	bool m_Zlep2Phi = -999;
	bool m_Zlep2Eta = -999;
	bool m_Zlep2No = -999;
	
	bool m_WlepPt = -999;
	bool m_WlepPhi = -999;
	bool m_WlepEta = -999;
	bool m_WlepNo = -999;
	bool m_Wlepsign = -999;
	
	// VR setup
	bool m_lept1Pt_VR = -999;
	bool m_lept1Eta_VR = -999;
	bool m_lept1Phi_VR = -999;
	bool m_lept1sign_VR = -999;
	
	bool m_lept2Pt_VR = -999;
	bool m_lept2Eta_VR = -999;
	bool m_lept2Phi_VR = -999;
	bool m_lept2sign_VR = -999;
	
	//Jet Variables
	bool m_nJets =0;
	bool m_nBtagJets=0;
	
	bool m_jet1Pt =-999;
	bool m_jet1Eta =-999;
	bool m_jet1Phi =-999;
	bool m_jet1M=-999;
	bool m_jet1origin=-999;
	bool m_jet1type=-999;
	
	bool m_jet2Pt=-999;
	bool m_jet2Eta=-999;
	bool m_jet2Phi=-999;
	bool m_jet2M=-999;
	bool m_jet2origin=-999;
	bool m_jet2type=-999;
	
	bool m_jet3Pt=-999;
	bool m_jet3Eta=-999;
	bool m_jet3Phi=-999;
	bool m_jet3M=-999;
	bool m_jet3origin=-999;
	bool m_jet3type=-999;
	
	bool m_jet4Pt=-999;
	bool m_jet4Eta=-999;
	bool m_jet4Phi=-999;
	bool m_jet4M=-999;
	bool m_jet4origin=-999;
	bool m_jet4type=-999;
	
	//Di-Lepton System: Calculated for OS Pairs
	bool m_mll=-999;
	bool m_mt2=-999;
	bool m_dRll=-999;
	bool m_ptll=-999;
	bool m_Zeta=-999;
	
	//Tri-Lepton System:
	bool m_mlll=-999;
	bool m_ptlll=-999;
	bool m_mTW=-999;
	bool m_mTW_alt = -999;
	bool m_mll_alt = -999;
	//Di-Jet system: Calculated for the Two Leading Jets
	bool m_mjj=-999;
	bool m_dRjj=-999;
	bool m_ptjj=-999;
	bool m_mj2j3 = -999;
	//calculation of overall jet mass
	bool m_mJ=-999;
	bool m_mjjW=-999;//closest to the W-boson mass
	
	//Cleaning Variable: If MET is in the same direction as the Jet
	bool m_minDphi=-999;
	// Some lab frame angles and stuff
	bool m_dphill = -999;
	bool m_dphilep1MET = -999;
	bool m_dphilep2MET = -999;
	bool m_dphilep3MET = -999;
	bool m_dphiJMET = -999; 
	bool m_dphilll = -999;
	bool m_dphilllMET = -999;
	bool m_dphillMET = -999;
	bool m_dphijj = -999;
	bool m_dphijet1MET = -999;
	bool m_dphijet2MET = -999;
	bool m_dphijjMET = -999;
	bool m_dphil3MET = -999; 
	bool m_MET=-999;
	bool m_MET_phi = -999;
	bool m_METTST = -999;
	bool m_METTST_phi = -999;
	bool m_Meff=-999;
	bool m_LT=-999;
	
	bool m_MDR=-999;
	bool m_PP_VisShape=-999;
	bool m_gaminvPP=-999;
	bool m_MP=-999;
	
	bool m_mC1=-999;
	bool m_mN2=-999;
	
	bool m_mTW_Pa=-999;
	bool m_mTW_PP=-999;
	
	bool m_mTZ_Pb=-999;
	bool m_mTZ_PP=-999;
	
	// 3L CA 
	bool m_min_mt = -999;
	bool m_pt_lll = -999;
	bool m_mTl3 = -999;
	//##############################//
	//# Recursive Jigsaw Variables #//
	//##############################//
	
	//Scale Variables
	bool m_H2PP=-999;
	bool m_HT2PP=-999;
	bool m_H3PP=-999;
	bool m_HT3PP=-999;
	bool m_H4PP=-999;
	bool m_HT4PP=-999;
	bool m_H5PP=-999;
	bool m_HT5PP=-999;
	bool m_H6PP=-999;
	bool m_HT6PP=-999;
	
	bool m_H2Pa=-999;
	bool m_H2Pb=-999;
	bool m_minH2P=-999;
	bool m_R_H2Pa_H2Pb=-999;
	bool m_H3Pa=-999;
	bool m_H3Pb=-999;
	bool m_minH3P=-999;
	bool m_R_H3Pa_H3Pb=-999;
	bool m_R_minH2P_minH3P=-999;
	bool m_minR_pT2i_HT3Pi=-999;
	bool m_maxR_H1PPi_H2PPi=-999;
	
	//Anglular Variables
	bool m_cosPP=-999;
	bool m_cosPa=-999;
	bool m_cosPb=-999;
	bool m_dphiVP=-999;
	bool m_dphiPPV=-999;
	bool m_dphiPC1=-999;
	bool m_dphiPN2=-999;
	
	bool m_sangle=-999;
	bool m_dangle=-999;
	
	//Ratio Variables
	bool m_RPZ_HT4PP=-999;
	bool m_RPT_HT4PP=-999;
	bool m_R_HT4PP_H4PP=-999;
	
	bool m_RPZ_HT5PP=-999;
	bool m_RPT_HT5PP=-999;
	bool m_R_HT5PP_H5PP=-999;
	bool m_W_PP = -999;
	bool m_WZ_PP = -999;
	///Variables for the compressed/Intermediate tree
	bool m_PTCM=-999;
	bool m_PTISR=-999;
	bool m_PTI=-999;
	bool m_RISR=-999;
	bool m_cosCM=-999;
	bool m_cosS=-999;
	bool m_MISR=-999;
	bool m_dphiCMI=-999;
	bool m_dphiSI=-999;
	bool m_dphiISRI=-999;
	bool m_HN2S=-999;
	bool m_R_Ib_Ia=-999;
	bool m_H11S = -999.;
	bool m_HN1Ca = -999.;
	bool m_HN1Cb = -999.;
	bool m_H11Ca = -999.;
	bool m_H11Cb = -999.;
	bool m_cosC = -999.;
	bool m_Is_Z = -999.;
	bool m_Is_OS = -999;
	bool m_MZ = -999.;
	bool m_MJ = -999.;
	bool m_mTWComp =-999.;
	bool m_cosZ = -999.;
	bool m_cosJ = -999.;
	bool m_NjS   = 0;
	bool m_NjISR = 0;
	bool m_NbS   = 0;
	bool m_NbISR = 0;
	
	bool m_MZ_VR = -999;
	bool m_MJ_VR = -999;  
	bool m_PTCM_VR = -999;
	bool m_PTISR_VR = -999;
	bool m_PTI_VR = -999;
	bool m_RISR_VR = -999;
	bool m_dphiISRI_VR = -999;
	bool m_NjS_VR = 0;
	bool m_NjISR_VR = 0;
	
	
	bool m_H2PP_VR = -999;
	bool m_H5PP_VR = -999;
	bool m_HT5PP_VR = -999;
	bool m_RPT_HT5PP_VR = -999;
	bool m_dphiVP_VR = -999;
	bool m_R_minH2P_minH3P_VR=-999;
	
	bool m_DPhi_METW = -999;
	//compressed
	bool m_WmassOnZ = -999;
	bool m_WptOnZ = -999;
	bool m_DPhi_METZ = -999;
	bool m_NonWJet_pT = -999;
	bool m_DPhi_METJetLeading = -999;
	bool m_DR_WOnZ2Jet = -999;
	bool m_DPhi_METNonWJet = -999;
	bool m_DPhi_METWonZ = -999;
	
	// Testing for low mass 3L
	bool m_M_I = -999;
	bool m_p_z_I = -999;
	bool m_p_z_Ia = -999;
	bool m_p_z_Ib = -999;
	bool m_boostx = -999;
	bool m_boosty = -999;
	bool m_boostz = -999;

	// Classify events
	
	if (signalLeptons.size()==2) m_is2Lep = true;
	else if (signalLeptons.size()==3) {m_is3Lep = true; //cout << "3L here" << endl;
	}
	else if (signalLeptons.size()==4) m_is4Lep = true;
	else return;
	
	if(m_is2Lep && m_nJets>1 ) m_is2Lep2Jet = true;
	if(m_is2Lep && m_nJets>2 ) m_is2L2JInt = true;
	if(m_is3Lep && m_nJets>0 ) m_is3LInt = true;
	if(m_is3Lep && m_nJets>1)  m_is3Lep2Jet = true; //
	if(m_is3Lep && m_nJets>2)  m_is3Lep3Jet = true; //
	if(m_is4Lep && m_nJets>1)  m_is4Lep2Jet = true; //
	if(m_is4Lep && m_nJets>2)  m_is4Lep3Jet = true; //
	
	if(signalLeptons.size()==3)m_is3Lep=true;
	
	TLorentzVector metLV;
	//TLorentzVector bigFatJet;
	metLV.SetPxPyPzE(ptot.px(),ptot.py(),0.,sqrt(ptot.px()*ptot.px()+ptot.py()*ptot.py()));

	//Put the Jets in a more useful form
	vector<TLorentzVector> myJets;
	for(unsigned int ijet=0; ijet<signalJets.size();ijet++)
	  {
	    TLorentzVector tmp;
	    tmp.SetPtEtaPhiM(signalJets[ijet]->pT(),signalJets[ijet]->eta(),signalJets[ijet]->phi(),signalJets[ijet]->mass());
	    myJets.push_back(tmp);
	  }
	
	
	//Put the Leptons in a more useful form
	vector<pair<TLorentzVector,int> > myLeptons;
	//vector<lep> myLeptons;
	for(unsigned int ilep=0; ilep<signalLeptons.size(); ilep++)
	  {
	    pair<TLorentzVector,int> temp;
	    TLorentzVector tlv_temp;
	    
	    tlv_temp.SetPtEtaPhiM(signalLeptons[ilep]->pT(),signalLeptons[ilep]->eta(),signalLeptons[ilep]->phi(),0.0);
	    temp.first = tlv_temp;
	    int lepton_charge=0;
	    if(signalLeptons[ilep]->pid()<0)lepton_charge=-1;
	    if(signalLeptons[ilep]->pid()>0)lepton_charge=1;
	    temp.second = lepton_charge;
	    //temp.third = lepton_origin->at(lep_signal_index[ilep]);
	    //temp.fourth = lepton_type->at(lep_signal_index[ilep]);
	    //temp = make_tuple(tlv_temp,lepton_charge->at(lep_signal_index[ilep]),lepton_origin->at(lep_signal_index_[ilep]),lepton_type->at(lepton_signal_index[ilep]));
	    myLeptons.push_back(temp);
	  }
	
	sort(myJets.begin(), myJets.end(), SortJets);
	sort(myLeptons.begin(), myLeptons.end(), SortLeptons);
	
	if(m_is2Lep2Jet)
	  {
	    //Creating the Lab Frame
	    LAB_2L2J->ClearEvent();
	    
	    if(myLeptons[0].first.Pt()<25000.0 || myLeptons[1].first.Pt()<25000.0) return;
	    
	    //Setting the Standard Variables
	    //Di-Lepton System:
	    m_lept1Pt   = myLeptons[0].first.Pt();
	    m_lept1Eta  = myLeptons[0].first.Eta();
	    m_lept1Phi  = myLeptons[0].first.Phi();
	    m_lept1sign = myLeptons[0].second;
	    
	    m_lept2Pt   = myLeptons[1].first.Pt();
	    m_lept2Eta  = myLeptons[1].first.Eta();
	    m_lept2Phi  = myLeptons[1].first.Phi();
	    m_lept2sign = myLeptons[1].second;
	    
	    m_mll  = (myLeptons[0].first+myLeptons[1].first).M();
	    m_ptll = (myLeptons[0].first+myLeptons[1].first).Pt();
	    m_dRll = myLeptons[0].first.DeltaR(myLeptons[1].first);
	    m_Zeta = fabs(myLeptons[0].first.Eta() - myLeptons[1].first.Eta());
	    
	    vector<TLorentzVector> vleptons;
	    vleptons.push_back(myLeptons[0].first);
	    vleptons.push_back(myLeptons[1].first);      
	    //m_mt2 = myTool.GetMt2(vleptons,metLV);      
	    
	    //min{d#phi}
	    double mindphi=100000;
	    double dphi=0;
	    TLorentzVector tempjet;
	    for(unsigned int ijet=0; ijet<signalJets.size();ijet++)
	      {
		tempjet.SetPtEtaPhiM(signalJets[ijet]->pT(),signalJets[ijet]->eta(),signalJets[ijet]->phi(),signalJets[ijet]->mass());
		
		dphi = fabs(metLV.DeltaPhi(tempjet));
		
		if(dphi<mindphi) mindphi=dphi;
	      }
	    
	    m_minDphi = mindphi;//cleaning variable for missmeasured jets;
	    
	    //just use the two leading jets
	    int indexJ1=0;
	    int indexJ2=1;
	    
	    //Di-Jet System: Here we decide which jets to use as output. The leading and sub-leading jet pair, or the jet pair with invariant mass closest to the W-Mass
	    //jet closest to the W-boson mass
	    m_jet1Pt  = myJets[indexJ1].Pt();
	    m_jet1Eta = myJets[indexJ1].Eta();
	    m_jet1Phi = myJets[indexJ1].Phi();
	    m_jet1M   = myJets[indexJ1].M();
	    
	    m_jet2Pt  = myJets[indexJ2].Pt();
	    m_jet2Eta = myJets[indexJ2].Eta();
	    m_jet2Phi = myJets[indexJ2].Phi();
	    m_jet2M   = myJets[indexJ2].M();
	    
	    if(m_nJets>2) {
	      m_jet3Pt  = myJets[2].Pt();
	      m_jet3Eta = myJets[2].Eta();
	      m_jet3Phi = myJets[2].Phi();
	      m_jet3M   = myJets[2].M();
	      m_mj2j3 = (myJets[1] + myJets[2]).M();
	      if(m_nJets>3) {
                m_jet4Pt  = myJets[3].Pt();
                m_jet4Eta = myJets[3].Eta();
                m_jet4Phi = myJets[3].Phi();
                m_jet4M   = myJets[3].M();	  
	      }
	    }
	    
	    m_mjj  = (myJets[indexJ1]+myJets[indexJ2]).M();
	    m_ptjj = (myJets[indexJ1]+myJets[indexJ2]).Pt();
	    m_dRjj = myJets[indexJ1].DeltaR(myJets[indexJ2]);
	    
	    
	    //////////////////////////////////////////////////////////////////////////////
	    //Variables for the conventional approach
	    m_DPhi_METW = fabs((myJets[indexJ1]+myJets[indexJ2]).DeltaPhi(metLV));
	    //for the comrpessed tree
	    double min_dPhi = 1000;
	    int WindexJ1 = -999;
	    int WindexJ2 = -999;
	    for (int j0=0;j0<myJets.size();j0++) {
	      
            double my_min_dphi= fabs(myJets[j0].DeltaPhi(metLV+myLeptons[0].first+myLeptons[1].first));
            if (min_dPhi>my_min_dphi) {       
	      min_dPhi = my_min_dphi;
	      WindexJ1 = j0;	  
            }	
	    }
	    
	    min_dPhi = 1000;
	    for (int j1=0;j1<myJets.size();j1++) {
	      double my_min_dphi= fabs(myJets[j1].DeltaPhi(metLV+myLeptons[0].first+myLeptons[1].first));
	      
	      if (min_dPhi>my_min_dphi) {
                if (j1!=WindexJ1) {
		  min_dPhi = my_min_dphi;
		  WindexJ2 = j1;
                }	  
	      }	
	    }
	    
	    m_WmassOnZ=(myJets[WindexJ1]+myJets[WindexJ2]).M();
	    m_WptOnZ=(myJets[WindexJ1]+myJets[WindexJ2]).Pt();
	    m_DPhi_METZ=fabs((myLeptons[0].first+myLeptons[1].first).DeltaPhi(metLV));
	    TLorentzVector nonWjetsLV;
	    for(int kjet=0;kjet<myJets.size();kjet++) {
	      if(kjet!=WindexJ1 && kjet!=WindexJ2) {
                nonWjetsLV+=myJets[kjet];
	      }
	    }
	    if(m_nJets>2) {
	      m_NonWJet_pT=nonWjetsLV.Pt();
	      m_DPhi_METJetLeading = fabs(myJets[0].DeltaPhi(metLV));
	      m_DR_WOnZ2Jet = myJets[WindexJ1].DeltaR(myJets[WindexJ2]);
	      m_DPhi_METNonWJet = fabs(nonWjetsLV.DeltaPhi(metLV));
	      m_DPhi_METWonZ = fabs((myJets[WindexJ1]+myJets[WindexJ2]).DeltaPhi(metLV));
	    }
	    //////////////////////////////////////////////////////////////////////////////
	    
	    
	    
	    L1_2L2J->SetLabFrameFourVector(myLeptons[0].first); // Set lepton 4-vectors
	    L2_2L2J->SetLabFrameFourVector(myLeptons[1].first); 
	    J1_2L2J->SetLabFrameFourVector(myJets[indexJ1]); // Set jets 4-vectors
	    J2_2L2J->SetLabFrameFourVector(myJets[indexJ2]); 
	    TVector3 MET = ETMiss;                     // Get the MET
	    MET.SetZ(0.);
	    INV_2L2J->SetLabFrameThreeVector(MET);                  // Set the MET in reco tree
	    TLorentzVector lep1;
	    TLorentzVector lep2;
	    //////////////////////////////////////////////////
	    //Lotentz vectors have been set, now do the boosts
	    //////////////////////////////////////////////////
	    LAB_2L2J->AnalyzeEvent();                               //analyze the event
	    //cout << L1_2L2J->GetFourVector(*LAB_2L2J).Pt() << endl;
	    if (L1_2L2J->GetFourVector(*LAB_2L2J).Pt() > L2_2L2J->GetFourVector(*LAB_2L2J).Pt()){
	      m_Zlep1Pt = L1_2L2J->GetFourVector(*LAB_2L2J).Pt();
	      m_Zlep1sign = myLeptons[0].second;
	      m_Zlep1No = 0;
	      m_Zlep2Pt = L2_2L2J->GetFourVector(*LAB_2L2J).Pt();
	      m_Zlep2sign = myLeptons[1].second;
	      m_Zlep2No = 1;
	      lep1 = L1_2L2J->GetFourVector(*LAB_2L2J);
	      lep2 = L2_2L2J->GetFourVector(*LAB_2L2J);
	    }
	    else {
	      
	      m_Zlep1Pt = L2_2L2J->GetFourVector(*LAB_2L2J).Pt();
	      m_Zlep1sign = myLeptons[1].second;
	      m_Zlep1No = 1;
	      m_Zlep2Pt = L1_2L2J->GetFourVector(*LAB_2L2J).Pt();
	      m_Zlep2sign = myLeptons[0].second;
	      m_Zlep2No = 0;
	      
	      lep1 = L2_2L2J->GetFourVector(*LAB_2L2J);
	      lep2 = L1_2L2J->GetFourVector(*LAB_2L2J);
	    }
	    // set the jet lab frame 4-vector 
	    TLorentzVector jet1 = J1_2L2J->GetFourVector(*LAB_2L2J);
	    TLorentzVector jet2 = J2_2L2J->GetFourVector(*LAB_2L2J);
	    
	    // Some lab frame stuff
	    m_dphill = lep1.DeltaPhi(lep2);
	    m_dphilep1MET = fabs(lep1.DeltaPhi(metLV));
	    m_dphilep2MET = fabs(lep2.DeltaPhi(metLV));
	    m_dphillMET = fabs((lep1 + lep2).DeltaPhi(metLV));
	    m_dphijet1MET = fabs(jet1.DeltaPhi(metLV));
	    m_dphijet2MET = fabs(jet2.DeltaPhi(metLV));
	    m_dphijj = fabs(jet1.DeltaPhi(jet2));
	    m_dphijjMET = fabs((jet1 + jet2).DeltaPhi(metLV));
	    //... then by setting the Variables
	    TLorentzVector vP_V1aPP = J1_2L2J->GetFourVector(*C1N2_2L2J);
	    TLorentzVector vP_V2aPP = J2_2L2J->GetFourVector(*C1N2_2L2J);
	    TLorentzVector vP_V1bPP = L1_2L2J->GetFourVector(*C1N2_2L2J);
	    TLorentzVector vP_V2bPP = L2_2L2J->GetFourVector(*C1N2_2L2J);
	    TLorentzVector vP_IaPP  = X1a_2L2J->GetFourVector(*C1N2_2L2J);
	    TLorentzVector vP_IbPP  = X1b_2L2J->GetFourVector(*C1N2_2L2J);
	    
	    TLorentzVector vP_V1aPa = J1_2L2J->GetFourVector(*C1a_2L2J);
	    TLorentzVector vP_V2aPa = J2_2L2J->GetFourVector(*C1a_2L2J);
	    TLorentzVector vP_IaPa  = X1a_2L2J->GetFourVector(*C1a_2L2J);
	    TLorentzVector vP_V1bPb = L1_2L2J->GetFourVector(*N2b_2L2J);
	    TLorentzVector vP_V2bPb = L2_2L2J->GetFourVector(*N2b_2L2J);
	    TLorentzVector vP_IbPb  = X1b_2L2J->GetFourVector(*N2b_2L2J);
	    
	    
	    //Variables w/ 4 objects 
	    //Four vector sum of all visible objets + four vector sum of inv objects
	    m_H2PP = (vP_V1aPP + vP_V2aPP + vP_V1bPP + vP_V2bPP).P() + (vP_IaPP+vP_IbPP).P();//H(1,1)PP
	    m_HT2PP = (vP_V1aPP + vP_V2aPP + vP_V1bPP + vP_V2bPP).Pt() + (vP_IaPP+vP_IbPP).Pt();//HT(1,1)PP
	    //Scalar sum of all visible objects + vector sum of invisible momenta 
	    m_H5PP = vP_V1aPP.P() + vP_V2aPP.P() + vP_V1bPP.P() + vP_V2bPP.P() + (vP_IaPP + vP_IbPP).P();//H(4,1)PP
	    m_HT5PP = vP_V1aPP.Pt() + vP_V2aPP.Pt() + vP_V1bPP.Pt() + vP_V2bPP.Pt() + (vP_IaPP + vP_IbPP).Pt();//HT(4,1)PP
	    //scalar sum of all objects 
	    m_H6PP = vP_V1aPP.P() + vP_V2aPP.P() + vP_V1bPP.P() + vP_V2bPP.P() + vP_IaPP.P() + vP_IbPP.P();//H(4,2)PP
	    m_HT6PP = vP_V1aPP.Pt() + vP_V2aPP.Pt() + vP_V1bPP.Pt() + vP_V2bPP.Pt() + vP_IaPP.Pt() + vP_IbPP.Pt();
	    
	    m_H2Pa = (vP_V1aPa + vP_V2aPa).P() + vP_IaPa.P();
	    m_H2Pb = (vP_V1bPb + vP_V2bPb).P() + vP_IbPb.P();
	    m_H3Pa = vP_V1aPa.P() + vP_V2aPa.P() + vP_IaPa.P();
	    m_H3Pb = vP_V1bPb.P() + vP_V2bPb.P() + vP_IbPb.P();
	    m_minH2P = std::min(m_H2Pa,m_H2Pb);
	    m_minH3P = std::min(m_H3Pa,m_H3Pb);
	    m_R_H2Pa_H2Pb = m_H2Pa/m_H2Pb;
	    m_R_H3Pa_H3Pb = m_H3Pa/m_H3Pb;
	    m_R_minH2P_minH3P = m_minH2P/m_minH3P;
	    double H3PTa = vP_V1aPa.Pt() + vP_V2aPa.Pt() + vP_IaPa.Pt();
	    
	    m_minR_pT2i_HT3Pi = std::min(vP_V1aPa.Pt()/H3PTa,vP_V2aPa.Pt()/H3PTa);
	    
	    
	    m_R_HT5PP_H5PP = m_HT5PP/m_H5PP;
	    
	    // Invisible in the PP frame, Px frames and lab frame
	    TLorentzVector vP_IaLAB = X1a_2L2J->GetFourVector(*LAB_2L2J);
	    TLorentzVector vP_IbLAB = X1b_2L2J->GetFourVector(*LAB_2L2J);
	    m_IaLAB = vP_IaLAB.P();
	    m_IbLAB = vP_IbLAB.P();
	    m_IaPP = vP_IaPP.P();
	    m_IbPP = vP_IbPP.P();
	    m_IaPa = vP_IaPa.P();
	    m_IbPb = vP_IbPb.P();
	    
	    double jetMetphiP = (vP_V1aPa+vP_V2aPa).DeltaPhi(vP_IaPa);
	    m_mTW_Pa = sqrt(2*(vP_V1aPa+vP_V2aPa).Pt()*vP_IaPa.Pt()*(1-cos(jetMetphiP)));
	    
	    double jetMetphiPP = (vP_V1aPP+vP_V2aPP).DeltaPhi(vP_IaPP+vP_IbPP);
	    m_mTW_PP = sqrt(2*(vP_V1aPP+vP_V2aPP).Pt()*(vP_IaPP+vP_IbPP).Pt()*(1-cos(jetMetphiPP)));  
	    
	    double dilepMetphiP = (vP_V1bPb+vP_V2bPb).DeltaPhi(vP_IbPb);
	    m_mTZ_Pb = sqrt(2*(vP_V1bPb+vP_V2bPb).Pt()*vP_IbPb.Pt()*(1-cos(dilepMetphiP)));
	    
	    double dilepMetphiPP = (vP_V1bPP+vP_V2bPP).DeltaPhi(vP_IaPP+vP_IbPP);
	    m_mTZ_PP = sqrt(2*(vP_V1bPP+vP_V2bPP).Pt()*(vP_IaPP+vP_IbPP).Pt()*(1-cos(dilepMetphiPP)));  

	    
	    double H1PPa = (vP_V1aPP + vP_V2aPP).P();
	    double H1PPb = (vP_V1bPP + vP_V2bPP).P();
	    double H2PPa = vP_V1aPP.P() + vP_V2aPP.P();
	    double H2PPb = vP_V1bPP.P() + vP_V2bPP.P();
	    m_maxR_H1PPi_H2PPi = std::max(H1PPa/H2PPa,H1PPb/H2PPb);
	    
	    // signal variables
	    TLorentzVector vP_Va = C1a_2L2J->GetVisibleFourVector(*C1a_2L2J);
	    TLorentzVector vP_Vb = N2b_2L2J->GetVisibleFourVector(*N2b_2L2J);
	    m_MP = (vP_Va.M2()-vP_Vb.M2())/(2.*(vP_Va.E()-vP_Vb.E()));
	    
	    double P_P = C1a_2L2J->GetMomentum(*C1N2_2L2J);
	    
	    double MPP = 2.*sqrt(P_P*P_P + m_MP*m_MP);
	    TVector3 vP_PP = C1N2_2L2J->GetFourVector(*LAB_2L2J).Vect();
	    double Pt_PP = vP_PP.Pt();
	    double Pz_PP = fabs(vP_PP.Pz());
	    m_RPT_HT5PP = Pt_PP / (Pt_PP + m_HT5PP);
	    m_RPZ_HT5PP = Pz_PP / (Pz_PP + m_HT5PP);
	    
	    m_PP_VisShape = C1N2_2L2J->GetVisibleShape();
	    
	    m_gaminvPP = 2.*m_MP/MPP;
	    m_MDR = m_PP_VisShape*C1N2_2L2J->GetMass();
	    
	    m_mC1 = C1a_2L2J->GetMass();
	    m_mN2 = N2b_2L2J->GetMass();
	    
	    
	    //Angular properties of the sparticles system
	    m_cosPP = C1N2_2L2J->GetCosDecayAngle(); //decay angle of the PP system
	    m_cosPa = C1a_2L2J->GetCosDecayAngle(*X1a_2L2J);//decay angle of the C1a system
	    m_cosPb = N2b_2L2J->GetCosDecayAngle(*X1b_2L2J);//decay angle of the N2b system
	    
	    //difference in azimuthal angle between the total sum of visible ojects in the C1N2 frame
	    m_dphiPPV = C1N2_2L2J->GetDeltaPhiBoostVisible();
	    m_dphiVP = C1N2_2L2J->GetDeltaPhiDecayVisible();
	    
	    //hemisphere variables
	    m_dphiPC1 = C1a_2L2J->GetDeltaPhiDecayPlanes(*Wa_2L2J);
	    m_dphiPN2 = N2b_2L2J->GetDeltaPhiDecayPlanes(*Zb_2L2J);
	    
	    m_sangle =(m_cosPa+(m_dphiVP-acos(-1.)/2.)/(acos(-1.)/2.))/2.;
	    m_dangle =(m_cosPa-(m_dphiVP-acos(-1.)/2.)/(acos(-1.)/2.))/2.;
	    
	  }//end is 2L2J event
	
	if(m_is3Lep){

	  bool m_pass3L_presel;
	  
	  if(myLeptons[0].first.Pt()<25.0 || myLeptons[1].first.Pt()<25.0 || myLeptons[2].first.Pt()<20.0)
	    {
	      m_pass3L_presel=false;
	    }
	  else{
	    m_pass3L_presel=true;
	  }
	  
	  //if(!m_pass3L_presel)return;
	  
	  //Tri-Lepton System
	  //Here we choose leptons based on where they "come from"
	  //lept1 and lept2 are the lepton pair with invariant mass closest to the Z-Mass
	  //lept3 is the remaining lepton
	  //This is meant to emulate lept1 and lept2 being produced by the Z, while lept3 is produced by the W
	  
	  double diff = 10000000000.0;
	  int Zlep1 = -99;
	  int Zlep2 = -99;
	  double Zmass = -999.0;
	  bool foundSFOS = false;
	  
	  for(unsigned int i=0; i<myLeptons.size(); i++)
	    {
	      for(unsigned int j=i+1; j<myLeptons.size(); j++)
		{
		  //Opposite-Sign
                if(myLeptons[i].second*myLeptons[j].second<0)
		  {
                    //Same-Flavor
                    if(abs(myLeptons[i].second)==abs(myLeptons[j].second))
		      {
                        double mass = (myLeptons[i].first+myLeptons[j].first).M();
			double massdiff = fabs(mass-91.1876);
                        if(massdiff<diff)
			  {
                            diff=massdiff;
                            Zmass=mass;
                            Zlep1 = i;
                            Zlep2 = j;
                            foundSFOS = true;
			  }
		      }
		  }
		}
	    }

	  if(!foundSFOS)
	    {
	      m_foundSFOS=false;
	    }
	  else {
	    m_foundSFOS=true;
	  }
	  
	  if(m_foundSFOS){

	    int Wlep1 = -999;
	    if( (Zlep1==0 && Zlep2==1) || (Zlep1==1 && Zlep2==0) ) Wlep1=2;
	    else if( (Zlep1==0 && Zlep2==2) || (Zlep1==2 && Zlep2==0) ) Wlep1=1;
	    else if((Zlep1==1 && Zlep2==2) || (Zlep1==2 && Zlep2==1) ) Wlep1=0;

	    //Knowing the indices, we perform assignments
	    m_lept1Pt   = myLeptons[0].first.Pt();
	    m_lept1sign = myLeptons[0].second;
	    
	    m_lept2Pt   = myLeptons[1].first.Pt();
	    m_lept2sign = myLeptons[1].second;
	    
	    m_lept3Pt   = myLeptons[2].first.Pt();
	    m_lept3sign = myLeptons[2].second;
	    
	    
	    m_mll  = Zmass; //based on mass minimization

	    vector<TLorentzVector> Leptons;
	    Leptons.push_back(myLeptons[Wlep1].first);
	    Leptons.push_back(myLeptons[Zlep1].first);
	    Leptons.push_back(myLeptons[Zlep2].first);
	    
	    

	    double wlepMetphi = myLeptons[Wlep1].first.DeltaPhi(metLV);
	    
	    m_mTW = sqrt(2*myLeptons[Wlep1].first.Pt()*metLV.Pt()*(1-cos(wlepMetphi)));  
	    	    
	    INV_3L->SetLabFrameThreeVector(ETMiss); //set the MET in the event
	    
	    
	    L1a_3L->SetLabFrameFourVector(Leptons[0]); // Set lepton from W
	    L1b_3L->SetLabFrameFourVector(Leptons[1]); // Set lepton1 from Z
	    L2b_3L->SetLabFrameFourVector(Leptons[2]); // Set lepton2 from Z

	    if(!LAB_3L->AnalyzeEvent()) cout << "FillEvent:: Something went wrong..." << endl;
	    
	    TLorentzVector l1;
	    TLorentzVector l2;
	    TLorentzVector l3 = L1a_3L->GetFourVector(*LAB_3L);
	  
	    //if(DEBUG) cout << "WlepPt: " << m_WlepPt << " Wlepsign: " << m_Wlepsign << endl;
	    if (L1b_3L->GetFourVector(*LAB_3L).Pt() > L2b_3L->GetFourVector(*LAB_3L).Pt()){
	      l1 = L1b_3L->GetFourVector(*LAB_3L);
	      l2 = L2b_3L->GetFourVector(*LAB_3L);
	      
	    }
	    else {
	      
	   
	      l2 = L1b_3L->GetFourVector(*LAB_3L);
	      l1 = L2b_3L->GetFourVector(*LAB_3L);
	    }


	    // More lab frame stuff
 
	    //if(DEBUG)  cout << "Zlep1: " << m_Zlep1Pt << " " << m_Zlep1sign << " Zlep2Pt: " << m_Zlep2Pt << " " << m_Zlep2sign << endl;
	    TLorentzVector vP_V1aPP  = L1a_3L->GetFourVector(*C1N2_3L);
	    TLorentzVector vP_V1bPP  = L1b_3L->GetFourVector(*C1N2_3L);
	    TLorentzVector vP_V2bPP  = L2b_3L->GetFourVector(*C1N2_3L);
	    TLorentzVector vP_I1aPP  = X1a_3L->GetFourVector(*C1N2_3L);
	    TLorentzVector vP_I1bPP  = X1b_3L->GetFourVector(*C1N2_3L);
	    
	    TLorentzVector vP_V1aPa  = L1a_3L->GetFourVector(*C1a_3L);
	    TLorentzVector vP_I1aPa  = X1a_3L->GetFourVector(*C1a_3L);
	    
	    TLorentzVector vP_V1bPb = L1b_3L->GetFourVector(*N2b_3L);
	    TLorentzVector vP_V2bPb = L2b_3L->GetFourVector(*N2b_3L);
	    TLorentzVector vP_I1bPb = X1b_3L->GetFourVector(*N2b_3L);
	    
	    

	    //Variables w/ 4 objects 
	    
	    /// Defined in the PP-frame
	    //Four vector sum of all visible objets + four vector sum of inv objects
	  	    
	    //Scalar sum of all visible objects + vector sum of invisible momenta 
	    m_H4PP = vP_V1aPP.P() + vP_V1bPP.P() + vP_V2bPP.P() + (vP_I1aPP + vP_I1bPP).P();//H(3,1)PP
	    m_HT4PP = vP_V1aPP.Pt() + vP_V1bPP.Pt() + vP_V2bPP.Pt() + (vP_I1aPP + vP_I1bPP).Pt();//HT(3,1)PP
	    
	    // Invisible components again
	    TLorentzVector vP_IaLAB = X1a_3L->GetFourVector(*LAB_3L);
	    TLorentzVector vP_IbLAB = X1b_3L->GetFourVector(*LAB_3L);
	  
	    
	    // Testing for low mass 3L
	    TLorentzVector p_Ia_Lab = X1a_3L->GetFourVector(*LAB_3L);
	    TLorentzVector p_Ib_Lab = X1b_3L->GetFourVector(*LAB_3L);
	    TVector3 lab_to_pp = C1N2_3L->GetBoostInParentFrame();
	    
	    /// Defined in the P-frame    
	    
	    ////Calculation of dRll_I_PP;
	    //m_dRll_I_PP = (vP_V1bPP+vP_V1bPP).DeltaR(vP_I1bPP);
	    //m_R_Ib_Ia = (vP_V1bPP + vP_V2bPP + vP_I1bPP).P()/(vP_V1aPP+vP_I1aPP).P();
	    
	    // signal variables
	    TLorentzVector vP_Va = C1a_3L->GetVisibleFourVector(*C1a_3L);
	    TLorentzVector vP_Vb = N2b_3L->GetVisibleFourVector(*N2b_3L);
	    
	    TVector3 vP_PP = C1N2_3L->GetFourVector(*LAB_3L).Vect();
	    double Pt_PP = vP_PP.Pt();
	    m_RPT_HT4PP = Pt_PP / (Pt_PP + m_HT4PP);
	    
	    	    
	    // mt_min here
	    
	    /*double min0 = -999;
	    double min1 = -999;
	    double min2 = -999;
	    double lepmetphi0 = myLeptons[0].first.DeltaPhi(metLV);
	    double lepmetphi1 = myLeptons[1].first.DeltaPhi(metLV);
	    double lepmetphi2 = myLeptons[2].first.DeltaPhi(metLV);

	    if (myLeptons[0].second == -myLeptons[1].second) min0 =  sqrt(2*myLeptons[2].first.Pt()*metLV.Pt()*(1-cos(lepmetphi2)));
	    if (myLeptons[0].second == -myLeptons[2].second) min1 =  sqrt(2*myLeptons[1].first.Pt()*metLV.Pt()*(1-cos(lepmetphi1)));
	    if (myLeptons[1].second == -myLeptons[2].second) min2 =  sqrt(2*myLeptons[0].first.Pt()*metLV.Pt()*(1-cos(lepmetphi0)));
	    
	    if (min0 > 0 && min1 > 0) m_min_mt = min(min0,min1);
	    else if (min0 > 0 && min2 > 0) m_min_mt = min(min0,min2);
	    else if (min1 > 0 && min2 > 0) m_min_mt = min(min1,min2);
	    else if (min0 > 0 && min1 < 0 && min2 < 0) m_min_mt = min0;
	    else if (min1 > 0 && min0 < 0 && min2 < 0) m_min_mt = min1;
	    else if (min2 > 0 && min0 < 0 && min1 <0) m_min_mt = min2;*/
	  	    
	    
	  } // end of if(m_foundSFOS)
	} // end of m_is3Lep
	


	// Cutflow check

	cutFlowVector_str[0] = "No cuts ";
        cutFlowVector_str[1] = "Preselection ";
        cutFlowVector_str[2] = "75 GeV < mll < 105 GeV ";
        cutFlowVector_str[3] = "mTW > 100 GeV ";
        cutFlowVector_str[4] = "m_HT4PP/m_H4PP > 0.9 ";
        cutFlowVector_str[5] = "m_H4PP > 250 GeV ";
        cutFlowVector_str[6] = "pT_PP/(pT_PP + HT_PP(3,1)) ";
	
	for(int j=0;j<NCUTS;j++){
	  
	  if( (j==0) ||
	      
	      (j==1 && m_is3Lep && (((m_lept1sign*m_lept2sign<0 && abs(m_lept1sign)==abs(m_lept2sign)) || (m_lept1sign*m_lept3sign<0 && abs(m_lept1sign)==abs(m_lept3sign)) || (m_lept2sign*m_lept3sign<0 && abs(m_lept2sign)==abs(m_lept3sign)))) && m_lept1Pt>60. && m_lept2Pt>40. && m_lept3Pt>30. && signalBJets.size()==0 && signalJets.size()==0) ||
	      
	      (j==2 && m_is3Lep && (((m_lept1sign*m_lept2sign<0 && abs(m_lept1sign)==abs(m_lept2sign)) || (m_lept1sign*m_lept3sign<0 && abs(m_lept1sign)==abs(m_lept3sign)) || (m_lept2sign*m_lept3sign<0 && abs(m_lept2sign)==abs(m_lept3sign)))) && m_lept1Pt>60. && m_lept2Pt>40. && m_lept3Pt>30. && signalBJets.size()==0 && signalJets.size()==0 && m_mll>75. && m_mll<105.) ||
	      
	      (j==3 && m_is3Lep && (((m_lept1sign*m_lept2sign<0 && abs(m_lept1sign)==abs(m_lept2sign)) || (m_lept1sign*m_lept3sign<0 && abs(m_lept1sign)==abs(m_lept3sign)) || (m_lept2sign*m_lept3sign<0 && abs(m_lept2sign)==abs(m_lept3sign)))) && m_lept1Pt>60. && m_lept2Pt>40. && m_lept3Pt>30. && signalBJets.size()==0 && signalJets.size()==0 && m_mll>75. && m_mll<105. && m_mTW>100.) ||
	      
	      (j==4 && m_is3Lep && (((m_lept1sign*m_lept2sign<0 && abs(m_lept1sign)==abs(m_lept2sign)) || (m_lept1sign*m_lept3sign<0 && abs(m_lept1sign)==abs(m_lept3sign)) || (m_lept2sign*m_lept3sign<0 && abs(m_lept2sign)==abs(m_lept3sign)))) && m_lept1Pt>60. && m_lept2Pt>40. && m_lept3Pt>30. && signalBJets.size()==0 && signalJets.size()==0 && m_mll>75. && m_mll<105. && m_mTW>100. && m_HT4PP/m_H4PP > 0.9) ||
	      
	      (j==5 && m_is3Lep && (((m_lept1sign*m_lept2sign<0 && abs(m_lept1sign)==abs(m_lept2sign)) || (m_lept1sign*m_lept3sign<0 && abs(m_lept1sign)==abs(m_lept3sign)) || (m_lept2sign*m_lept3sign<0 && abs(m_lept2sign)==abs(m_lept3sign)))) && m_lept1Pt>60. && m_lept2Pt>40. && m_lept3Pt>30. && signalBJets.size()==0 && signalJets.size()==0 && m_mll>75. && m_mll<105. && m_mTW>100. && m_HT4PP/m_H4PP > 0.9 && m_H4PP > 250.) ||
	      
	      (j==6 && m_is3Lep && (((m_lept1sign*m_lept2sign<0 && abs(m_lept1sign)==abs(m_lept2sign)) || (m_lept1sign*m_lept3sign<0 && abs(m_lept1sign)==abs(m_lept3sign)) || (m_lept2sign*m_lept3sign<0 && abs(m_lept2sign)==abs(m_lept3sign)))) && m_lept1Pt>60. && m_lept2Pt>40. && m_lept3Pt>30. && signalBJets.size()==0 && signalJets.size()==0 && m_mll>75. && m_mll<105. && m_mTW>100. && m_HT4PP/m_H4PP > 0.9 && m_H4PP > 250. && m_RPT_HT4PP < 0.05)
	      
	      )cutFlowVector[j]++;
	}
	
	// Now apply the signal region cuts

	if(m_is2Lep2Jet && m_lept1sign*m_lept2sign<0 && abs(m_lept1sign)==abs(m_lept2sign) && m_lept1Pt>25. && m_lept2Pt>25. && m_jet1Pt>30. && m_jet2Pt>30. && signalBJets.size()==0 && signalJets.size()>=2 && m_mll>80. && m_mll<100. && m_mjj>60. && m_mjj<100. && m_R_minH2P_minH3P>0.8 && m_RPT_HT5PP< 0.05 && m_dphiVP>0.3 && m_dphiVP<2.8 && m_H5PP>800.)_num2L2JHIGH++;

	if(m_is2Lep2Jet && m_lept1sign*m_lept2sign<0 && abs(m_lept1sign)==abs(m_lept2sign) && m_lept1Pt>25. && m_lept2Pt>25. && m_jet1Pt>30. && m_jet2Pt>30. && signalBJets.size()==0 && signalJets.size()>=2 && m_mll>80. && m_mll<100. && m_mjj>60. && m_mjj<100. && m_R_minH2P_minH3P>0.8 && m_RPT_HT5PP<0.05 && m_dphiVP>0.6 && m_dphiVP<2.6 && m_H5PP>600.)_num2L2JINT++;


	if(m_is2Lep2Jet && m_lept1sign*m_lept2sign<0 && abs(m_lept1sign)==abs(m_lept2sign) && m_lept1Pt>25. && m_lept2Pt>25. && m_jet1Pt>30. && m_jet2Pt>30. && signalBJets.size()==0 && signalJets.size()==2 && m_mll>80. && m_mll<100. && m_mjj>70. && m_mjj<90. && m_H2PP/m_H5PP>0.35 && m_H2PP/m_H5PP<0.6 && m_RPT_HT5PP<0.05 && m_minDphi>2.4 && m_H5PP>400.)_num2L2JLOW++;

	if(m_is2L2JInt && m_lept1sign*m_lept2sign<0 && abs(m_lept1sign)==abs(m_lept2sign) && m_lept1Pt>25. && m_lept2Pt>25. && m_jet1Pt>30. && m_jet2Pt>30. && signalBJets.size()==0 && m_NjS==2 && m_NjISR<3 && m_MZ>80. && m_MZ<100. &&  m_MJ>500. && m_MJ<110. && m_dphiISRI>2.8 && m_RISR > 0.40 && m_RISR < 0.75 && m_PTISR > 180. && m_PTI > 100. && m_PTCM < 20.)_num2L2JCOMP++;

	if(m_is3Lep && (((m_lept1sign*m_lept2sign<0 && abs(m_lept1sign)==abs(m_lept2sign)) || (m_lept1sign*m_lept3sign<0 && abs(m_lept1sign)==abs(m_lept3sign)) || (m_lept2sign*m_lept3sign<0 && abs(m_lept2sign)==abs(m_lept3sign)))) && m_lept1Pt>60. && m_lept2Pt>60. && m_lept3Pt>40. && signalBJets.size()==0 && signalJets.size()<3 && m_mll>75. && m_mll<105. && m_mTW>150. && m_HT4PP/m_H4PP > 0.75 && m_R_minH2P_minH3P>0.8 && m_H4PP > 550. && m_RPT_HT4PP < 0.2)_num3LHIGH++;

	if(m_is3Lep && (((m_lept1sign*m_lept2sign<0 && abs(m_lept1sign)==abs(m_lept2sign)) || (m_lept1sign*m_lept3sign<0 && abs(m_lept1sign)==abs(m_lept3sign)) || (m_lept2sign*m_lept3sign<0 && abs(m_lept2sign)==abs(m_lept3sign)))) && m_lept1Pt>60. && m_lept2Pt>50. && m_lept3Pt>30. && signalBJets.size()==0 && signalJets.size()<3 && m_mll>75. && m_mll<105. && m_mTW>130. && m_HT4PP/m_H4PP > 0.8 && m_R_minH2P_minH3P>0.75 && m_H4PP > 450. && m_RPT_HT4PP < 0.15)_num3LINT++;

	if(m_is3LInt && (((m_lept1sign*m_lept2sign<0 && abs(m_lept1sign)==abs(m_lept2sign)) || (m_lept1sign*m_lept3sign<0 && abs(m_lept1sign)==abs(m_lept3sign)) || (m_lept2sign*m_lept3sign<0 && abs(m_lept2sign)==abs(m_lept3sign)))) && m_lept1Pt>25. && m_lept2Pt>25. && m_lept3Pt>20. && signalBJets.size()==0 && signalJets.size()<4 && m_mll>75000 && m_mll<105. && m_mTW>100. && m_dphiISRI>2.0 && m_RISR>0.55 && m_RISR<1.0 && m_PTISR>100. && m_PTI>80. && m_PTCM<25.)_num3LCOMP++;
	
	if(m_is3Lep && (((m_lept1sign*m_lept2sign<0 && abs(m_lept1sign)==abs(m_lept2sign)) || (m_lept1sign*m_lept3sign<0 && abs(m_lept1sign)==abs(m_lept3sign)) || (m_lept2sign*m_lept3sign<0 && abs(m_lept2sign)==abs(m_lept3sign)))) && m_lept1Pt>60. && m_lept2Pt>40. && m_lept3Pt>30. && signalBJets.size()==0 && signalJets.size()==0 && m_mll>75. && m_mll<105. && m_mTW>100. && m_HT4PP/m_H4PP > 0.9 && m_H4PP > 250. && m_RPT_HT4PP < 0.05)_num3LLOW++;
	
	return;
	
      }
      

      void add(BaseAnalysis* other) {
        // The base class add function handles the signal region vector and total # events.
        HEPUtilsAnalysis::add(other);

        Analysis_ATLAS_13TeV_RJ3L_lowmass_36invfb* specificOther
                = dynamic_cast<Analysis_ATLAS_13TeV_RJ3L_lowmass_36invfb*>(other);

        // Here we will add the subclass member variables:
        if (NCUTS != specificOther->NCUTS) NCUTS = specificOther->NCUTS;
        for (int j=0; j<NCUTS; j++) {
          cutFlowVector[j] += specificOther->cutFlowVector[j];
          cutFlowVector_str[j] = specificOther->cutFlowVector_str[j];
        }

	_num3LLOW+= specificOther->_num3LLOW;

      }


      void collect_results() {

	double scale_by=1.;
	cout << "------------------------------------------------------------------------------------------------------------------------------ "<<endl;
	cout << "CUT FLOW: ATLAS 13 TeV 3 lep low mass RJ signal region "<<endl;
	cout << "------------------------------------------------------------------------------------------------------------------------------"<<endl;
	cout<< right << setw(40) << "CUT" << setw(20) << "RAW" << setw(20) << "SCALED"
	    << setw(20) << "%" << setw(20) << "clean adj RAW"<< setw(20) << "clean adj %" << endl;
	for (int j=0; j<NCUTS; j++) {
	  cout << right << setw(40) << cutFlowVector_str[j].c_str() << setw(20)
	       << cutFlowVector[j] << setw(20) << cutFlowVector[j]*scale_by << setw(20)
	       << 100.*cutFlowVector[j]/cutFlowVector[0] << "%" << setw(20)
	       << cutFlowVector[j]*scale_by << setw(20) << 100.*cutFlowVector[j]/cutFlowVector[0]<< "%" << endl;
	}
	cout << "------------------------------------------------------------------------------------------------------------------------------ "<<endl;

	/// Register results objects with the results for each SR; obs & bkg numbers from the paper

	static const string ANAME = "Analysis_ATLAS_13TeV_RJ3L_lowmass_36invfb";

	/*int _numSRA_TT, _numSRA_TW, _numSRA_T0;
	int _numSRB_TT, _numSRB_TW, _numSRB_T0;
	int _numSRC1, _numSRC2, _numSRC3, _numSRC4, _numSRC5;
	int _numSRD_low, _numSRD_high, _numSRE;*/
	
        add_result(SignalRegionData(ANAME, "3LLOW", 20, {_num3LLOW,  0.}, {10.31, 1.96}));     
	
        /*SignalRegionData results_SRA_TT;
        results_SRA_TT.analysis_name = "Analysis_ATLAS_13TeV_RJ3L_lowmass_36invfb";
        results_SRA_TT.sr_label = "SRA_TT";
        results_SRA_TT.n_observed = 11.;
        results_SRA_TT.n_background = 15.8;
        results_SRA_TT.background_sys = 1.9;
        results_SRA_TT.signal_sys = 0.;
        results_SRA_TT.n_signal = _numSRA1;

        add_result(results_SRA_TT);*/
	
	//deletion of RJ 3-lepton pointers
	delete LAB_3L;
	delete C1N2_3L;
	delete C1a_3L;
	delete N2b_3L;
	delete L1a_3L;
	delete L1b_3L;
	delete L2b_3L;
	delete X1a_3L;
	delete X1b_3L;
	delete INV_3L;
	delete X1_mass_3L;
	delete X1_eta_3L;
	delete X1X1_contra_3L;
	
	
        return;
      }

    };


    DEFINE_ANALYSIS_FACTORY(ATLAS_13TeV_RJ3L_lowmass_36invfb)


  }
}
