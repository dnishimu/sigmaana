//////////////////////////////////////
///       Sigma_calc_C macro       ///
//////////////////////////////////////
//This program was initially developed by Daiki Nishimura on May 2024.
//ver1.0.0 is released on June 8th 2024. 
//ver1.1.0 is released on June 20th 2024. 
//ver1.1.1 is released on June 21th 2024. 
//
// write revision here
// On June 11th 2024
// Limit of F5X of +-60mm is added
// AoQ57woPLA calibration range is revised from -3%~+3% to 2sigma
// On June 13th 2024
// AoQ35_5 gate is widened to 6sigma
// June 14th 2024
// Transmission check of F3XA,F3YB,F5XF3A are added
// Emittance cut range is shown in transmission check figures
// NUCLIDE, BEAM,TARGET names are extraced from input file name
// When out range of F7PLAX from 35mm is judged, noF7PLA.
// F7X_2 deduced from F7PPAC1 is corrected by using Fpl_XT[3] of F3A information 
// On June 15  2024
// itteration of getXStats is added for 5 times with 3-sigma zoom.
// On June 16th 2024
// deduction of dZet7& Zet7_sigma is changed.
// [important]alpha_2 is added by using actual Brho57 with F7PPAC
// odd AoQ57_7 correction with Delta57_4 is removed.
// Two Gaussian function is changed to be the same sigma value
// On June 18th 2024
// ellipse of emittance of 2-d PID35 is added.
// Information of F7X and n_F7s are written in resuluts_out.
// On June 19th
// outF7PLA changed to be made from not F7X_PLATdiff_2 but F7X_PLATdiff (debug). 
// out_F7PLA_Qcheck is added for clean up AoQ57_5 by transporting the edge of F7PLA
// PID57_SE(south east elimination gate) is removed.
// AoQ57 and AoQ57woPLA gates are set with +-5 sigma of mean AoQ57 values
// several 1-d transmission plots can be created from 2-d transimission plots
// On June 20th 
// Unused parts have been removed.
// On June 21th
// Subroutine of EmittanceManualCorrection is added.
// Q of F3PLA is clean up by not Zet3 but raw F3IC_Esum2
// clean-up gate for Q of F5PLA is removed -> it is added again. 
// Zet7 gate for transmission is changed from only for lower limit to for both of lower and higher limits
// Case1, Case2 and Case3 are named for downstream gates condition
// number of unreacted events in Case3(noF7PLA&noF7PPAC events) is estimated by using noF7PLA events.
// On June 25th
// F5X of +/- 70mm is tried.
//Pre_PUR is changed from -8000ns to -6000ns.

#include "Rtypes.h"
#include <iostream>
#include <fstream>
#include <TMarker.h>
#include <cmath>
#include <TMath.h>

void EmittanceManualCorrection(const string& nuclide, const string& beam, const string& optics_para, double& limit, bool& flag, double& value,  ofstream& fout) {
  
  double f3b_limit_left = -22.;
  double f3b_limit_right =+22.;
  if(optics_para == "f3b_left"&& value <f3b_limit_left){
    limit = f3b_limit_left;
    flag = true;
  } else if(optics_para == "f3b_right"&& value > f3b_limit_right){
    limit = f3b_limit_right;
    flag = true;
  }

  double f5x_limit_left= -70.;
  double f5x_limit_right =+70.;
  if(optics_para == "f5x_left"&& value <f5x_limit_left){
    limit = f5x_limit_left;
    flag = true;
  } else if(optics_para == "f5x_right"&& value > f5x_limit_right){
    limit = f5x_limit_right;
    flag = true;
  }
  double f5x_narrow_limit_left= -37.;
  double f5x_narrow_limit_right= -37.;
  if((beam == "54Co" ||beam=="47Cr")&&optics_para == "f5x_left"&& value <f5x_narrow_limit_left){
    limit = f5x_narrow_limit_left;
    flag = true;
  } else if((beam == "54Co" ||beam=="47Cr")&&optics_para == "f5x_right"&& value > f5x_narrow_limit_right){
    limit = f5x_narrow_limit_right;
    flag = true;
  }

  // Set your personal emittance cut correction here
  if (nuclide == "51Fe" && beam == "52Fe" && optics_para == "f3x_right") {
    limit = 8.0;
    flag = true;
  } else if (nuclide == "52Fe" && beam == "52Fe" && optics_para == "f3x_right") {
    limit = 2.0;
    flag = true;
  } 
  if (flag) {
    cout << "special correction of " << optics_para << "_limit = " << value << "-> " << limit << " for " << nuclide << " in " << beam << endl;
    fout << "special correction of " << optics_para << "_limit = " << value << "-> " << limit << " for " << nuclide << " in " << beam << endl;
    value = limit;
  }
}


