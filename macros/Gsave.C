void Gsave(char const *Cname){ 
  //void Gsave(char const *Cname,TString *fname){ 
  TCutG*mycut;

  /*
  TString str = "./cut/";
  std::ostringstream oss;
  oss <<Cname;
  str+=oss.str();
  */
  TString fname;
  fname="./cut/";
  fname+=Cname;
  fname+=".C";

    mycut=(TCutG*)gROOT->GetListOfSpecials()->FindObject(Cname);
  //    mycut=(TCutG*)gROOT->GetListOfSpecials()->FindObject("CUTG");
  mycut->SetName(Cname);
  //  mycut->SetName("./cut/gate");

  cout << "Comment ?" << endl;
  TString comment;
  cin >> comment;
  mycut->SetTitle(comment.Data());
  
  mycut->SaveAs(fname);
  //  mycut->SaveAs("./cut/"+Cname+".C");
}
