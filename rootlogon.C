#if !defined(__CINT__) && !defined(__CLING__)
#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TCanvas.h"

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#endif

void AddIncludePath(std::string dir);
void AddLinkedLibs();
void LoadModule(std::string dir);




void rootlogon()
{
  char* c = getenv("TARTSYS");
  if(c){
    std::string install_dir(c);
  }else{
    std::cout << "set environment variable \"TARTSYS\"" << std::endl;
    std::cout << "quit ROOT" << std::endl;      
    gROOT->ProcessLine(".q");
  } 

  AddIncludePath(install_dir);
  AddLinkedLibs();
  LoadModule(install_dir);


  //  gROOT->SetStyle("Plain");
  gROOT->SetStyle("Modern");

  // default fill color 
  gStyle->SetHistFillColor(kCyan);
  gStyle->SetHistFillStyle(3002);
  // default line color for histograms
  gStyle->SetHistLineColor(kBlue);
  // default line color for Fit functions
  gStyle->SetFuncColor(kRed);
  gStyle->SetFuncWidth(3);

  gStyle->SetCanvasColor(0);
  gStyle->SetCanvasDefH(600);
  gStyle->SetCanvasDefW(600);
  gStyle->SetPadBorderMode(1);
  gStyle->SetPadRightMargin(0.15);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadTopMargin(0.15);
  gStyle->SetPadBottomMargin(0.15);

  // grid configuration
  gStyle->SetNdivisions(508,"X");
  gStyle->SetNdivisions(508,"Y");
  gStyle->SetPadGridX(1);
  gStyle->SetPadGridY(1);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetGridStyle(3);
  gStyle->SetGridColor(16);

  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleY(0.93);
  gStyle->SetTitleXOffset(1.5);
  gStyle->SetTitleYOffset(2.0);
  gStyle->SetTitleAlign(22);
  gStyle->SetStatColor(0);
  gStyle->SetStatStyle(0);
  gStyle->SetStatX(0.85);
  gStyle->SetStatY(0.85);
  gStyle->SetStatW(0.35);
  gStyle->SetStatH(0.50);
  //  gStyle->SetStatH(0.25);

  //===== Palette ==================================================
  const Int_t NRGBs = 5;    //色の区切りの数（区切り色　= RGB空間に打たれた点のさす色）
  const Int_t NCont = 255;  //グラデーションの段数
 
  //グラデーションの色を配列で指定
  //stopはカラーバーでの位置（比率）。つまり、区切りの色がくる場所。
  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 }; 
  //Red、Green,Blueは各区切り位置でのR,G,Bの値（区切り色を指定）  　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　
  //RGBは一般的なRGBの値を255で割って規格化（？）している。
  Double_t Red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  //縦に見る。カラーバーの位置0.00でのRGB=(0.00,0.00,0.51),
  Double_t Green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  //位置0.34でのRGB=(0.00,0.81,1.00),のようになっている。
  Double_t Blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 }; 
  //グラデーションを作る。勝手にgStyle->SetPalette()してくれる
  TColor::CreateGradientColorTable(NRGBs, stops, Red, Green, Blue, NCont);
  gStyle->SetNumberContours(NCont);
  //================================================================================

  //  gStyle->SetPalette(55);
  gStyle->SetStripDecimals(kFALSE);

  // stat option
  gStyle->SetOptStat(1111111);
  //  gStyle->SetOptStat(0000000);
  gStyle->SetStatFont(42);
  gStyle->SetStatFontSize(0.03);
  //  gStyle->SetStatX(0.9);
  //  gStyle->SetStatY(0.9);
  //  gStyle->SetStatW(0.17);
  gStyle->SetOptFit();
  gStyle->SetOptFit(1111);


  // tick
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  //  gStyle->SetPadTickLength(0.00001,"X");

  // text configuration
  gStyle->SetLabelFont(42,"XYZ");
  gStyle->SetLabelSize(0.04,"XYZ");
  gStyle->SetLabelOffset(0.015,"XY");
  gStyle->SetTitleFont(42,"XYZ");
  gStyle->SetTitleFont(42,"");
  gStyle->SetTextFont(42);
  gStyle->SetStatFont(42);

  // Title position
  //  gStyle->SetTitleBorderSize(0);
  //  gStyle->SetTitleAlign(13);
  //  gStyle->SetTextAlign(12);
  //  gStyle->SetTitleX(0.075);
  //  gStyle->SetTitleY(0.958);

  // Fill color on
  //gStyle->SetFillStyle(3004);
  //  gStyle->SetFillColor(0);

  // Done
  gROOT->ForceStyle();

  // Macros
  //  gROOT->LoadMacro("./macros/Treso2D.C");
  //  gROOT->LoadMacro("./monitor/Monitor_C.so");
  //  gROOT->LoadMacro("./macros/plmacro.cxx");
  gROOT->LoadMacro("./macros/output.C");
  gROOT->LoadMacro("./macros/ID.C");
  gROOT->LoadMacro("./macros/Gsave.C");

}


void AddIncludePath(std::string install_dir)
{
  std::vector<std::string> include;
#if ROOT_VERSION_CODE > ROOT_VERSION(5,99,99)
  include.push_back(install_dir + "/include");
#else
  include.push_back("-I"+install_dir+"/include");
#endif
  //  include.push_back("`xml2-config --cflags`");

  std::vector<std::string>::iterator it = include.begin();
  while(it != include.end()){
#if ROOT_VERSION_CODE > ROOT_VERSION(5,99,99)
    gInterpreter->AddIncludePath((*it).c_str());
#else
    gSystem->AddIncludePath((*it).c_str());
#endif
    std::cout << "add include path : " << *it << std::endl;
    ++it;
  }
}

void AddLinkedLibs()
{
//  std::vector<std::string> include;
//  include.push_back("`xml2-config --libs`");

//  std::vector<std::string>::iterator it = include.begin();
//  while(it != include.end()){
//    gSystem->AddLinkedLibs((*it).c_str());
//    std::cout << "add linked libs : " << *it << std::endl;
//    ++it;
//  }
}

void LoadModule(std::string install_dir)
{
  std::vector<std::string> modules;
  modules.push_back("libXMLParser.so");
  modules.push_back(install_dir+"/lib/"+"libanaroot.so"); // load at once
//  modules.push_back(install_dir+"/lib/"+"libananadeko.so"); // load each modules one by one
//  modules.push_back(install_dir+"/lib/"+"libanacore.so");
//  modules.push_back(install_dir+"/lib/"+"libanabrips.so");
//  modules.push_back(install_dir+"/lib/"+"libanadali.so");
//  modules.push_back(install_dir+"/lib/"+"libanasamurai.so");
//  modules.push_back(install_dir+"/lib/"+"libanaanaloop.so");

  std::vector<std::string>::iterator it = modules.begin();
  while(it != modules.end()){
    std::cout << "reading " << *it << std::endl;
    if(gSystem->Load((*it).c_str()) < 0){
      std::cout << "cannnot read in " << *it << std::endl;      
    }
    ++it;
  }
}