void Sigma_calc(const string& input_filename)
{
  /////////////////////////////
  /////////////////////////////
  //Section 1 Initialization
  /////////////////////////////
  /////////////////////////////
  double n_sigma_Zet3 = 4.0;//default is 4.0 sigma.
  double n_sigma_AoQ35 = 6.0;//default is 6.0 sigma.
  double w_normal_F7PLA = 30.;
  double n_sigma_AoQ57 = 5.0;
  double n_sigma_AoQ57woPLA = 5.0;
  double n_sigma_Zet7 = 3.0;
  double n_sigma_f3x = 2.0;
  double n_sigma_f3a = 2.0;
  double n_sigma_f3y = 2.0;
  double n_sigma_f3b = 2.0;
  double n_sigma_f5x = 3.0;
  double Pre_PUR = -6000.;
  double Post_PUR = 2500.;
  double Pre_PUR2 = -4000.;
  double Post_PUR2 = 2000.;

  int Flag_All = 0;// Drawing flag for upsteam condition
  int Flag_Emittance = 0;//Drawing flag for emittance cut
  int Flag_Fix_up_para = 0;//fix flag for the upstream parameters
#define HBK_WRITE
  //Set analysis evnt numbers
  int Neve_all = tree-> GetEntries();//read of the number of all events           
  double R_n = 1.0;//reduction factor of decording events
  //Set nuclide
  double A0 ;//mass number of nuclide of interest
  double Z0 ;//atomic number of nuclide of interest 
  //Set parameters of PID gates for each nucleus
  double dZet3 =0.00001;
  double Zet3_sigma =0.1201;
  double dAoQ35 =0.000001;
  double AoQ35_sigma = 0.003001; 
  double dZet7 =0.001;
  double Zet7_sigma =0.2;
  double dAoQ57 =0.00001;
  double AoQ57_sigma =0.004001;
  double dZet7woPLA =0.000;// woPLA means without F7PLA information
  double Zet7woPLA_sigma =0.2;
  double dAoQ57woPLA = 0.000;
  double AoQ57woPLA_sigma =0.004;
  double D57_AoQ57woPLA_slope = 0.000;// slope param. for Delta57 vs AoQ57 without F7PLA plot this is no used in ver1.1.0
  double D57_AoQ57woPLA_offset = 0.000;// offset param. for Delta57 vs AoQ57 without F7PLA plot  this is no used in ver1.1.0 
  //Set F7X-X correlation
  double F7X_PLATdiff_offset = 3.1;// timing offset(ns) 
  double F7X_PLATdiff_slope =9.5;//slope parameter (mm/ns) for F7PLATdiff to F7X position
  double f3x_mu,f3x_sigma,f3a_mu,f3a_sigma,f3y_mu,f3y_sigma,f3b_mu,f3b_sigma,f5x_left,f5x_right;
  double  f3x_right_limit; 
  double f3x_left,f3x_right,f3a_left,f3a_right,f3y_left,f3y_right,f3b_left,f3b_right;
  double  f3x_right_limit; 
  double f3x_left_limit,f3x_right_limit,f3a_left_limit,f3a_right_limit,f3y_left_limit,f3y_right_limit,f3b_left_limit,f3b_right_limit,f5x_left_limit,f5x_right_limit;
  bool Flag_f3x_left_cor = false;
  bool Flag_f3x_right_cor = false;
  bool Flag_f3a_left_cor = false;
  bool Flag_f3a_right_cor = false;
  bool Flag_f3y_left_cor = false;
  bool Flag_f3y_right_cor = false;
  bool Flag_f3b_left_cor = false;
  bool Flag_f3b_right_cor = false;
  bool Flag_f5x_left_cor = false;
  bool Flag_f5x_right_cor = false;

  //Set parameters for PID spectra for each beam settting
  double AoQ35_offset=0.007;
  double AoQ35_slope=1.000;
  double Zet3_offset=+0.05;
  double Zet3_slope=1.000;
  double AoQ57_offset=0.000;
  double AoQ57_slope=1.000;
  double Zet7_offset=0.20;
  double Zet7_slope=1.000;

  //Set variable parameters of PID gates after this trial run
  double dZet3_exp;
  double Zet3_mu_exp;
  double Zet3_sigma_exp;
  double dAoQ35_exp;
  double AoQ35_mu_exp;
  double AoQ35_sigma_exp; 
  double dZet7_exp;
  double Zet7_mu_exp;
  double Zet7_sigma_exp;
  double dAoQ57_exp;
  double AoQ57_mu_exp;
  double AoQ57_sigma_exp;
  double dZet7woPLA_exp;
  double Zet7woPLA_mu_exp;
  double Zet7woPLA_sigma_exp;
  double dAoQ57woPLA_exp;
  double AoQ57woPLA_mu_exp;
  double AoQ57woPLA_sigma_exp;
  double f3x_mu_exp,f3x_sigma_exp,f3a_mu_exp,f3a_sigma_exp,f3y_mu_exp,f3y_sigma_exp,f3b_mu_exp,f3b_sigma_exp,f5x_left_exp,f5x_right_exp;
  double f5x_mu_exp,f5x_sigma_exp;
  double f7x_ppac_mu_exp,f7x_ppac_sigma_exp,f7x_pla_mu_exp,f7x_pla_sigma_exp;
  double F7X_PLATdiff_offset_exp;
  double F7X_PLATdiff_slope_exp;
  double D57_AoQ57woPLA_offset_exp;
  double D57_AoQ57woPLA_slope_exp;
  double delta57wopla_mu_exp,delta57wopla_sigma_exp,aoq57wopla_mu_exp,aoq57wopla_sigma_exp;//added on June 11th 2024
  double n0,n1;//number of incoming partcles and that of non-reacted paticles 

  //Read and set of input, output, results files
  //  gDirectory->Delete("TH1*");
  string full_input_path = "./input/" + input_filename; //input file
  size_t startPos = input_filename.find('input_');
  startPos += strlen("input_");
  size_t endPos = input_filename.find_last_of('.');
  string name = input_filename.substr(startPos , endPos - startPos );
  string output_filename = "./output/output_"+name+".dat";//output file 
  ofstream fout(output_filename.c_str());
  string results_filename = "./results/results_"+name+".dat";//results file
  ofstream results_out(results_filename.c_str());

  cout << name << endl;
  fout << name << endl;

    // Find the positions of the underscores
  size_t startPos1 = input_filename.find('_') + 1;
  size_t endPos1 = input_filename.find('_', startPos1);
    
  size_t startPos2 = endPos1 + 1;
  size_t endPos2 = input_filename.find('_', startPos2);
    
  size_t startPos3 = endPos2 + 1;
  size_t endPos3 = input_filename.find('_', startPos3);
    
  // Extract the substrings
  string nuclide = input_filename.substr(startPos1, endPos1 - startPos1);
  string beam = input_filename.substr(startPos2, endPos2 - startPos2);
  string target = input_filename.substr(startPos3, endPos3 - startPos3);
    
  cout << "NUCLIDE: " << nuclide << endl;
  cout << "BEAM: " << beam << endl;
  cout << "TARGET: " << target << endl;
  fout << "NUCLIDE: " << nuclide << endl;
  fout << "BEAM: " << beam << endl;
  fout << "TARGET: " << target << endl;

  //Reading inputfile
  ifstream inputFile(full_input_path.c_str());  
  if (!inputFile) {
    cerr << "Error opening input file!" << endl;
    return;
  }
  string line;
  while (getline(inputFile, line)) {
    if (line[0] == '#') {
      continue;
    }
    istringstream iss(line);
    string key;
    double value;
    if (!(iss >> key >> value)) {
      cerr << "Error reading line: " << line << endl;
      continue; // 
    }
    if(key == "R_n") {R_n = value; 
    } else if (key == "Flag_All") {Flag_All = value;
    } else if (key == "Flag_Emittance") {Flag_Emittance = value;
    } else if (key == "Flag_Fix_up_para") {Flag_Fix_up_para = value;
    } else if (key == "A0") {A0 = value;
    } else if (key == "Z0") {Z0 = value;
    } else if (key == "dZet3") {dZet3 = value;
    } else if (key == "Zet3_sigma") {Zet3_sigma = value;
    } else if (key == "dAoQ35") {dAoQ35 = value;
    } else if (key == "AoQ35_sigma") {AoQ35_sigma = value;
    } else if (key == "dZet7") {dZet7 = value;
    } else if (key == "Zet7_sigma") {Zet7_sigma = value;
    } else if (key == "dAoQ57") { dAoQ57= value;
    } else if (key == "AoQ57_sigma") { AoQ57_sigma= value;
    } else if (key == "dAoQ57woPLA") { dAoQ57woPLA= value;
    } else if (key == "AoQ57woPLA_sigma") { AoQ57woPLA_sigma= value;
    } else if (key == "f3x_mu") { f3x_mu= value;
    } else if (key == "f3x_sigma") { f3x_sigma= value;
    } else if (key == "f3a_mu") { f3a_mu= value;
    } else if (key == "f3a_sigma") { f3a_sigma= value;
    } else if (key == "f3y_mu") { f3y_mu= value;
    } else if (key == "f3y_sigma") { f3y_sigma= value;
    } else if (key == "f3b_mu") { f3b_mu= value;
    } else if (key == "f3b_sigma") { f3b_sigma= value;
    } else if (key == "f5x_left") { f5x_left= value;
    } else if (key == "f5x_right") { f5x_right= value;
    } else if (key == "F7X_PLATdiff_offset") { F7X_PLATdiff_offset= value;
    } else if (key == "F7X_PLATdiff_slope") { F7X_PLATdiff_slope= value;
    } else if (key == "D57_AoQ57woPLA_slope") { D57_AoQ57woPLA_slope= value; // this parameter is not used from ver.1.1.0
    } else if (key == "AoQ35_offset") { AoQ35_offset= value;
    } else if (key == "AoQ35_slope") { AoQ35_slope= value;
    } else if (key == "Zet3_offset") { Zet3_offset= value;
    } else if (key == "Zet3_slope") { Zet3_slope= value;
    } else if (key == "AoQ57_offset") { AoQ57_offset= value;
    } else if (key == "AoQ57_slope") { AoQ57_slope= value;
    } else if (key == "Zet7_offset") { Zet7_offset= value;
    } else if (key == "Zet7_slope") { Zet7_slope= value;
    } else {
      cerr << "Unknown parameter: " << key << endl;
    }
    }

  //Set the nuclide of interest and event numbers to be analysed. Ranges are also set. 
  int Neve=Neve_all*R_n;//
  double AoQ0 = A0/Z0;
  double AoQ_min = AoQ0-0.25;//Set the range of AoQ histogram
  double AoQ_max = AoQ0+0.25;
  double Z3_min = 10;//Set the range of Zet histogram
  double Z3_max = 32.;
  double Z7_min = 0;//Set the range of Zet histogram
  double Z7_max = 32.;
  double f3x_min =-30.;
  double f3x_max = 30.;
  double Delta_min = -3;
  double Delta_max = 3;
  double dDelta_min = -6;
  double dDelta_max = 6;
  double Beta_min = 0.45;
  double Beta_max = 0.75;
  double PLAT_min = -15000.;
  double PLAT_max = 10000.;

  /////////////////////////
  /////////////////////////
  //Section 2 TCut gate Setting 
  /////////////////////////
  /////////////////////////


  /////////////////////////
  //for upstream
  /////////////////////////
  TCut Zet3 = Form("abs((Zet3_3-(%f+%f))/(%f*%f))<1.0",Z0,dZet3,n_sigma_Zet3,Zet3_sigma);
  TCut AoQ35 = Form("abs((AoQ35_5-(%f+%f))/(%f*%f))<1.0",AoQ0,dAoQ35,n_sigma_AoQ35,AoQ35_sigma);
  TCut PID35 = Form("((Zet3_3-(%f+%f))/(%f*%f))**2+((AoQ35_5-(%f+%f))/(%f*%f))**2<1.0**2",Z0,dZet3,n_sigma_Zet3,Zet3_sigma,AoQ0,dAoQ35,n_sigma_AoQ35,AoQ35_sigma);//
  // TCut PUR = "(PLA_MT[3][1][1]-PLA_MT[3][1][0])>3000||PLA_MT[3][1][1]<-200";//Pile-up rejection by using Multi-hit timing of F3PLA_R
  TCut PUR = Form("(abs(PLA_MT[3][0][0]-11840)<30.&&((PLA_MT[3][0][1]-PLA_MT[3][0][0])>%f||PLA_MT[3][0][1]<-200))||(abs(PLA_MT[3][0][1]-11840)<30.&&((PLA_MT[3][0][2]-PLA_MT[3][0][1])>%f||PLA_MT[3][0][2]<-200)&&(PLA_MT[3][0][0]-PLA_MT[3][0][1])<%f)||(abs(PLA_MT[3][0][2]-11840)<30.&&((PLA_MT[3][0][3]-PLA_MT[3][0][2])>%f||PLA_MT[3][0][3]<-200)&&(PLA_MT[3][0][1]-PLA_MT[3][0][2])<%f)||(abs(PLA_MT[3][0][3]-11840)<30.&&((PLA_MT[3][0][4]-PLA_MT[3][0][3])>%f||PLA_MT[3][0][4]<-200)&&(PLA_MT[3][0][2]-PLA_MT[3][0][3])<%f)",Post_PUR,Post_PUR,Pre_PUR,Post_PUR,Pre_PUR,Post_PUR,Pre_PUR);
  TCut PUR2 = Form("(abs(F3PLA2_MT[0][0]-11840)<30.&&((F3PLA2_MT[0][1]-F3PLA2_MT[0][0])>%f||F3PLA2_MT[0][1]<-200))||(abs(F3PLA2_MT[0][1]-11840)<30.&&((F3PLA2_MT[0][2]-F3PLA2_MT[0][1])>%f||F3PLA2_MT[0][2]<-200)&&(F3PLA2_MT[0][0]-F3PLA2_MT[0][1])<%f)||(abs(F3PLA2_MT[0][2]-11840)<30.&&((F3PLA2_MT[0][3]-F3PLA2_MT[0][2])>%f||F3PLA2_MT[0][3]<-200)&&(F3PLA2_MT[0][1]-F3PLA2_MT[0][2])<%f)||(abs(F3PLA2_MT[0][3]-11840)<30.&&((F3PLA2_MT[0][4]-F3PLA2_MT[0][3])>%f||F3PLA2_MT[0][4]<-200)&&(F3PLA2_MT[0][2]-F3PLA2_MT[0][3])<%f)",Post_PUR2,Post_PUR2,Pre_PUR2,Post_PUR2,Pre_PUR2,Post_PUR2,Pre_PUR2);
  //  TCut PUR2 = "abs((abs(F3PLA2_MT[0][4]-PLA_MT[3][1][0])-(1300.)))>1200.&&abs((abs(F3PLA2_MT[0][3]-PLA_MT[3][1][0])-(1300.)))>1200.&&abs((abs(F3PLA2_MT[0][2]-PLA_MT[3][1][0])-(1300.)))>1200.&&abs((abs(F3PLA2_MT[0][1]-PLA_MT[3][1][0])-(1300.)))>1200.&&abs((F3PLA2_MT[0][0]-PLA_MT[3][1][0]-(-1300.)))>1200.";//Pile-up rejection by using Multi-hit timing of F3PLA2_L
  TCut PLA3F3IC = "abs((PLA_Qave[3][1]-100.)-0.070*F3IC_Esum2)<150";//TOF35 clean up by using F3PLA_Q and F3IC correlation
  TCut PLA5F3IC = "abs((PLA_Qave[5][1]+236)*(1.-0.000025*(F5X_2-0.0)**2.)-0.825*(F3IC_Esum2+1000)**0.8)<250.";//TOF35 clean up by using F5PLA_Q and F3IC correlation with F5X correction
  //Set emittance cut
  //f3x
  f3x_left = f3x_mu -n_sigma_f3x *f3x_sigma;  f3x_right = f3x_mu +n_sigma_f3x*f3x_sigma;
  EmittanceManualCorrection(nuclide, beam, "f3x_left", f3x_left_limit, Flag_f3x_left_cor, f3x_left, fout);
  EmittanceManualCorrection(nuclide, beam, "f3x_right", f3x_right_limit, Flag_f3x_right_cor, f3x_right, fout);
  TCut F3X = Form("(Fpl_XP[3]>%f)&&(Fpl_XP[3]<%f)",f3x_left,f3x_right);
  //f3a
  f3a_left = f3a_mu -n_sigma_f3a*f3a_sigma;  f3a_right = f3a_mu +n_sigma_f3a*f3a_sigma;
  EmittanceManualCorrection(nuclide, beam, "f3a_left", f3a_left_limit, Flag_f3a_left_cor, f3a_left, fout);
  EmittanceManualCorrection(nuclide, beam, "f3a_right", f3a_right_limit, Flag_f3a_right_cor, f3a_right, fout);
  TCut F3A = Form("(Fpl_XT[3]>%f)&&(Fpl_XT[3]<%f)",f3a_left,f3a_right);
  //f3y
  f3y_left = f3y_mu -n_sigma_f3y*f3y_sigma;  f3y_right = f3y_mu +n_sigma_f3y*f3y_sigma;
  EmittanceManualCorrection(nuclide, beam, "f3y_left", f3y_left_limit, Flag_f3y_left_cor, f3y_left, fout);
  EmittanceManualCorrection(nuclide, beam, "f3y_right", f3y_right_limit, Flag_f3y_right_cor, f3y_right, fout);
  TCut F3Y = Form("(Fpl_YP[3]>%f)&&(Fpl_YP[3]<%f)",f3y_left,f3y_right);
  //f3b
  f3b_left = f3b_mu -n_sigma_f3b*f3b_sigma;  f3b_right = f3b_mu +n_sigma_f3b*f3b_sigma;
  EmittanceManualCorrection(nuclide, beam, "f3b_left", f3b_left_limit, Flag_f3b_left_cor, f3b_left, fout);
  EmittanceManualCorrection(nuclide, beam, "f3b_right", f3b_right_limit, Flag_f3b_right_cor, f3b_right, fout);
  TCut F3B = Form("(Fpl_YT[3]>%f)&&(Fpl_YT[3]<%f)",f3b_left,f3b_right);
  //f5x
  //f5x_left and f5x_right are determined by Draw part( Emittance cut for F5X  )
  EmittanceManualCorrection(nuclide, beam, "f5x_left", f5x_left_limit, Flag_f5x_left_cor, f5x_left, fout);
  EmittanceManualCorrection(nuclide, beam, "f5x_right", f5x_right_limit, Flag_f5x_right_cor, f5x_right, fout);
  TCut F5X = Form("(F5X_2>%f)&&(F5X_2<%f)",f5x_left,f5x_right);
  //
  TCut F3XA = "abs(Fpl_XP[3]+0.5*Fpl_XT[3])<150.";// opened
  //TCut CU = PUR&&PUR2&&PLA3F3IC&&PLA5F3IC;//all of Clean Up gates
  TCut CU = PUR&&PUR2&&PLA3F3IC&&PLA5F3IC;//all of Clean Up gates
  //  TCut CU = PUR&&PUR2&&PLA3F3IC;//all of Clean Up gates PLA5F3IC is removed
  TCut Em_all = F3X&&F3A&&F3Y&&F3B&&F5X&&F3XA;//all of Emittance cut gates

  /////////////////////////
  //for downstream
  /////////////////////////
  TCut Zet7 = Form("abs((Zet7wob_2-(%f+%f))/%f)<%f",Z0,dZet7,Zet7_sigma,n_sigma_Zet7);//olny lower limit is set for transmission check
  TCut out_F7PLA_Qcheck = "abs((PLA_Qave[7][1]-100)-(0.045*F7IC_Esum2))>200";
  TCut outF7PLA = Form("abs(F7X_PLATdiff)>%f",w_normal_F7PLA);
  TCut noAoQ57= "AoQ57_5<1.0";//no AoQ57 i.e. no hit of F7PLA 
  TCut noF7PLA= noAoQ57||outF7PLA||out_F7PLA_Qcheck;//no F7PLA 
  TCut AoQ57= Form("abs((AoQ57_5-(%f+%f))/%f)<%f",AoQ0,dAoQ57,AoQ57_sigma,n_sigma_AoQ57);//AoQ57 gate for Case1(!noF7PLA)
  TCut AoQ57woPLA= Form("abs((AoQ57_7-(%f+%f))/%f)<%f",AoQ0,dAoQ57woPLA,AoQ57woPLA_sigma,n_sigma_AoQ57woPLA);//AoQ57 gate for Case2(noF7PLA&&!noF7PPAC1)
  TCut noF7PPAC1 = "F7X_2<-150.";// no hit of F7PPAC1
  TCut PID57 = Form("((Zet7_4-(%f+%f))/0.206)**2+((AoQ57_5-(%f/%f+%f))/(2.5*0.0031))**2<4.0**2",Z0,dZet7,A0,Z0,dAoQ57);//this gate is only for check
  TCut Case1 = !noF7PLA;//
  TCut gCase1 = AoQ57;// gate for Case1
  TCut Case2 = !Case1&&!noF7PPAC1;
  TCut gCase2 = AoQ57woPLA;// gate for Case2
  TCut Case3 = !Case1&&!Case2;

  TCanvas *c1 = new TCanvas("c1","Draw",0,1920,1000,1000);
  c1->SetLogz();
  //  c1->SetLogy();
  gStyle->SetOptStat(11111111);

  ///////////////////////////////
  ///////////////////////////////
  //Section 3 Definition of new branches
  //////////////////////////////
  ///////////////////////////////
  double Clight = 2.99792458E-01;// speed of light in a unit of m/ns
  double Mnucleon = 9.3149432E-01;// atomic mass unit in a unit of GeV/c^2
  //////////////////
  // for upstream //
  //////////////////
  tree->SetAlias("F3IC_Esum2","F3IC_Eraw[0]+F3IC_Eraw[1]+F3IC_Eraw[2]+F3IC_Eraw[3]+F3IC_Eraw[4]+F3IC_Eraw[5]");//deltaE of F7IC without energy calibration
  tree->SetAlias("F5X_2","((X[5][0]>-200)*(X[5][1]>-200)*(abs(X[5][0]-(X[5][1]-1.3))<3.)*(X[5][0]+(X[5][1]-1.3))/2.+(X[5][0]>-200)*(X[5][1]<-200)*X[5][0]+(X[5][0]<-200)*(X[5][1]>-200)*(X[5][1]-1.3)+(X[5][0]<-200)*(X[5][1]<-200)*(-9999)+(X[5][0]>-200)*(X[5][1]>-200)*(abs(X[5][0]-X[5][1]+1.3)>=3.)*(-9999))-Fpl_XT[3]*0.001*(-400.)");// F5X deduced by F5PPAC1at Focal plane of target position 
  tree->SetAlias("Delta35_3","( F5X_2 - 9.20250E-01 * Fpl_XP[3] ) / 3.16870E+01");
  tree->SetAlias("Beta35_3","(Delta35_3>-5)*(23.318-0.001*(4.01337E+00 *Delta35_3 + 3.44330E+00 *Fpl_XT[3]))/(TOF35-0.3)/(0.299792458)");
  tree->SetAlias("Brho35_3","Brho_NMR[2] * (1.0 + Delta35_3 * 0.01)");
  tree->SetAlias("AoQ35_3","Brho35_3/(Beta35_3/sqrt(1.-Beta35_3**2))*(0.299792458)/0.93149432");//optics correction
  tree->SetAlias("AoQ35_4","AoQ35_3*(1.-(PLA_Qave[3][1]**(-0.5)-0.04)*0.3)");//slew correction
  tree->SetAlias("AoQ35_5",Form("%f+%f*AoQ35_4",AoQ35_offset,AoQ35_slope));//fine tuning of AoQ35 calibration
  tree->SetAlias("Zet3_3",Form("%f+%f*Zet3_2",Zet3_offset,Zet3_slope));//fine tuning of Zet3 calibration
  tree->SetAlias("Pre_F3PLA_LT","((abs(PLA_MT[3][0][1]-11840)<30.)*(PLA_MT[3][0][0]-PLA_MT[3][0][1])) +((abs(PLA_MT[3][0][2]-11840)<30.)*(PLA_MT[3][0][1]-PLA_MT[3][0][2])) +((abs(PLA_MT[3][0][3]-11840)<30.)*(PLA_MT[3][0][2]-PLA_MT[3][0][3])) +((abs(PLA_MT[3][0][4]-11840)<30.)*(PLA_MT[3][0][3]-PLA_MT[3][0][4]))");
  tree->SetAlias("Post_F3PLA_LT","(abs(PLA_MT[3][0][0]-11840)<30.)*(PLA_MT[3][0][1]-PLA_MT[3][0][0])+(abs(PLA_MT[3][0][1]-11840)<30.)*(PLA_MT[3][0][2]-PLA_MT[3][0][1])+(abs(PLA_MT[3][0][2]-11840)<30.)*(PLA_MT[3][0][3]-PLA_MT[2][0][3])+(abs(PLA_MT[3][0][3]-11840)<30.)*(PLA_MT[3][0][4]-PLA_MT[3][0][3])");
  tree->SetAlias("Pre_F3PLA2_LT","((abs(F3PLA2_MT[0][1]-11840)<30.)*(F3PLA2_MT[0][0]-F3PLA2_MT[0][1])) +((abs(F3PLA2_MT[0][2]-11840)<30.)*(F3PLA2_MT[0][1]-F3PLA2_MT[0][2])) +((abs(F3PLA2_MT[0][3]-11840)<30.)*(F3PLA2_MT[0][2]-F3PLA2_MT[0][3])) +((abs(F3PLA2_MT[0][4]-11840)<30.)*(F3PLA2_MT[0][3]-F3PLA2_MT[0][4]))");
  tree->SetAlias("Post_F3PLA2_LT","(abs(F3PLA2_MT[0][0]-11840)<30.)*(F3PLA2_MT[0][1]-F3PLA2_MT[0][0])+(abs(F3PLA2_MT[0][1]-11840)<30.)*(F3PLA2_MT[0][2]-F3PLA2_MT[0][1])+(abs(F3PLA2_MT[0][2]-11840)<30.)*(F3PLA2_MT[0][3]-F3PLA2_MT[0][2])+(abs(F3PLA2_MT[0][3]-11840)<30.)*(F3PLA2_MT[0][4]-F3PLA2_MT[0][3])");
  //////////////////
  // for downstream //
  //////////////////
  //F7PLA_2
  tree->SetAlias("F7_PLAQave_2","log((PLA_Q[7][0]) * (PLA_Q[7][1])**1.2)");//not used now
  tree->SetAlias("F7_PLAQdiff_2","+0.6*(PLA_Q[7][0])**2 - 0.5*(PLA_Q[7][1])**2");//not used now
  tree->SetAlias("TOF57_2","1.1*0.95*0.0075*(F7PLA2_Traw[0]+F7PLA2_Traw[1])-PLA_Tave[3]+132.");//by F7PLA2 with small threshold.
  //F7X
  tree->SetAlias("F7X_2","((X[7][1]<-1000)*(X[7][0])+(X[7][0]<-1000)*(X[7][1]+0.8)+(X[7][0]>-1000)*(X[7][1]>-1000)*(((X[7][0]+0.8)+X[7][1])/2.))*(1.-0.025*Fpl_XT[3])");//optical correction is added 
  tree->SetAlias("F7X_PLATdiff",Form("(PLA_Tave[7]>-2400)*(-(PLA_Tdiff[7]**1.-%f)*%f)+(PLA_Tave[7]<-2400)*(-9999)",F7X_PLATdiff_offset,F7X_PLATdiff_slope ));
  tree->SetAlias("F7X_PLATdiff_2",Form("((F7PLA2_Traw[0]+F7PLA2_Traw[1])>-50000)*(-((0.032*(F7PLA2_Traw[0]-F7PLA2_Traw[1])-1.76)**1.-%f)*%f)+((F7PLA2_Traw[0]+F7PLA2_Traw[1])<-50000)*(-9999)",F7X_PLATdiff_offset,F7X_PLATdiff_slope ));//not used now    
  //Delta
  tree->SetAlias("Delta57_3","(F7X_PLATdiff-1.08732E+00*F5X_2)/(-3.44541E+01)");//by F7PLA
  tree->SetAlias("Delta57_4","(F7X_2+0.0011*850.*Fpl_XT[3]-1.08732E+00*F5X_2)/(-3.44541E+01)");//by F7PPAC1
  //dDelta
  tree->SetAlias("dDelta","Delta57_4-Delta35_3");
  //Brho57
  tree->SetAlias("Brho57_3","Brho_NMR[4]*(1.+0.01*Delta57_3)");//by F7PLA
  tree->SetAlias("Brho57_4","Brho_NMR[4]*(1.+0.01*Delta57_4)");//by F7PPAC1
  //Beta
  tree->SetAlias("Beta57_3",Form("(23.658-0.001*(3.93341E+00*Delta57_3-%f*3.16870E+00*Fpl_XT[3]))/(TOF57-0.2)/(0.299792458)",0.0));//by TOF52 of standard threshold
  tree->SetAlias("Beta57_4",Form("(23.658-0.001*(3.93341E+00*Delta57_3-%f*3.16870E+00*Fpl_XT[3]))/(TOF57_2-0.2)/(0.299792458)",0.0));//by TOF57_2 of low threshold
  tree->SetAlias("alpha","Brho_NMR[4]/Brho_NMR[2]*(1.-(Beta35_2)**2.)**(-0.5)*Beta35_2");//
  tree->SetAlias("Beta57wob","alpha*(1+alpha**2)**(-0.5)");//Beta57pseudo predicted by Beta35
  //AoQ57
  tree->SetAlias("AoQ57_3","Brho57_3/(Beta57_3/sqrt(1.-Beta57_3**2))*0.299792458/0.93149432");
  //	tree->SetAlias("AoQ57_4","AoQ57_3*(1.+(PLA_Qave[7][1]**(-0.5)-0.06)*0.0)");//slew correction
  tree->SetAlias("AoQ57_5",Form("%f+%f*AoQ57_3",AoQ57_offset,AoQ57_slope));//fine tuning for AoQ57 with F7PLA
  tree->SetAlias("AoQ57_6","AoQ35_5*(1.+0.01*dDelta)");       
  tree->SetAlias("AoQ57_7",Form("%f+%f*AoQ57_6",AoQ57_offset,AoQ57_slope));//fine tuning for AoQ57 without F7PLA

  //Zet7
  tree->SetAlias("F7IC_Esum2","(F7IC_Eraw[0]<8192)*F7IC_Eraw[0]+(F7IC_Eraw[1]<8192)*F7IC_Eraw[1]+(F7IC_Eraw[3]<8192)*F7IC_Eraw[3]+(F7IC_Eraw[4]<8192)*F7IC_Eraw[4]+(F7IC_Eraw[5]<8192)*F7IC_Eraw[5]");//each channel calibration is not performed currently
  tree->SetAlias("Zet7_3","0.3*sqrt(F7IC_Esum2)*Beta57_2");//Zet7 calculated with TOF57 but not used in ver1.1.0
  tree->SetAlias("Zet7_4",Form("%f+%f*Zet7_3",Zet7_offset,Zet7_slope));//fine tunig of Zet7 using Beta57
  tree->SetAlias("Zet7wob","0.3*sqrt(F7IC_Esum2)*alpha*(1+alpha**2)**(-0.5)");//Beta57pseudo correction is used.
  tree->SetAlias("Zet7wob_2",Form("%f+%f*Zet7wob",Zet7_offset,Zet7_slope));// fine tuning of of Zet7 with Beta57pseudo

  //////////////////////////////////
  //////////////////////////////////
  //Section 4  Drawing for cross section deduction
  //////////////////////////////////
  //////////////////////////////////////////
    cout << "Upstream calculation" << endl; 
  //////////////////////////////////////////
    // PID 35 plots
    if(Flag_All){
      tree->Draw(Form("Zet3_3:Pre_F3PLA_LT>>pref3plaZet3_all(400,%f,%f,400,%f,%f)",PLAT_min,PLAT_max,Z3_min,Z3_max),"","colz",Neve);//for all events
      tree->Draw(Form("Zet3_3:Pre_F3PLA_LT>>pref3plaZet3_cleanup(400,%f,%f,400,%f,%f)",PLAT_min,PLAT_max,Z3_min,Z3_max),CU,"colz",Neve);//for all events
      tree->Draw(Form("Zet3_3:Post_F3PLA_LT>>postf3plaZet3_all(400,%f,%f,400,%f,%f)",PLAT_min,PLAT_max,Z3_min,Z3_max),"","colz",Neve);//for all events
      tree->Draw(Form("Zet3_3:Post_F3PLA_LT>>postf3plaZet3_cleanup(400,%f,%f,400,%f,%f)",PLAT_min,PLAT_max,Z3_min,Z3_max),CU,"colz",Neve);//for clean-up events
      tree->Draw(Form("Zet3_3:Pre_F3PLA2_LT>>pref3pla2Zet3_all(400,%f,%f,400,%f,%f)",PLAT_min,PLAT_max,Z3_min,Z3_max),"","colz",Neve);//for all events
      tree->Draw(Form("Zet3_3:Pre_F3PLA2_LT>>pref3pla2Zet3_cleanup(400,%f,%f,400,%f,%f)",PLAT_min,PLAT_max,Z3_min,Z3_max),CU,"colz",Neve);//for clean-up  events
      tree->Draw(Form("Zet3_3:Post_F3PLA2_LT>>postf3pla2Zet3_all(400,%f,%f,400,%f,%f)",PLAT_min,PLAT_max,Z3_min,Z3_max),"","colz",Neve);//for all events
      tree->Draw(Form("Zet3_3:Post_F3PLA2_LT>>postf3pla2Zet3_cleanup(400,%f,%f,400,%f,%f)",PLAT_min,PLAT_max,Z3_min,Z3_max),CU,"colz",Neve);//for clean-up  events
      tree->Draw(Form("Zet7_3:Pre_F3PLA_LT>>pref3plaZet7_all(400,%f,%f,400,%f,%f)",PLAT_min,PLAT_max,Z7_min,Z7_max),"","colz",Neve);//for all events
      tree->Draw(Form("Zet7_3:Pre_F3PLA_LT>>pref3plaZet7_cleanup(400,%f,%f,400,%f,%f)",PLAT_min,PLAT_max,Z7_min,Z7_max),CU,"colz",Neve);//for clean-up  events
      tree->Draw(Form("Zet7_3:Post_F3PLA_LT>>postf3plaZet7_all(400,%f,%f,400,%f,%f)",PLAT_min,PLAT_max,Z7_min,Z7_max),"","colz",Neve);//for all events
      tree->Draw(Form("Zet7_3:Post_F3PLA_LT>>postf3plaZet7_cleanup(400,%f,%f,400,%f,%f)",PLAT_min,PLAT_max,Z7_min,Z7_max),CU,"colz",Neve);//for clean-up  events
      tree->Draw("PLA_Qave[3][1]:F3IC_Esum2>>pla3f3ic_all(400,0,30000,400,0,4096)","","colz",Neve);//for all events    
      tree->Draw("PLA_Qave[3][1]:F3IC_Esum2>>pla3f3ic_cleanup(400,0,30000,400,0,4096)",CU,"colz",Neve);//for all events    
      tree->Draw("PLA_Qave[5][1]:F3IC_Esum2>>pla5f3ic_all(400,0,30000,400,0,4096)","","colz",Neve);//for all events    
      tree->Draw("PLA_Qave[5][1]:F3IC_Esum2>>pla5f3ic_cleanup(400,0,30000,400,0,4096)",CU,"colz",Neve);//for all events    
    }
    if(Flag_All)tree->Draw(Form("Zet3_3:AoQ35_5>>pid35_all(400,%f,%f,400,%f,%f)",AoQ_min,AoQ_max,Z3_min,Z3_max),"","colz",Neve);//for all events
    if(Flag_All)tree->Draw(Form("Zet3_3:AoQ35_5>>pid35_cleanup(400,%f,%f,400,%f,%f)",AoQ_min,AoQ_max,Z3_min,Z3_max),CU,"colz",Neve);//for the events with Clean up of Pile-up rejection and F3&F5PLA charge normal in comparison with Zet3.
    tree->Draw(Form("Zet3_3:AoQ35_5>>pid_35_g_woEm(400,%f,%f,400,%f,%f)",AoQ_min,AoQ_max,Z3_min,Z3_max),CU&&PID35,"colz",Neve);//for the nuclei of interest with this PID35 gate
    Zet3_mu_exp=Z0+dZet3;//initial value of Zet3
    Zet3_sigma_exp=Zet3_sigma;//initial value of sigma of Zet3
    AoQ35_mu_exp=AoQ0+dAoQ35;//initial value of AoQ35
    AoQ35_sigma_exp=AoQ35_sigma;//initial value of sigma of AoQ35
    getXYStats(pid_35_g_woEm,AoQ35_mu_exp,AoQ35_sigma_exp,Zet3_mu_exp,Zet3_sigma_exp,2,3.);//the above 4 parameters are obtained from this PID35 plot.
    cout << "AoQ35 vs Zet3: " << AoQ35_mu_exp <<"pm "<<AoQ35_sigma_exp <<", "<<Zet3_mu_exp <<"pm "<<Zet3_sigma_exp << endl;
    fout << "AoQ35 vs Zet3: " << AoQ35_mu_exp <<"pm "<<AoQ35_sigma_exp <<", "<<Zet3_mu_exp <<"pm "<<Zet3_sigma_exp << endl;
    dAoQ35_exp = AoQ35_mu_exp-AoQ0;//push back the pamameter to next calculation 
    dZet3_exp = Zet3_mu_exp-Z0;//push back the pamameter to next calculation 
    if(Flag_All)tree->Draw(Form("Zet3_3:AoQ35_5>>pid_35_g(400,%f,%f,400,%f,%f)",AoQ_min,AoQ_max,Z3_min,Z3_max),CU&&Em_all&&PID35,"colz",Neve);//PID35 plot with all upstream gates
    // Emittance plots
    tree->Draw("Fpl_XT[3]:Fpl_XP[3]>>f3xa_pid(400,-30,30,400,-30,30)",CU&&PID35,"colz",Neve);
    getXYStats(f3xa_pid,f3x_mu_exp,f3x_sigma_exp,f3a_mu_exp,f3a_sigma_exp,1,3.);
    if(Flag_All)tree->Draw("Fpl_XT[3]:Fpl_XP[3]>>f3xa_u(400,-30,30,400,-30,30)",CU&&Em_all&&PID35,"colz",Neve);
    tree->Draw("Fpl_YT[3]:Fpl_YP[3]>>f3yb_pid(400,-30,30,400,-30,30)",CU&&PID35,"colz",Neve);
    getXYStats(f3yb_pid,f3y_mu_exp,f3y_sigma_exp,f3b_mu_exp,f3b_sigma_exp,1,3.);
    if(Flag_All)tree->Draw("Fpl_YT[3]:Fpl_YP[3]>>f3yb_u(400,-30,30,400,-30,30)",CU&&Em_all&&PID35,"colz",Neve);
    if(Flag_All)tree->Draw("Fpl_YP[5]:Fpl_XP[5]>>f5xy(400,-120,120,400,-50,50)",CU&&PID35,"colz",Neve);
    if(Flag_All)tree->Draw("Fpl_YP[5]:Fpl_XP[5]>>f5xy_u(400,-120,120,400,-50,50)",CU&&Em_all&&PID35,"colz",Neve);
    // Emittance cut for F5X
    tree->Draw("F5X_2>>f5x_pid35(240,-120,120)",CU&&F3X&&F3A&&F3Y&&F3B&&F3XA&&PID35,"colz",Neve);
    getXStats(f5x_pid35,f5x_mu_exp,f5x_sigma_exp,1,3.);
    f5x_left_exp = f5x_mu_exp - n_sigma_f5x * f5x_sigma_exp;
    f5x_right_exp = f5x_mu_exp + n_sigma_f5x * f5x_sigma_exp;
    // F3X vs Zet3 for the check of simmilar AoQ35 value isotopes
    if(Flag_All)tree->Draw(Form("Zet3_3:Fpl_XP[3]>>f3xz3_cu(400,%f,%f,400,%f,%f)",f3x_min,f3x_max,Z3_min,Z3_max),CU,"colz",Neve);
    if(Flag_All)tree->Draw(Form("Zet3_3:Fpl_XP[3]>>f3xz3_pid35(400,%f,%f,400,%f,%f)",f3x_min,f3x_max,Z3_min,Z3_max),CU&&Em_all&&PID35,"colz",Neve);

    // F7X with F7PLA calibration for downstream PID
    tree->Draw("F7X_PLATdiff:Fpl_XP[7]>>f7xx(400,-100,100,400,-100,100)",CU&&Em_all,"colz",Neve);
    PrfXFitPol1(f7xx,F7X_PLATdiff_slope,F7X_PLATdiff_offset,F7X_PLATdiff_slope_exp,F7X_PLATdiff_offset_exp,-30.,30.,fout);
    cout << "F7X_PLATdiff: before= "<<F7X_PLATdiff_slope <<", "<< F7X_PLATdiff_offset <<"->after: "<< F7X_PLATdiff_slope_exp <<", "<< F7X_PLATdiff_offset_exp <<endl;
    fout << "F7X_PLATdiff: before= "<<F7X_PLATdiff_slope <<", "<< F7X_PLATdiff_offset <<"->after: "<< F7X_PLATdiff_slope_exp <<", "<< F7X_PLATdiff_offset_exp <<endl;
    if(Flag_All)tree->Draw("F7X_PLATdiff:Fpl_XP[7]>>f7xx_u(400,-100,100,400,-100,100)",CU&&Em_all&&PID35,"colz",Neve);
    if(Flag_All)tree->Draw("F7X_PLATdiff:Fpl_XP[7]>>f7xx_d(400,-100,100,400,-100,100)",CU&&Em_all&&PID35&&PID57,"colz",Neve);
    if(Flag_All)getXYStats(f7xx_u,f7x_ppac_mu_exp,f7x_ppac_sigma_exp,f7x_pla_mu_exp,f7x_pla_sigma_exp,1,3.);
    if(Flag_All)cout << "F7XPPAC(mu,sigma)=(" <<f7x_ppac_mu_exp <<", "<<f7x_ppac_sigma_exp <<"),F7XPLA(mu,sigma)=("<<f7x_pla_mu_exp <<", "<<f7x_pla_sigma_exp <<")" <<endl;  
    if(Flag_All)fout << "F7XPPAC(mu,sigma)=(" <<f7x_ppac_mu_exp <<", "<<f7x_ppac_sigma_exp <<"),F7XPLA(mu,sigma)=("<<f7x_pla_mu_exp <<", "<<f7x_pla_sigma_exp <<")" <<endl;  

  // for get mean X position for correction of wedge target thickness
  tree->Draw("F5X_2>>f5x_u(240,-120,120)",CU&&Em_all&&PID35,"colz",Neve);	
  double f5x_mu_actual = f5x_mu_exp;//added on June 18th
  double f5x_sigma_actual = f5x_sigma_exp;//added on June 18th
  getXStats(f5x_u,f5x_mu_actual,f5x_sigma_actual,1,3.);
  results_out << "f5x_mu " << f5x_mu_actual << endl;
  results_out << "f5x_sigma " << f5x_sigma_actual << endl;

  n0 = f5x_u->GetEntries();
  cout << "n0: " << n0 << " events" <<endl;//n0 is the number of upsteam 
  fout << "n0: " << n0 << " events" <<endl;
  results_out << "n0 " << n0 << endl; 

  //////////////////////////////////////////
  cout << "Downstream calculation" << endl; 
  //////////////////////////////////////////
  tree->Draw(Form("Zet7wob_2>>Zet7_pid35(400,%f,%f)",Z7_min,Z7_max),CU&&Em_all&&PID35,"",Neve);
  double n1_pre;//
  int binMax = Zet7_pid35->GetMaximumBin();
  double Zet7_mu = Zet7_pid35->GetXaxis()->GetBinCenter(binMax);
  Zet7_mu_exp=Zet7_mu;
  dZet7_exp = Zet7_mu_exp -Z0;
  Zet7_sigma_exp = Zet7_sigma;//fixed value of inputfile
  cout << "Fit of All Zet7 events" << endl;
  double dZet7_2_exp;
  FitTwoGaus(Zet7_pid35, Z0, dZet7_exp,Zet7_sigma_exp, dZet7_2_exp, 1, n1_pre, fout);
  
  cout << "n1_pre with F7IC: " <<n1_pre << endl;
  //	tree->Draw(Form("Zet7wob_2:AoQ57_5>>pid57(400,%f,%f,400,%f,%f)",AoQ_min,AoQ_max,Z7_min,Z7_max),CU,"colz",Neve);
  cout << "Count of Zet7 events with F7PLA" << endl;  
  tree->Draw(Form("Zet7wob_2:AoQ57_5>>pid57_u(400,%f,%f,400,%f,%f)",AoQ_min,AoQ_max,Z7_min,Z7_max),CU&&Em_all&&PID35&&Case1,"colz",Neve);
  
  int n_F7PLA = pid57_u->GetEntries();
  AoQ57_mu_exp = AoQ0+dAoQ57;
  AoQ57_sigma_exp = AoQ57_sigma;
  tree->Draw(Form("AoQ57_5>>aoq57_u(400,%f,%f)",AoQ_min,AoQ_max),CU&&Em_all&&PID35&&Case1&&Zet7,"colz",Neve);
  getXStats(aoq57_u,AoQ57_mu_exp,AoQ57_sigma_exp,2,3.);
  if(fabs(AoQ57_mu_exp-AoQ0)<0.03){ //abs() cannot be operated correctly. fabs() is used. 
    dAoQ57_exp = AoQ57_mu_exp-AoQ0;
  }else{
    dAoQ57_exp=0.;
    cout << "CAUTION! abs(dAoQ57)>0.03 " << endl;
    fout << "CAUTION! abs(dAoQ57)>0.03 " << endl;
  }
  if(AoQ57_sigma_exp>0.01||AoQ57_sigma_exp<0.001){
      AoQ57_sigma_exp= 0.004;
    cout << "CAUTION! !(0.001<AoQ57<0.01) " << endl;
    fout << "CAUTION! !(0.001<AoQ57<0.01) " << endl;
    }
  AddLine(pid57_u,(AoQ0+dAoQ57)-(n_sigma_AoQ57*AoQ57_sigma), Z7_min,(AoQ0+dAoQ57)-(n_sigma_AoQ57*AoQ57_sigma), Z7_max);
  AddLine(pid57_u,(AoQ0+dAoQ57)+(n_sigma_AoQ57*AoQ57_sigma), Z7_min,(AoQ0+dAoQ57)+(n_sigma_AoQ57*AoQ57_sigma), Z7_max);
  cout << "dAoQ57_exp, AoQ57_sigma_exp = " << dAoQ57_exp << ", " <<  AoQ57_sigma_exp << endl;
  fout << "dAoQ57_exp, AoQ57_sigma_exp = " << dAoQ57_exp << ", " <<  AoQ57_sigma_exp << endl;

  if(Flag_All)tree->Draw(Form("Zet7wob_2:AoQ57_5>>pid57_d(400,%f,%f,400,%f,%f)",AoQ_min,AoQ_max,Z7_min,Z7_max),CU&&Em_all&&PID35&&(Case1&&gCase1),"colz",Neve);
  if(Flag_All)AddLine(pid57_d,(AoQ0+dAoQ57)-(n_sigma_AoQ57*AoQ57_sigma), Z7_min,(AoQ0+dAoQ57)-(n_sigma_AoQ57*AoQ57_sigma), Z7_max);
  if(Flag_All)AddLine(pid57_d,(AoQ0+dAoQ57)+(n_sigma_AoQ57*AoQ57_sigma), Z7_min,(AoQ0+dAoQ57)+(n_sigma_AoQ57*AoQ57_sigma), Z7_max);

  cout << "Count of Zet7 events without F7PLA" << endl;  
  tree->Draw(Form("Zet7wob_2:AoQ57_7>>pid57woPLA_u(400,%f,%f,400,%f,%f)",AoQ_min,AoQ_max,Z7_min,Z7_max),CU&&Em_all&&PID35,"colz",Neve);
  AoQ57woPLA_mu_exp = AoQ0+dAoQ57woPLA;
  AoQ57woPLA_sigma_exp = AoQ57woPLA_sigma;
  tree->Draw(Form("AoQ57_7>>aoq57woPLA_u(400,%f,%f)",AoQ_min,AoQ_max),CU&&Em_all&&PID35&&Zet7,"colz",Neve);//
  getXStats(aoq57woPLA_u,AoQ57woPLA_mu_exp,AoQ57woPLA_sigma_exp,1,3.);
  dAoQ57woPLA_exp = AoQ57woPLA_mu_exp-AoQ0;
  if(AoQ57woPLA_sigma_exp>0.01){
    AoQ57woPLA_sigma_exp=0.005;
    cout << "CAUTION! AoQ57woPLA<0.01 " << endl;
    fout << "CAUTION! AoQ57woPLA<0.01 " << endl;
  }
  AddLine(pid57woPLA_u,(AoQ0+dAoQ57woPLA)-n_sigma_AoQ57woPLA*AoQ57woPLA_sigma, Z7_min,(AoQ0+dAoQ57woPLA)-n_sigma_AoQ57woPLA*AoQ57woPLA_sigma, Z7_max);
  AddLine(pid57woPLA_u,(AoQ0+dAoQ57woPLA)+n_sigma_AoQ57woPLA*AoQ57woPLA_sigma, Z7_min,(AoQ0+dAoQ57woPLA)+n_sigma_AoQ57woPLA*AoQ57woPLA_sigma, Z7_max);
  cout << "dAoQ57woPLA_exp, AoQ57woPLA_sigma_exp = " << dAoQ57woPLA_exp << ", " <<  AoQ57woPLA_sigma_exp << endl;
  fout << "dAoQ57woPLA_exp, AoQ57woPLA_sigma_exp = " << dAoQ57woPLA_exp << ", " <<  AoQ57woPLA_sigma_exp << endl;

  tree->Draw(Form("Zet7wob_2:AoQ57_7>>pid57woPLA_u_noBeta(400,%f,%f,400,%f,%f)",AoQ_min,AoQ_max,Z7_min,Z7_max),CU&&Em_all&&PID35&&Case2,"colz",Neve);
  int n_noF7PLA = pid57woPLA_u_noBeta->GetEntries();
  AddLine(pid57woPLA_u_noBeta,(AoQ0+dAoQ57woPLA)-n_sigma_AoQ57woPLA*AoQ57woPLA_sigma, Z7_min,(AoQ0+dAoQ57woPLA)-n_sigma_AoQ57woPLA*AoQ57woPLA_sigma, Z7_max);
  AddLine(pid57woPLA_u_noBeta,(AoQ0+dAoQ57woPLA)+n_sigma_AoQ57woPLA*AoQ57woPLA_sigma, Z7_min,(AoQ0+dAoQ57woPLA)+n_sigma_AoQ57woPLA*AoQ57woPLA_sigma, Z7_max);
  
if(Flag_All)tree->Draw(Form("Zet7wob_2:AoQ57_7>>pid57woPLA_d_noBeta(400,%f,%f,400,%f,%f)",AoQ_min,AoQ_max,Z7_min,Z7_max),CU&&Em_all&&PID35&&(Case2&&gCase2),"colz",Neve);
if(Flag_All)AddLine(pid57woPLA_d_noBeta,(AoQ0+dAoQ57woPLA)-n_sigma_AoQ57woPLA*AoQ57woPLA_sigma, Z7_min,(AoQ0+dAoQ57woPLA)-n_sigma_AoQ57woPLA*AoQ57woPLA_sigma, Z7_max);
if(Flag_All)AddLine(pid57woPLA_d_noBeta,(AoQ0+dAoQ57woPLA)+n_sigma_AoQ57woPLA*AoQ57woPLA_sigma, Z7_min,(AoQ0+dAoQ57woPLA)+n_sigma_AoQ57woPLA*AoQ57woPLA_sigma, Z7_max);
  
  tree->Draw(Form("AoQ57_7>>aoq57woPLA_u_noBeta_Zet7(400,%f,%f)",AoQ_min,AoQ_max),CU&&Em_all&&PID35&&(Case2&&Zet7),"colz",Neve);
  int n_noF7PLA_gZet7 = aoq57woPLA_u_noBeta_Zet7->GetEntries();
  tree->Draw(Form("AoQ57_7>>aoq57woPLA_d_noBeta_Zet7(400,%f,%f)",AoQ_min,AoQ_max),CU&&Em_all&&PID35&&(Case2&&Zet7)&&gCase2,"colz",Neve);
  int n_noF7PLA_gZet7_g = aoq57woPLA_d_noBeta_Zet7->GetEntries();

  cout << "Display of Zet7 events without F7PLA without F7PPAC" << endl;
  tree->Draw(Form("Zet7wob_2>>Zet7_noPPAC(400,%f,%f)",Z7_min,Z7_max),CU&&Em_all&&PID35&&Case3,"",Neve);
  int n_noF7PLAnoF7PPAC = Zet7_noPPAC->GetEntries();
  tree->Draw(Form("Zet7wob_2>>Zet7_noPPAC_gZet7(400,%f,%f)",Z7_min,Z7_max),CU&&Em_all&&PID35&&Case3&&Zet7,"",Neve);
  int n_noF7PLAnoF7PPAC_gZet7 = Zet7_noPPAC_gZet7->GetEntries();

  cout << "n0:n_F7PLA:n_noF7PLA&F7PPAC:n_noF7PA&noF7PPAC = " << n0 <<": " << n_F7PLA <<": " << n_noF7PLA<<": " << n_noF7PLAnoF7PPAC<< endl;
  fout << "n0:n_F7PLA:n_noF7PLA&F7PPAC:n_noF7PA&noF7PPAC = " << n0 <<": " << n_F7PLA <<": " << n_noF7PLA<<": " << n_noF7PLAnoF7PPAC<< endl;
  //  tree->Draw(Form("Zet7wob_2>>Zet7_d_all(400,%f,%f)",Z7_min,Z7_max),CU&&Em_all&&PID35&&(Case1&&gCase1||Case2&&gCase2||Case3),"",Neve);
  tree->Draw(Form("Zet7wob_2>>Zet7_d_all(400,%f,%f)",Z7_min,Z7_max),CU&&Em_all&&PID35&&(Case1&&gCase1||Case2&&gCase2),"",Neve);
  
  FitTwoGaus(Zet7_d_all, Z0, dZet7_exp,Zet7_sigma_exp, dZet7_2_exp, 2, n1, fout);
  double n1_cor = n1;
  if(n_noF7PLA_gZet7!=0){
    n1_cor = n1 + (double)n_noF7PLAnoF7PPAC_gZet7*((double)n_noF7PLA_gZet7_g/(double)n_noF7PLA_gZet7);
  }
  cout << "n1 ="  << n1 <<" + " << n_noF7PLAnoF7PPAC_gZet7 <<"*("<<n_noF7PLA_gZet7_g <<"/"<<n_noF7PLA_gZet7<<") = "<< n1_cor << endl;
  fout << "n1 ="  << n1 <<" + " << n_noF7PLAnoF7PPAC_gZet7 <<"*("<<n_noF7PLA_gZet7_g <<"/"<<n_noF7PLA_gZet7<<") = "<< n1_cor << endl;
  results_out << "n1 " << n1_cor << endl;
  
  tree->Draw("F7X_2>>f7x_u(240,-120,120)",CU&&Em_all&&PID35,"colz",Neve);	
  double f7x_u_mu_exp;
  double f7x_u_sigma_exp;
  getXStats(f7x_u,f7x_u_mu_exp,f7x_u_sigma_exp,1,3.);
  results_out << "f7x_mu " << f7x_u_mu_exp << endl;
  results_out << "f7x_sigma " << f7x_u_sigma_exp << endl;

  results_out << "n_F7PLA " << n_F7PLA << endl;
  results_out << "n_noF7PLA "  << n_noF7PLA << endl;
  results_out << "n_noF7PAnoF7PPAC " << n_noF7PLAnoF7PPAC << endl;
  
 //		/*		
 if(Flag_Emittance){
   cout << "Emittance calculation" << endl; 

   // F3XA Emittance
   tree->Draw("Fpl_XT[3]:Fpl_XP[3]>>f3xa(60,-30.,30.,60,-30,30)",CU&&F3Y&&F3B&&F5X&&PID35,"colz",Neve);
   tree->Draw("Fpl_XT[3]:Fpl_XP[3]>>f3xa_g(60,-30.,30.,60,-30.,30)",CU&&F3Y&&F3B&&F5X&&PID35&&((Case1&&gCase1)||(Case2&&gCase2)||Case3)&&Zet7,"colz",Neve);
   TH2F * f3xa_t=  f3xa ->Clone("f3xa_t");
   TH1F * f3x_t2=  f3xa ->ProjectionX()->Clone("f3x_t2");
   TH1F * f3a_t2=  f3xa ->ProjectionY()->Clone("f3a_t2");
   transmission2d(f3xa,f3xa_g,f3xa_t,f3x_t2,f3a_t2,f3x_left,f3x_right,f3a_left,f3a_right,target);

   // F3YB Emittance
   tree->Draw("Fpl_YT[3]:Fpl_YP[3]>>f3yb(160,-20.,20.,80,-40,40)",CU&&F3X&&F3A&&F5X&&F3XA&&PID35,"colz",Neve);
   tree->Draw("Fpl_YT[3]:Fpl_YP[3]>>f3yb_g(160,-20.,20.,80,-40.,40)",CU&&F3X&&F3A&&F5X&&F3XA&&PID35&&((Case1&&gCase1)||(Case2&&gCase2)||Case3)&&Zet7,"colz",Neve);
   TH2F * f3yb_t=  f3yb ->Clone("f3yb_t");
   TH1F * f3y_t2=  f3yb ->ProjectionX()->Clone("f3y_t2");
   TH1F * f3b_t2=  f3yb ->ProjectionY()->Clone("f3b_t2");
   transmission2d(f3yb,f3yb_g,f3yb_t,f3y_t2,f3b_t2,f3y_left,f3y_right,f3b_left,f3b_right,target);

   // F5XF3A Emittance
   tree->Draw("Fpl_XT[3]:F5X_2>>f5xf3a(240,-120.,120.,60,-30,30)",CU&&F3X&&F3Y&&F3B&&F3XA&&PID35,"colz",Neve);
   tree->Draw("Fpl_XT[3]:F5X_2>>f5xf3a_g(240,-120.,120.,60,-30.,30)",CU&&F3X&&F3Y&&F3B&&F3XA&&PID35&&((Case1&&gCase1)||(Case2&&gCase2)||Case3)&&Zet7,"colz",Neve);
   TH2F * f5xf3a_t=  f5xf3a ->Clone("f5xf3a_t");
   TH1F * f5x_t2=  f5xf3a ->ProjectionX()->Clone("f5x_t2");
   TH1F * f3a_t3=  f5xf3a ->ProjectionY()->Clone("f3a_t3");
   transmission2d(f5xf3a,f5xf3a_g,f5xf3a_t,f5x_t2,f3a_t3,f5x_left,f5x_right,f3a_left,f3a_right,target);
   
   // PID35 Emittance
   tree->Draw(Form("(((Zet3_3-(%f+%f))/%f)**2+((AoQ35_5-(%f+%f))/%f)**2)**(0.5)>>pid35r(20,0,5.)",Z0,dZet3,Zet3_sigma,AoQ0,dAoQ35,AoQ35_sigma),CU&&Em_all,"colz",Neve);
   tree->Draw(Form("(((Zet3_3-(%f+%f))/%f)**2+((AoQ35_5-(%f+%f))/%f)**2)**(0.5)>>pid35r_g(20,0,5.)",Z0,dZet3,Zet3_sigma,AoQ0,dAoQ35,AoQ35_sigma),CU&&Em_all&&((Case1&&gCase1)||(Case2&&gCase2)||Case3)&&Zet7,"colz",Neve);
   TH1F * pid35r_t=  pid35r ->Clone("pid35r_t");
   transmission1d(pid35r,pid35r_g,pid35r_t,0.,n_sigma_Zet3,target);

   //Beta35Delta35
   tree->Draw(Form("Delta35_2:Beta35_2>>beta35delta35(800,%f,%f,120,%f,%f)",Beta_min,Beta_max,Delta_min,Delta_max),CU&&Em_all&&PID35,"colz",Neve);
   tree->Draw(Form("Delta35_2:Beta35_2>>beta35delta35_g(800,%f,%f,120,%f,%f)",Beta_min,Beta_max,Delta_min,Delta_max),CU&&Em_all&&PID35&&((Case1&&gCase1)||(Case2&&gCase2)||Case3)&&Zet7,"colz",Neve);
   TH2F * beta35delta35_t =  beta35delta35 ->Clone("beta35delta35_t");
   TH1F * beta35_t2 =  beta35delta35 ->ProjectionX()->Clone("beta35_t2");
   TH1F * delta35_t2 =  beta35delta35 ->ProjectionY()->Clone("delta35_t2");
   double Beta_Nmax = beta35_t2->GetXaxis()->GetBinCenter(beta35_t2->GetMaximumBin());
   Beta_min = Beta_Nmax *0.97;
   Beta_max = Beta_Nmax *1.03;

   transmission2d(beta35delta35,beta35delta35_g,beta35delta35_t,beta35_t2,delta35_t2,Beta_min,Beta_max,Delta_min,Delta_max,target);
   beta35delta35_t->SetMinimum(0.5);
   beta35_t2->SetMinimum(0.5);
   delta35_t2->SetMinimum(0.5);

   beta35delta35_px->GetXaxis()->SetRangeUser(Beta_min,Beta_max);
   beta35_t2->GetXaxis()->SetRangeUser(Beta_min,Beta_max);

   beta35delta35->GetXaxis()->SetRangeUser(Beta_min,Beta_max);
   beta35delta35_g->GetXaxis()->SetRangeUser(Beta_min,Beta_max);
   beta35delta35_t->GetXaxis()->SetRangeUser(Beta_min,Beta_max);

   // 2-d PID35  Emittance
   tree->Draw(Form("Zet3_3:AoQ35_5>>pid35_2d(40,%f,%f,20,%f,%f)",AoQ0+dAoQ35-10.*AoQ35_sigma,AoQ0+dAoQ35+10.*AoQ35_sigma,Z0+dZet3-10.*Zet3_sigma,Z0+dZet3+10.*Zet3_sigma),CU&&Em_all,"colz",Neve);
   tree->Draw(Form("Zet3_3:AoQ35_5>>pid35_2d_g(40,%f,%f,20,%f,%f)",AoQ0+dAoQ35-10.*AoQ35_sigma,AoQ0+dAoQ35+10.*AoQ35_sigma,Z0+dZet3-10.*Zet3_sigma,Z0+dZet3+10.*Zet3_sigma),CU&&Em_all&&((Case1&&gCase1)||(Case2&&gCase2)||Case3)&&Zet7,"colz",Neve);
   TH2F * pid35_2d_t= pid35_2d ->Clone("pid35_2d_t");
   TH1F * aoq35_t2=  pid35_2d ->ProjectionX()->Clone("aoq35_t2");
   TH1F * zet3_t2=  pid35_2d ->ProjectionY()->Clone("zet3_t2");
   transmission2d(pid35_2d,pid35_2d_g,pid35_2d_t,aoq35_t2,zet3_t2,AoQ0+dAoQ35-n_sigma_AoQ35*AoQ35_sigma,AoQ0+dAoQ35+n_sigma_AoQ35*AoQ35_sigma,Z0+dZet3-n_sigma_Zet3*Zet3_sigma,Z0+dZet3+n_sigma_Zet3*Zet3_sigma,target);
   TEllipse *ellipse = new TEllipse(AoQ0+dAoQ35,Z0+dZet3, n_sigma_AoQ35*AoQ35_sigma, n_sigma_Zet3*Zet3_sigma);
   ellipse->SetFillStyle(0); // No filling
   ellipse->SetLineColor(kBlack); // Set line color to black
   ellipse->SetLineWidth(2); // Set line width
   pid35_2d->GetListOfFunctions()->Add(ellipse);
   pid35_2d_g->GetListOfFunctions()->Add(ellipse);
   pid35_2d_t->GetListOfFunctions()->Add(ellipse);
   pid35_2d_t->SetMinimum(0.5);
   aoq35_t2->SetMinimum(0.5);
   zet3_t2->SetMinimum(0.5);
 }//end of Emittance

  /////////////////////////////
  /////////////////////////////
  //Section 5 Writing hbk and parameter files 
  /////////////////////////////
  /////////////////////////////
 size_t pos = input_filename.find_last_of('.');
 string output_filename = input_filename.substr(0, pos) + "_rev" + input_filename.substr(pos);
 if(Flag_Fix_up_para){
   write_parameters(output_filename, R_n, Flag_All,Flag_Emittance, Flag_Fix_up_para, A0, Z0, dZet3, Zet3_sigma, dAoQ35, AoQ35_sigma, dZet7_exp, Zet7_sigma_exp, dAoQ57_exp, AoQ57_sigma_exp, dAoQ57woPLA_exp, AoQ57woPLA_sigma_exp, f3x_mu, f3x_sigma, f3a_mu, f3a_sigma, f3y_mu, f3y_sigma, f3b_mu, f3b_sigma, f5x_left, f5x_right,F7X_PLATdiff_offset_exp,F7X_PLATdiff_slope_exp,D57_AoQ57woPLA_slope_exp,AoQ35_offset,AoQ35_slope,Zet3_offset,Zet3_slope,AoQ57_offset,AoQ57_slope,Zet7_offset,Zet7_slope );
 }else{ write_parameters(output_filename, R_n, Flag_All,Flag_Emittance, Flag_Fix_up_para,A0, Z0, dZet3_exp, Zet3_sigma_exp, dAoQ35_exp, AoQ35_sigma_exp, dZet7_exp, Zet7_sigma_exp, dAoQ57_exp, AoQ57_sigma_exp,dAoQ57woPLA_exp, AoQ57woPLA_sigma_exp, f3x_mu_exp, f3x_sigma_exp, f3a_mu_exp, f3a_sigma_exp, f3y_mu_exp, f3y_sigma_exp, f3b_mu_exp, f3b_sigma_exp, f5x_left_exp, f5x_right_exp,F7X_PLATdiff_offset_exp,F7X_PLATdiff_slope_exp,D57_AoQ57woPLA_slope_exp,AoQ35_offset,AoQ35_slope,Zet3_offset,Zet3_slope,AoQ57_offset,AoQ57_slope,Zet7_offset,Zet7_slope );
	}
#ifdef HBK_WRITE
 hstore(Form("hbk/%s.hbk",name.c_str()));
#endif
 return;
}

