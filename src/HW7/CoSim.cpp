// Standard library includes

// ROOT includes
#include "TGraph.h"
#include "TF1.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TLine.h"
#include "TMath.h"
#include "TLegend.h"
#include "TAxis.h"
#include "TRandom.h"

// Local includes
#include "Random.h"

// Directives and declarations for namespaces and namespace members
using std::cout, std::acos, std::cos, std::abs, std::vector, std::string,
      std::stoi;

// Global variables and objects
const double PI = acos(-1.0);
const double CYL_VOL = PI*PI;
const double ANALYTICAL = CYL_VOL/2;
Random rng;

// Program-specific helper functions
/**
 * @param a: first string to be compared
 * @param b: second string to be compared
 * @return 1 if a & b are identical character-wise and in length, 0 otherwise
 */
bool strsame(const string a, const string b);

/**
 * Proposal function
 * @return z-y radius at x = X[0] of cylinder obtained by revolving y = 1 on
 *         [-pi/2, pi/2] about the x axis
 */
double Cyl(const double X[3]);

/**
 * Proposal sampling function
 * @post X contains cartesian coordinates uniformly sampled from the cylinder
 *       obtained by revolving y = 1 on [-pi/2, pi/2] about the x axis
 */
void SampleCyl(double (&X)[3]);

/**
 * Target function
 * @return radius at x = X[0] of volume obtained by revolving y = cos(x) on
 *         [-pi/2, pi/2] about the x axis
 */
double RevCos(const double X[3]);

// Begin primary program function
int main(int argc, char** argv){

  // Command line option parsing variables
  bool argexists = 0;
  bool printhelp = 0;

  // Command line option storage variables
  int mode = 0;
  int samples = 1000;
  int min = 100;
  int max = 100000;
  int step = 1;

  // Parse and process command line options
  for(int i = 1; i < argc; ++i){
    argexists = 0;
    if(strsame(argv[i],"--help") || strsame(argv[i],"-h")){
      argexists = 1;
      printhelp = 1;
    }
    if(strsame(argv[i],"-c")){
      argexists = 1;
      mode = 0;
    }
    if(strsame(argv[i],"--samples")){
      argexists = 1;
      samples = stoi(argv[++i]);
      if(samples <= 0){
        printhelp = 1;
        cout << "Option samples must be a positive integer.\n";
      }
    }
    if(strsame(argv[i],"-e")){
      argexists = 1;
      mode = 1;
    }
    if(strsame(argv[i],"--min")){
      argexists = 1;
      min = stoi(argv[++i]);
      if(min <= 0){
        printhelp = 1;
        cout << "Option min must be a positive integer.\n";
      }
    }
    if(strsame(argv[i],"--max")){
      argexists = 1;
      max = stoi(argv[++i]);
      if(max <= 0){
        printhelp = 1;
        cout << "Option max must be a positive integer.\n";
      }
    }
    if(strsame(argv[i],"--step")){
      argexists = 1;
      step = stoi(argv[++i]);
      if(step <= 0){
        printhelp = 1;
        cout << "Option step must be a positive integer.\n";
      }
    }
    if(!argexists){
      printhelp = 1;
      cout << "Undefined option: " << argv[i] << "\n";
    }
  }

  /* Print usage instructions then exit if the user adds -h or --help,
     doesn't provide necessary input, or provides an undefined option */
  if(printhelp){
    cout << "\nUsage: " << argv[0] << " [options]\n"
         << "  options:\n"
         << "   --help(-h)        print options\n"
         << "== CALCULATOR MODE ================================================"
         << "   -c                run in calculator mode: only output integral\n"
         << "                        approximation and error (default mode)\n"
         << "   --samples [int]   number of sample points (default value 10^3)\n"
         << "== ERROR MODE ====================================================="
         << "   -e                run in error mode: calculate approximation\n"
         << "                        error for a range of number of samples\n"
         << "                        and plot the results\n"
         << "   --min [int]        minimum number of sample points (100)\n"
         << "   --max [int]        maximum number of sample points (10^5)\n"
         << "   --step [int]       steps between number of sample points (1)\n";

    return 0;
  }

  if(mode == 0){
    min = samples;
    max = min;
  }

  // Integration helper variables
  double X[3] = {0};
//  double R = 0;
  int successes = 0;
  int trials = 0;

  // Integration restuls storage objects
  vector<double> approx, error, samplecount;

  // Approximate integration via rejection sampling
  do{
    samples = min;
    while(successes < samples){
      ++trials;
      SampleCyl(X);
/*    R = RevCos(X)/Cyl(X);
      if(rng.rand() <= R){ // Accept and advance
        ++successes;
      } // Reject and repeat     */
      if(sqrt(X[1]*X[1] + X[2]*X[2]) < RevCos(X)){ // Accept and advance
        ++successes;
      } // Reject and repeat
    }
    // Process sampling results as integral approximations
    approx.push_back((double(successes)/trials) * CYL_VOL);
    error.push_back(abs(1.0 - (approx.back()/ANALYTICAL)));
    samplecount.push_back(samples);

    min += step;
  }
  while(min < max);

  if(mode == 0){
    cout << "\nValue: \t\t" << approx[0] << "\n"
         << "Samples: \t" << samples << "\n"
         << "Efficiency: \t" << int(100*double(samples)/trials) << "%\n"
         << "True value: \t" << ANALYTICAL << "\n"
         << "Error: \t\t" << int(error[0]*100) << "%\n\n";
  } else {
    TCanvas* canvas = new TCanvas("c1", "Title", 800, 800);

    double lm = 0.15;
    double rm = 0.05;
    double bm = 0.1;
    double tm = 0.1;
    canvas->SetLeftMargin(lm);
    canvas->SetRightMargin(rm);
    canvas->SetBottomMargin(bm);
    canvas->SetTopMargin(tm);
    canvas->Draw();
    canvas->Update();

    TF1* form = new TF1("f1", "pow(x,[0])", 0, 100000);

    TGraph* graph = new TGraph(error.size(), &samplecount[0], &error[0]);
    graph->SetLineColor(kAzure+1);
    graph->SetLineWidth(2);
    graph->SetMarkerColor(4);
    graph->SetMarkerStyle(0);
    graph->SetTitle("Error vs samples");
    graph->GetXaxis()->SetTitle("samples/integral");
    graph->GetYaxis()->SetTitle("Percent error from analytical result");
    graph->Fit(form);
    graph->Draw();

    canvas->Update();
    canvas->SetLogx();
    canvas->SaveAs("ErrorVsSamples.png");
  }

  return 0;
}

// Program-specific helper function definitions
bool strsame(string a, string b){
  if(a.length()==b.length()){
    int n = a.length();
    for(int i = 0; i < n; i++){
      if(a.at(i)!=b.at(i)){
        return 0;
      }
    }
    return 1;
  }
  return 0;
}

double Cyl(const double X[3]){
  return 1.0;
}

void SampleCyl(double (&X)[3]){
  double r = sqrt(rng.rand());
  double t = 2.0*PI*rng.rand();
  X[0] = (-PI/2.0) + PI*rng.rand();
  X[1] = r*cos(t);
  X[2] = r*sin(t);
}

double RevCos(const double X[3]){
  return cos(X[0]);
}
