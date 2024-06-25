void output(TH1 *h1,const char *name3){
  /*
    name3 : output file name, output/[name3].dat
  */

  //name3
  TString str3 = "output/";
  std::ostringstream oss3;
  oss3 << name3;
  str3+=oss3.str();
  str3+=".dat";
  ofstream fout(str3.Data());

  for(int i=1;i<(h1->GetNbinsX())+1;i++){
    fout << h1->GetBinLowEdge(i) << " " << h1->GetBinContent(i) << endl;
  }

  fout.close();
  //  file.close();
}