// Subroutine for Binominal Error
double BinomialError(double p, double N) {
  if (N == 0) return 0;
  return TMath::Sqrt(p * (1 - p) / N);
}

// Subroutine for transmission calculation using histogram divide
void transmission1d(TH1F* in, TH1F* out, TH1F*& trans, double left, double right, string target){
  trans->Reset();
  for (int i = 1; i <= in->GetNbinsX(); ++i) {
    double binContent1 = in->GetBinContent(i);
    double binContent2 = out->GetBinContent(i);
    
    if (binContent2 != 0) {
      double ratio = binContent2 / binContent1;
      double error = BinomialError(ratio, binContent1);

      trans->SetBinContent(i, ratio);
      trans->SetBinError(i, error);
    } else {
            trans->SetBinContent(i, 0);
            trans->SetBinError(i, 0);
	  }
  }
  TMarker *marker_right = new TMarker(left, 1.000, kPlus);
  marker_right->SetMarkerColor(kRed);
  marker_right->SetMarkerSize(3.0);
  TMarker *marker_left = new TMarker(right, 1.000, kPlus);
  marker_left->SetMarkerColor(kRed);
  marker_left->SetMarkerSize(3.0);
  TLine *line = new TLine(left, 1.000, right, 1.000);
  line->SetLineColor(kRed);
  line->SetLineWidth(3.);

  in->GetListOfFunctions()->Add(marker_right);
  in->GetListOfFunctions()->Add(marker_left);
  in->GetListOfFunctions()->Add(line);
  out->GetListOfFunctions()->Add(marker_right);
  out->GetListOfFunctions()->Add(marker_left);
  out->GetListOfFunctions()->Add(line);
  trans->GetListOfFunctions()->Add(marker_right);
  trans->GetListOfFunctions()->Add(marker_left);
  trans->GetListOfFunctions()->Add(line);

  // Set Y axis range
   if(target=="C"||target=="CH2"){
  trans->SetMinimum(0.8); // Set the minimum value of the Y axis
  trans->SetMaximum(1.0); // Set the maximum value of the Y axis
   }else{
  trans->SetMinimum(0.95); // Set the minimum value of the Y axis
  trans->SetMaximum(1.0); // Set the maximum value of the Y axis
   }
}

