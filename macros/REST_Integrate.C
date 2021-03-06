
#include <TCanvas.h>
#include <TF1.h>
#include <TH1D.h>
#include <TMath.h>
#include <TRestRun.h>
#include <TRestTask.h>
#include <TSystem.h>

#ifndef RestTask_Integrate
#define RestTask_Integrate

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_Integrate(string varName, string rootFileName, double startVal, double endVal) {
    TRestStringOutput cout;
    cout << "Variable name : " << varName << endl;
    cout << "Integration range : ( " << startVal << " , " << endVal << " ) " << endl;

    if (startVal >= endVal) {
        cout << "Start or End integration values not properly defined!!!" << endl;
        return -1;
    }

    std::vector<string> inputFilesNew = TRestTools::GetFilesMatchingPattern(rootFileName);

    if (inputFilesNew.size() == 0) {
        cout << "Files not found!" << endl;
        return -1;
    }

    TRestRun* run = new TRestRun();
    Int_t integral = 0;

    for (unsigned int n = 0; n < inputFilesNew.size(); n++) {
        run->OpenInputFile(inputFilesNew[n]);

        run->PrintMetadata();

        Int_t obsID = run->GetAnalysisTree()->GetObservableID(varName);
        if (obsID == -1) {
            cout << endl;
            cout.setcolor(COLOR_BOLDRED);
            cout << "No observable \"" << varName << "\" in file " << inputFilesNew[n] << endl;
            continue;
        }
        for (int i = 0; i < run->GetEntries(); i++) {
            run->GetAnalysisTree()->GetBranch((TString)varName)->GetEntry(i);
            Double_t value = run->GetAnalysisTree()->GetDblObservableValue(obsID);
            if (value >= startVal && value < endVal) integral++;
        }
    }

    cout.setcolor(COLOR_BOLDBLUE);
    cout.setborder("*");
    cout << endl;
    cout << "=" << endl;
    cout << "Integral : " << integral << endl;
    cout << "=" << endl;
    cout << endl;

    delete run;

    return 0;
};

#endif
