#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

void results(string NUCLIDE, string BEAM, string LABEL) {
  // Define constants
  const double N_A = 6.02e23;
  const double A_C = 12.011;
  const double d_C_center = 1.503;

  // File input stream
  string filename_C = "results/results_" + NUCLIDE +"_"+ BEAM+ "_C_" + LABEL +"_rev_rev.dat"; 
  string filename_E = "results/results_" + NUCLIDE +"_"+ BEAM+ "_E_" + LABEL +"_rev_rev.dat"; 
  string filename_Summary = "results_"+ LABEL + "/results_" + NUCLIDE +"_"+ BEAM +"_" +  LABEL  +"_Summary.dat"; 
  ifstream file_C(filename_C.c_str());
  ifstream file_E(filename_E.c_str());
  ofstream fout(filename_Summary.c_str());
//  ifstream file_C("results/results_41Ca_41Ca_C_test2.dat");
//  ifstream file_E("results/results_41Ca_41Ca_E_test2.dat");

  // Variables to hold the data
  double f5x_mu_C, f5x_sigma_C,f5x_mu_E, f5x_sigma_E;
  double n0_C, n1_C, n0_E, n1_E;
  double f7x_mu_C,f7x_sigma_C,f7x_mu_E,f7x_sigma_E;
  int n_F7PLA_C,n_noF7PLA_C,n_noF7PLAnoF7PPAC_C;
  int n_F7PLA_E,n_noF7PLA_E,n_noF7PLAnoF7PPAC_E;

  // Read data from file
  if (file_C.is_open()) {
    string line;
    // Read the first set of data
    getline(file_C, line);
    sscanf(line.c_str(), "f5x_mu %lf", &f5x_mu_C);
    getline(file_C, line);
    sscanf(line.c_str(), "f5x_sigma %lf", &f5x_sigma_C);
    getline(file_C, line);
    sscanf(line.c_str(), "n0 %lf", &n0_C);
    getline(file_C, line);
    sscanf(line.c_str(), "n1 %lf", &n1_C);
    getline(file_C, line);
    sscanf(line.c_str(), "f7x_mu %lf", &f7x_mu_C);
    getline(file_C, line);
    sscanf(line.c_str(), "f7x_sigma %lf", &f7x_sigma_C);
    getline(file_C, line);
    sscanf(line.c_str(), "n_F7PLA %d", &n_F7PLA_C);
    getline(file_C, line);
    sscanf(line.c_str(), "n_noF7PLA %d",  &n_noF7PLA_C);
    getline(file_C, line);
    sscanf(line.c_str(), "n_noF7PAnoF7PPAC %d", &n_noF7PLAnoF7PPAC_C);

    file_C.close();
  } else {
    cerr << "Unable to open file for C" << endl;
    return 1;
  }

    // Read the second set of data

  if(file_E.is_open()){
    string line;
    getline(file_E, line);
    sscanf(line.c_str(), "f5x_mu %lf", &f5x_mu_E);
    getline(file_E, line);
    sscanf(line.c_str(), "f5x_sigma %lf", &f5x_sigma_E);
    getline(file_E, line);
    sscanf(line.c_str(), "n0 %lf", &n0_E);
    getline(file_E, line);
    sscanf(line.c_str(), "n1 %lf", &n1_E);
    getline(file_E, line);
    sscanf(line.c_str(), "f7x_mu %lf", &f7x_mu_E);
    getline(file_E, line);
    sscanf(line.c_str(), "f7x_sigma %lf", &f7x_sigma_E);
    getline(file_E, line);
    sscanf(line.c_str(), "n_F7PLA %d", &n_F7PLA_E);
    getline(file_E, line);
    sscanf(line.c_str(), "n_noF7PLA %d",  &n_noF7PLA_E);
    getline(file_E, line);
    sscanf(line.c_str(), "n_noF7PAnoF7PPAC %d", &n_noF7PLAnoF7PPAC_E);
    file_E.close();
  } else {
    cerr << "Unable to open file for E" << endl;
    return 1;
  }
  //Calcu;ate N_C_mean
    double N_C = N_A / A_C * d_C_center *((f5x_mu_C+125)*(0.00490648)+7.344)/7.95731*10.**(-27);
    double R_C = n1_C/n0_C;
    double R_E = n1_E/n0_E;
  // Calculate sigma_I
  double sigma_I = -1.0 / N_C * log(R_C/R_E);

  // Calculate error for sigma_I
  double err_sigma_I = 1.0 / N_C * sqrt((1.-R_C)/(n0_C*R_C)+(1.-R_E)/(n0_E*R_E));

  // Output the result

  cout << NUCLIDE << " " << BEAM << " " << n0_C << " " << n1_C << " " << n0_E << " " << n1_E << " " <<  f5x_mu_C << " " <<  f7x_mu_C << " "<<  f7x_mu_E << " "<<   sigma_I << " " << err_sigma_I << " " << n_F7PLA_C << " " << n_noF7PLA_C<< " " << n_noF7PLAnoF7PPAC_C << " " <<  n_F7PLA_E << " " << n_noF7PLA_E<< " " << n_noF7PLAnoF7PPAC_E <<endl;
  cout << "N0_C(counts) =" << n0_C << endl;
  cout << "N1_C(counts) =" << n1_C << endl;
  cout << "N0_E(counts) =" << n0_E << endl;
  cout << "N1_E(counts) =" << n1_E << endl;
  cout << "F5X(mm) =" << f5x_mu_C << endl; 
  cout << "F7X_C(mm) =" << f7x_mu_C << endl;  
  cout << "F7X_E(mm) =" << f7x_mu_E << endl;  
  cout << "sigma_I(mb) = " << sigma_I << endl;
  cout << "err_sigma_I(mb) = "<< err_sigma_I << endl;
  cout << "N1_F7PLA_C(counts) =" << n_F7PLA_C << endl;
  cout << "N1_noF7PLAC_C(counts) =" << n_noF7PLA_C << endl;
  cout << "N1_noF7PLAnoF7PPAC_C(counts) =" << n_noF7PLAnoF7PPAC_C << endl;
  cout << "N1_F7PLA_E(counts) =" << n_F7PLA_E << endl;
  cout << "N1_noF7PLA_E(counts) =" << n_noF7PLA_E << endl;
  cout << "N1_noF7PLAnoF7PPAC_E(counts) =" << n_noF7PLAnoF7PPAC_E << endl;

  fout << NUCLIDE << " " << BEAM << " " << n0_C << " " << n1_C << " " << n0_E << " " << n1_E << " " <<  f5x_mu_C << " " <<  f7x_mu_C << " "<<  f7x_mu_E << " "<<   sigma_I << " " << err_sigma_I << " " << n_F7PLA_C << " " << n_noF7PLA_C<< " " << n_noF7PLAnoF7PPAC_C << " " <<  n_F7PLA_E << " " << n_noF7PLA_E<< " " << n_noF7PLAnoF7PPAC_E<<endl;
  fout << "N0_C(counts) =" << n0_C << endl;
  fout << "N1_C(counts) =" << n1_C << endl;
  fout << "N0_E(counts) =" << n0_E << endl;
  fout << "N1_E(counts) =" << n1_E << endl;
  fout << "F5X(mm) =" << f5x_mu_C << endl; 
  fout << "F7X_C(mm) =" << f7x_mu_C << endl;  
  fout << "F7X_E(mm) =" << f7x_mu_E << endl;  
  fout << "sigma_I(mb) = " << sigma_I << endl;
  fout << "err_sigma_I(mb) = "<< err_sigma_I << endl;
  fout << "N1_F7PLA_C(counts) =" << n_F7PLA_C << endl;
  fout << "N1_noF7PLAC_C(counts) =" << n_noF7PLA_C << endl;
  fout << "N1_noF7PLAnoF7PPAC_C(counts) =" << n_noF7PLAnoF7PPAC_C << endl;
  fout << "N1_F7PLA_E(counts) =" << n_F7PLA_E << endl;
  fout << "N1_noF7PLA_E(counts) =" << n_noF7PLA_E << endl;
  fout << "N1_noF7PLAnoF7PPAC_E(counts) =" << n_noF7PLAnoF7PPAC_E << endl;
  //  return 0;
}