// Subroutine for transmission calculation using 2D histogram divide
void transmission2d(TH2F* in, TH2F* out, TH2F*& trans, TH1F*& trans_x,TH1F*& trans_y,double x_left,double x_right,double y_down,double y_up, string target) {
  trans->Reset();
  for (int i = 1; i <= in->GetNbinsX(); ++i) {
    for (int j = 1; j <= in->GetNbinsY(); ++j) {
      double binContent1 = in->GetBinContent(i, j);
      double binContent2 = out->GetBinContent(i, j);

      if (binContent2 != 0) {
        double ratio = binContent2 / binContent1;
        double error = BinomialError(ratio, binContent1);

        trans->SetBinContent(i, j, ratio);
        trans->SetBinError(i, j, error);
      } else {
        trans->SetBinContent(i, j, 0);
        trans->SetBinError(i, j, 0);
      }
    }
  }   
   TLine *line1 = new TLine(x_left,y_down, x_right, y_down);
   line1->SetLineColor(kBlack);
   line1->SetLineWidth(2.);
   TLine *line2 = new TLine(x_right,y_down, x_right,y_up);
   line2->SetLineColor(kBlack);
   line2->SetLineWidth(2.);
   TLine *line3 = new TLine(x_right,y_up, x_left, y_up);
   line3->SetLineColor(kBlack);
   line3->SetLineWidth(2.);
   TLine *line4 = new TLine(x_left,y_up, x_left, y_down);
   line4->SetLineColor(kBlack);
   line4->SetLineWidth(2.);
   in->GetListOfFunctions()->Add(line1);
   in->GetListOfFunctions()->Add(line2);
   in->GetListOfFunctions()->Add(line3);
   in->GetListOfFunctions()->Add(line4);
   out->GetListOfFunctions()->Add(line1);
   out->GetListOfFunctions()->Add(line2);
   out->GetListOfFunctions()->Add(line3);
   out->GetListOfFunctions()->Add(line4);
   trans->GetListOfFunctions()->Add(line1);
   trans->GetListOfFunctions()->Add(line2);
   trans->GetListOfFunctions()->Add(line3);
   trans->GetListOfFunctions()->Add(line4);

   // Project in and out onto X axis in the range [y_down, y_up]
    in_x = in->ProjectionX("in_x", trans->GetYaxis()->FindBin(y_down), trans->GetYaxis()->FindBin(y_up));
    out_x = out->ProjectionX("out_x", trans->GetYaxis()->FindBin(y_down), trans->GetYaxis()->FindBin(y_up));
    //    trans_x = (TH1F*)out_x->Clone("trans_x");
    //   trans_x->Reset();

   for (int i = 1; i <= in_x->GetNbinsX(); ++i) {
     double binContent1 = in_x->GetBinContent(i);
     double binContent2 = out_x->GetBinContent(i);

     if (binContent2 != 0) {
       double ratio = binContent2 / binContent1;
       double error = BinomialError(ratio, binContent1);

       trans_x->SetBinContent(i, ratio);
       trans_x->SetBinError(i, error);
     } else {
       trans_x->SetBinContent(i, 0);
       trans_x->SetBinError(i, 0);
     }
   }

   // Project in and out onto Y axis in the range [x_left, x_right]
    in_y = in->ProjectionY("in_y", trans->GetXaxis()->FindBin(x_left), trans->GetXaxis()->FindBin(x_right));
    out_y = out->ProjectionY("out_y", trans->GetXaxis()->FindBin(x_left), trans->GetXaxis()->FindBin(x_right));
    //   trans_y = (TH1F*)out_y->Clone("trans_y");
    //   trans_y->Reset();

   for (int i = 1; i <= in_y->GetNbinsX(); ++i) {
     double binContent1 = in_y->GetBinContent(i);
     double binContent2 = out_y->GetBinContent(i);

     if (binContent2 != 0) {
       double ratio = binContent2 / binContent1;
       double error = BinomialError(ratio, binContent1);

       trans_y->SetBinContent(i, ratio);
       trans_y->SetBinError(i, error);
     } else {
       trans_y->SetBinContent(i, 0);
       trans_y->SetBinError(i, 0);
     }
   }

  TMarker *marker_left = new TMarker(x_left, 1.000, kPlus);
  marker_left->SetMarkerColor(kRed);
  marker_left->SetMarkerSize(3.0);
  TMarker *marker_right = new TMarker(x_right, 1.000, kPlus);
  marker_right->SetMarkerColor(kRed);
  marker_right->SetMarkerSize(3.0);

  TLine *line = new TLine(x_left, 1.000, x_right, 1.000);
  line->SetLineColor(kRed);
  line->SetLineWidth(3.);

  trans_x->GetListOfFunctions()->Add(marker_left);
  trans_x->GetListOfFunctions()->Add(marker_right);
  trans_x->GetListOfFunctions()->Add(line);

  TMarker *marker_down = new TMarker(y_down, 1.000, kPlus);
  marker_down->SetMarkerColor(kRed);
  marker_down->SetMarkerSize(3.0);
  TMarker *marker_up = new TMarker(y_up, 1.000, kPlus);
  marker_up->SetMarkerColor(kRed);
  marker_up->SetMarkerSize(3.0);

  TLine *line = new TLine(y_down, 1.000, y_up, 1.000);
  line->SetLineColor(kRed);
  line->SetLineWidth(3.);

  trans_y->GetListOfFunctions()->Add(marker_down);
  trans_y->GetListOfFunctions()->Add(marker_up);
  trans_y->GetListOfFunctions()->Add(line);


   
  // Set Z axis range
   if(target=="C"||target=="CH2"){
  trans->SetMinimum(0.8); // Set the minimum value of the Z axis
  trans->SetMaximum(1.0); // Set the maximum value of the Z axis
  trans_x->SetMinimum(0.8); // Set the minimum value of the Z axis
  trans_x->SetMaximum(1.0); // Set the maximum value of the Z axis
  trans_y->SetMinimum(0.8); // Set the minimum value of the Z axis
  trans_y->SetMaximum(1.0); // Set the maximum value of the Z axis
   }else{
  trans->SetMinimum(0.95); // Set the minimum value of the Z axis
  trans->SetMaximum(1.0); // Set the maximum value of the Z axis
  trans_x->SetMinimum(0.95); // Set the minimum value of the Z axis
  trans_x->SetMaximum(1.0); // Set the maximum value of the Z axis
  trans_y->SetMinimum(0.95); // Set the minimum value of the Z axis
  trans_y->SetMaximum(1.0); // Set the maximum value of the Z axis
   }
}
// Subroutine for mean and sigma deduction of 2-d plot
void getXYStats(TH2F* hist, double& mean_x, double& std_dev_x, double& mean_y, double& std_dev_y, int mode, double n_sigma) {
  TH1D* projX_pre = hist->ProjectionX();
  TH1D* projY_pre = hist->ProjectionY();

  double mean_x_pre;
  double std_dev_x_pre;
  double mean_y_pre;
  double std_dev_y_pre;
  //initial values are used from actural data in mode1
  //initial values are used from initial setting values in mode2
  if(mode==1){
    mean_x_pre = projX_pre->GetMean();
    std_dev_x_pre = projX_pre->GetStdDev();
    mean_y_pre = projY_pre->GetMean();
    std_dev_y_pre = projY_pre->GetStdDev();
  }else{
    mean_x_pre = mean_x; //changed on June 15th 2024 for trusting initial calibration
    std_dev_x_pre = std_dev_x;
    mean_y_pre = mean_y;
    std_dev_y_pre = std_dev_y;
  }

  //  double n_sigma = 5.0;
  //  double n_sigma = 2.0;// 3sigma is changed on June 13th 2024
  TH1D* projX = hist->ProjectionX();
  TH1D* projY = hist->ProjectionY();
  for(int i=0;i<5;i++){// from 2times to 5times on June 13th 2024
  projX->GetXaxis()->SetRangeUser(mean_x_pre - n_sigma*std_dev_x_pre, mean_x_pre + n_sigma*std_dev_x_pre);
  projY->GetXaxis()->SetRangeUser(mean_y_pre - n_sigma*std_dev_y_pre, mean_y_pre + n_sigma*std_dev_y_pre);
  mean_x_pre = projX->GetMean();
  std_dev_x_pre = projX->GetStdDev();
  mean_y_pre = projY->GetMean();
  std_dev_y_pre = projY->GetStdDev();
  projX->GetXaxis()->UnZoom();
  projY->GetXaxis()->UnZoom();
  }  
  //  n_sigma = 3.0;
  projX->GetXaxis()->SetRangeUser(mean_x_pre - n_sigma*std_dev_x_pre, mean_x_pre + n_sigma*std_dev_x_pre);
  projY->GetXaxis()->SetRangeUser(mean_y_pre - n_sigma*std_dev_y_pre, mean_y_pre + n_sigma*std_dev_y_pre);
  double mean_x = projX->GetMean();
  double std_dev_x = projX->GetStdDev();
  double mean_y = projY->GetMean();
  double std_dev_y = projY->GetStdDev();
}

// Subroutine for mean and sigma deduction of 1-d plot
void getXStats(TH1F* hist, double& mean_x, double& std_dev_x, int mode, double n_sigma) {

  double mean_x_pre;
  double std_dev_x_pre;
  if(mode==1){
    mean_x_pre = hist->GetMean();
    std_dev_x_pre = hist->GetStdDev();
  }else{
    mean_x_pre = mean_x;
    std_dev_x_pre = std_dev_x;
  }
  //  double n_sigma = 3.0;
  for(int i=0;i<5;i++){// itteration
  hist->GetXaxis()->SetRangeUser(mean_x_pre - n_sigma*std_dev_x_pre, mean_x_pre + n_sigma*std_dev_x_pre);
  mean_x_pre = hist->GetMean();
  std_dev_x_pre = hist->GetStdDev();
  hist->GetXaxis()->UnZoom();
  }  
  hist->GetXaxis()->SetRangeUser(mean_x_pre - n_sigma*std_dev_x_pre, mean_x_pre + n_sigma*std_dev_x_pre);
  double mean_x = hist->GetMean();
  double std_dev_x = hist->GetStdDev();
  mean_x = hist->GetMean();
  std_dev_x = hist->GetStdDev();
}

void FitTwoGaus(TH1F* hist, double& mean_z, double& dz,double& sigma_z, double& dz2, int mode,  double& nfit, ofstream& fout){
  double M_PI = 3.1415926;
  double mean_z_cor = mean_z + dz;
  double max_value = hist->GetMaximum();

  // Check if histogram is empty
  if (max_value == 0) {
    cout << "Error: Histogram is empty" << endl;
    return;
  }
  //
  TF1 *fitFunc = new TF1("fitFunc", "[0]*exp(-0.5*((x-[1])/[2])^2) + [3]*exp(-0.5*((x-[4])/[2])^2)", 0, 100);//the same sigma parameter is used for two Gaussian
  fitFunc->SetParameters(max_value, mean_z_cor, sigma_z, max_value*0.001, mean_z_cor-1.); // Initial parameters
  fitFunc->SetParLimits(0, 1., 100000000.);
  fitFunc->SetParLimits(1, mean_z_cor-0.1, mean_z_cor+0.1);
  fitFunc->SetParLimits(2, 0.5*sigma_z, 2.*sigma_z);
  fitFunc->SetParLimits(3, 0.001, max_value*100.);
  if(mode==1){
  fitFunc->SetParLimits(4, mean_z_cor-1.2, mean_z_cor-0.8);
  }else{
    fitFunc->FixParameter(4,mean_z+dz2);
  }
  double bin_width = hist->GetBinWidth(1);;
  
  // Fit the histogram with the function
  hist->Fit(fitFunc,"","",mean_z_cor-1.5,mean_z_cor+1.);
  
  // Draw the histogram
  hist->Draw();
  
  // Draw the fit results
  fitFunc->SetLineColor(kRed);
  fitFunc->Draw("same");
  
  // Add a legend
  TLegend *legend = new TLegend(0.1, 0.7, 0.3, 0.9);
  legend->AddEntry(hist, "Data", "l");
  legend->AddEntry(fitFunc, "Fit (two Gaussians)", "l");
  legend->Draw();

  // Get fitting parameters
  double amp1 = fitFunc->GetParameter(0);
  double mean1 = fitFunc->GetParameter(1);
  double sigma1 = fitFunc->GetParameter(2);

  double amp2 = fitFunc->GetParameter(3);
  double mean2 = fitFunc->GetParameter(4);

  // Define and draw the individual Gaussian components
  TF1 *gaus1 = new TF1("gaus1", "[0]*exp(-0.5*((x-[1])/[2])^2)", 0, 100);
  gaus1->SetParameters(amp1, mean1, sigma1);
  gaus1->SetLineColor(kRed);
  gaus1->SetLineStyle(2); // Set line style to dashed

  TF1 *gaus2 = new TF1("gaus2", "[0]*exp(-0.5*((x-[1])/[2])^2)", 0, 100);
  gaus2->SetParameters(amp2, mean2, sigma1);
  gaus2->SetLineColor(kRed);
  gaus2->SetLineStyle(2); // Set line style to dashed

  // Add the Gaussian components to the histogram
  hist->GetListOfFunctions()->Add(gaus1);
  hist->GetListOfFunctions()->Add(gaus2);

  //  double int_range_left = mean_z_cor -0.8;
  double intersection = -0.5-sigma1**2.*(log(amp1)-log(amp2));// intersection point of two gaussian 
  cout << "intersectionpoint of two gaussian(-1~0):" <<  intersection << endl;
  double dz_min = intersection;
  double dz_max = 1.0;
  double int_range_left = mean_z_cor +dz_min ;
  double int_range_right = mean_z_cor + dz_max;

  TMarker *marker_right = new TMarker(int_range_left, amp1, 23);
  marker_right->SetMarkerColor(kRed);
  marker_right->SetMarkerSize(1.0);
  TMarker *marker_left = new TMarker(int_range_right, amp1, 23);
  marker_left->SetMarkerColor(kRed);
  marker_left->SetMarkerSize(1.0);
  TLine *line_left = new TLine(int_range_left, 0.5, int_range_left, amp1);
  TLine *line_right = new TLine(int_range_right, 0.5, int_range_right, amp1);
  line_left->SetLineColor(kBlack);
  line_left->SetLineWidth(1.5);
  line_right->SetLineColor(kBlack);
  line_right->SetLineWidth(1.5);

  hist->GetListOfFunctions()->Add(marker_left);
  hist->GetListOfFunctions()->Add(marker_right);
  hist->GetListOfFunctions()->Add(line_left);
  hist->GetListOfFunctions()->Add(line_right);

  // Calculate the areas under the Gaussian functions within the specified range
  int bin_left = hist->GetXaxis()->FindBin(int_range_left);
  int bin_right = hist->GetXaxis()->FindBin(int_range_right);

  double events_in_range = 0;
  for (int i = bin_left; i <= bin_right; ++i) {
    events_in_range += hist->GetBinContent(i);
  }


  // Calculate the Gaussian contributions outside the specified range
  double total_area1 = amp1 * sigma1 * sqrt(2. * M_PI) / bin_width;
  double total_area2 = amp2 * sigma1 * sqrt(2. * M_PI) / bin_width;

  double range_area1 = gaus_integral(amp1, mean1, sigma1, int_range_left, int_range_right) / bin_width;
  double range_area2 = gaus_integral(amp2, mean2, sigma1, int_range_left, int_range_right) / bin_width;

  nfit = events_in_range + (total_area1 - range_area1) + (- range_area2);

  // Print the areas
  cout << "Area within the first events: " << events_in_range << endl;
  cout << "Total area of the first Gaussian: " << total_area1 << endl;
  cout << "Total area of the second Gaussian: " << total_area2 << endl;
  cout << "Area within the range for the first Gaussian: " << range_area1 << endl;
  cout << "Area within the range for the second Gaussian: " << range_area2 << endl;
  cout << "Adding area of the outrange for the first Gaussian: " << total_area1-range_area1 << endl;
  cout << "Subtracting area within the range for the second Gaussian: " << range_area2 << endl;
  cout << "Number of events (nfit): " << nfit << endl;

  fout << "Area within the first events: " << events_in_range << endl;
  fout << "Total area of the first Gaussian: " << total_area1 << endl;
  fout << "Total area of the second Gaussian: " << total_area2 << endl;
  fout << "Area within the range for the first Gaussian: " << range_area1 << endl;
  fout << "Area within the range for the second Gaussian: " << range_area2 << endl;
  fout << "Adding area of the outrange for the first Gaussian: " << total_area1-range_area1 << endl;
  fout << "Subtracting area within the range for the second Gaussian: " << range_area2 << endl;
  fout << "Number of events (nfit): " << nfit << endl;


 // Set values
 dz = mean1 - mean_z; 
 sigma_z = sigma1;  
 dz2 = mean2 - mean_z; 
}

double gaus_integral(double amp, double mean, double sigma, double x_min, double x_max) {
  double M_PI = 3.1415926;
  return amp * sigma * sqrt(2 * M_PI) * (TMath::Erf((x_max - mean) / (sqrt(2.) * sigma)) - TMath::Erf((x_min - mean) / (sqrt(2.) * sigma))) / 2.;
}

void PrfXFitPol1(TH2F* hist, double& para_slope, double& para_offset, double& para_slope_exp, double& para_offset_exp, double& Xrange_min, double& Xrange_max, ofstream& fout) {
  // Create a profile from the histogram
  TProfile* prfX = hist->ProfileX();
    
  // Fit the profile with a first order polynomial
  prfX->Fit("pol1", "", "", Xrange_min, Xrange_max);
    
  // Get the fit function
  TF1* fitFunc = prfX->GetFunction("pol1");
    
  // Check if the fit function is valid (not nullptr)
  if (fitFunc) {
    double para_slope_cor = fitFunc->GetParameter(1);
    double para_offset_cor = fitFunc->GetParameter(0);
    cout << "F7X_cor" << para_slope_cor << ", " << para_offset_cor << endl;
        
    para_slope_exp = para_slope / para_slope_cor;
    para_offset_exp = para_offset - para_offset_cor / (para_slope / para_slope_cor);
        
    cout << "F7X_Tdiff_calib: " << para_slope_exp << " +/- " << fitFunc->GetParError(1) << ", " << para_offset_exp << " +/- " << fitFunc->GetParError(0) << endl;
    fout << "F7X_Tdiff_calib: " << para_slope_exp << " +/- " << fitFunc->GetParError(1) << ", " << para_offset_exp << " +/- " << fitFunc->GetParError(0) << endl;
  } else {
    para_slope_exp = para_slope;
    para_offset_exp = para_offset;
    // Handle the case where the fit function is not valid
    cout << "Error: Fit function is null. Fit might have failed due to empty data." << endl;
    fout << "Error: Fit function is null. Fit might have failed due to empty data." << endl;
  }
}

void AddLine(TH2F* hist, double x1, double y1, double x2, double y2) {
  // Calculate the x-coordinate for the vertical line
  // Create the vertical line
  TLine *line = new TLine(x1, y1, x2, y2);    
  // Set the line properties
  line->SetLineColor(kBlack);
  line->SetLineWidth(1.5);
  
  // Add the line to the pad
  hist->GetListOfFunctions()->Add(line);

}

void write_parameters(const string& filename, double R_n, int Flag_All, int Flag_Emittance, int Flag_Fix_up_para, int A0, int Z0, double dZet3, double Zet3_sigma, double dAoQ35, double AoQ35_sigma, double dZet7, double Zet7_sigma, double dAoQ57, double AoQ57_sigma, double dAoQ57woPLA, double AoQ57woPLA_sigma,double f3x_mu, double f3x_sigma, double f3a_mu, double f3a_sigma, double f3y_mu, double f3y_sigma, double f3b_mu, double f3b_sigma, double f5x_left, double f5x_right,double F7X_PLATdiff_offset, double F7X_PLATdiff_slope,double D57_AoQ57woPLA_slope, double AoQ35_offset,double AoQ35_slope,double Zet3_offset,double Zet3_slope,double AoQ57_offset,double AoQ57_slope,double Zet7_offset,double Zet7_slope ) {
  //string filename2 = "output.dat";
  //    ofstream outputFile(filename2);
  cout << filename << endl;
  string full_output_path = "./input/" + filename;
  ofstream outputFile(full_output_path.c_str());
    if (!outputFile.is_open()) {
        cerr << "Error opening output file: " << filename << endl;
        return;
    }

    outputFile << "R_n " << R_n << endl;
    outputFile << "Flag_All " << Flag_All << endl;
    outputFile << "Flag_Emittance " << Flag_Emittance << endl;
    outputFile << "Flag_Fix_up_para " << Flag_Fix_up_para << endl;
    outputFile << "A0 " << A0 << endl;
    outputFile << "Z0 " << Z0 << endl;
    outputFile << "dZet3 " << dZet3 << endl;
    outputFile << "Zet3_sigma " << Zet3_sigma << endl;
    outputFile << "dAoQ35 " << dAoQ35 << endl;
    outputFile << "AoQ35_sigma " << AoQ35_sigma << endl;
    outputFile << "dZet7 " << dZet7 << endl;
    outputFile << "Zet7_sigma " << Zet7_sigma << endl;
    outputFile << "dAoQ57 " << dAoQ57 << endl;
    outputFile << "AoQ57_sigma " << AoQ57_sigma << endl;
    outputFile << "dAoQ57woPLA " << dAoQ57woPLA << endl;
    outputFile << "AoQ57woPLA_sigma " << AoQ57woPLA_sigma << endl;
    outputFile << "f3x_mu " << f3x_mu << endl;
    outputFile << "f3x_sigma " << f3x_sigma << endl;
    outputFile << "f3a_mu " << f3a_mu << endl;
    outputFile << "f3a_sigma " << f3a_sigma << endl;
    outputFile << "f3y_mu " << f3y_mu << endl;
    outputFile << "f3y_sigma " << f3y_sigma << endl;
    outputFile << "f3b_mu " << f3b_mu << endl;
    outputFile << "f3b_sigma " << f3b_sigma << endl;
    outputFile << "f5x_left " << f5x_left << endl;
    outputFile << "f5x_right " << f5x_right << endl;
    outputFile <<"F7X_PLATdiff_offset " <<F7X_PLATdiff_offset  <<endl;
    outputFile <<"F7X_PLATdiff_slope "<<F7X_PLATdiff_slope  <<endl;
    outputFile <<"D57_AoQ57woPLA_slope "<<D57_AoQ57woPLA_slope  <<endl;
    outputFile <<"AoQ35_offset "<<AoQ35_offset  <<endl;
    outputFile <<"AoQ35_slope "<<AoQ35_slope  <<endl;
    outputFile <<"Zet3_offset "<<Zet3_offset  <<endl;
    outputFile <<"Zet3_slope "<<Zet3_slope   <<endl;
    outputFile <<"AoQ57_offset "<<AoQ57_offset  <<endl;
    outputFile <<"AoQ57_slope "<<AoQ57_slope  <<endl;
    outputFile <<"Zet7_offset "<<Zet7_offset  <<endl;
    outputFile <<"Zet7_slope "<<Zet7_slope  <<endl;

    outputFile.close();
}
