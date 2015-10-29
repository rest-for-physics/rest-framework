///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRun.cxx
///
///             Base class for managing run data storage. It contains a TRestEvent and TRestMetadata array. 
///
///             apr 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///             aug 2015    Javier Galan
///_______________________________________________________________________________


#include "TRestRun.h"

ClassImp(TRestRun)
//______________________________________________________________________________
    TRestRun::TRestRun()
{
    Initialize();

    SetVersion();
}

TRestRun::TRestRun( char *cfgFileName) : TRestMetadata (cfgFileName)
{
    Initialize();

    LoadConfig( "run", fConfigFileName );

    SetVersion();
}

void TRestRun::Initialize()
{
    cout << __PRETTY_FUNCTION__ << endl;

    time_t  timev; time(&timev);
    fStartTime = (Double_t) timev;
    fEndTime = fStartTime-1; // So that run length will be -1 if fEndTime is not set

    fRunIndex = 0;
    fRunUser = getenv("USER");
    fRunType = "Null";
    fExperimentName = "Null";
    fRunTag = "Null";

    fOutputFile = NULL;
    fInputFile = NULL;

    fInputEvent = NULL;
    fOutputEvent = NULL;

    fInputEventTree = NULL;
    fOutputEventTree = NULL;

    fInputFilename = "null";
    fOutputFilename = "null";

    fGeometry = NULL;
}

void TRestRun::ResetRunTimes()
{
    time_t  timev; time(&timev);
    fStartTime = (Double_t) timev;
    fEndTime = fStartTime-1; // So that run length will be -1 if fEndTime is not set
}


//______________________________________________________________________________
TRestRun::~TRestRun()
{
    cout << "Deleting TRestRun" << endl;
    if( fOutputFile != NULL ) CloseOutputFile();
}

void TRestRun::Start( )
{
    if( fEventProcess.size() == 0 ) { cout << "WARNNING Run does not contain processes" << endl; return; }

    this->OpenOutputFile();

    this->SetInputEvent( fEventProcess.front()->GetInputEvent() );

    this->SetOutputEvent( fEventProcess.back()->GetOutputEvent() );

    this->SetRunType( fEventProcess[fEventProcess.size()-1]->GetProcessName() );

    this->ResetRunTimes();

    for( unsigned int i = 0; i < fEventProcess.size(); i++ ) fEventProcess[i]->InitProcess();

    TRestEvent *processedEvent;
    for( unsigned int i = 0; i < fInputEventTree->GetEntries(); i++ )
    {
        processedEvent = fInputEvent;
        fInputEventTree->GetEntry( i );

        for( unsigned int j = 0; j < fEventProcess.size(); j++ )
        {
            fEventProcess[j]->BeginOfEventProcess();
            processedEvent = fEventProcess[j]->ProcessEvent( processedEvent );
            if( processedEvent == NULL ) break;
            fEventProcess[j]->EndOfEventProcess();
        }
        if( processedEvent == NULL ) continue;

        fOutputEvent = processedEvent;

        fOutputEvent->SetEventID( fInputEvent->GetEventID() );

        fOutputEventTree->Fill();
    }

    for( unsigned int i = 0; i < fEventProcess.size(); i++ )
        fEventProcess[i]->EndProcess();

}

void TRestRun::RunProcess(TRestEventProcess *process){


this->OpenOutputFile();
this->SetOutputEvent( process->GetOutputEvent() );
this->SetRunType( process->GetProcessName() );
this->ResetRunTimes();

TRestEvent *processedEvent;

process->InitProcess();


	while(processedEvent!=NULL){
	
	process->BeginOfEventProcess();
	processedEvent = process->ProcessEvent(NULL);
	fOutputEvent = processedEvent;
	fOutputEventTree->Fill();
	process->EndOfEventProcess();
	//if(fOutputEventTree->GetEntries()>1000)break;
	
	PrintProcessedEvents(100);
		
	}

cout<<fOutputEventTree->GetEntries()<<" processed events "<<endl;

process->EndProcess();

}


void TRestRun::OpenInputFile( TString fName )
{
    if( fInputFile != NULL ) fInputFile->Close();

    if( !fileExists( fName.Data() ) ) {
        cout << "TRestRun. WARNING. Input file does not exist" << endl;
        return;
    }

    fInputFilename = fName;

    fInputFile = new TFile( fName );
    TIter nextkey(fInputFile->GetListOfKeys());
    TKey *key;
    while ( (key = (TKey*)nextkey() ) ) {

        string className = key->GetClassName();

        if ( className == "TRestRun" ) this->Read( key->GetName() );

    }

    // Transfering metadata to historic
    for( size_t i = 0; i < fMetadata.size(); i++ )
        fHistoricMetadata.push_back( fMetadata[i] );
    fMetadata.clear();
    for( size_t i = 0; i < fEventProcess.size(); i++ )
        fHistoricEventProcess.push_back( fEventProcess[i] );
    fEventProcess.clear();
}

void TRestRun::OpenInputFile( TString fName, TString cName )
{
    cout << __PRETTY_FUNCTION__ << endl;
    cout << "OBSOLETE.........." << endl;
    /*
    if( fInputFile != NULL ) fInputFile->Close();

    fInputFile = new TFile( fName );
    TIter nextkey(fInputFile->GetListOfKeys());
    TKey *key;
    while ( (key = (TKey*)nextkey() ) ) {
        string className = key->GetClassName();

        if ( className == cName )
        {
            this->Read( key->GetName() );
        }
    }
    */
}

Bool_t TRestRun::isClass(TString className){

if( fInputFile == NULL){cout<<"No input file"<<endl;return kFALSE;}

TIter nextkey(fInputFile->GetListOfKeys());
    TKey *key;
    while ( (key = (TKey*)nextkey() ) ) {
        TString cName (key->GetName());

        
        if ( cName.Contains(className.Data()) ){
        cout << "className : " << cName << " target "<<className << endl;
        return kTRUE;
        }
    }

cout<<"Class "<<className<<" not found"<<endl;

return kFALSE;
}



void TRestRun::OpenOutputFile( )
{
    SetRunFilenameAndIndex();

    if( GetVerboseLevel() == REST_Info ) cout << "Opening file : " << fOutputFilename << endl;

    fOutputFile = new TFile( fOutputFilename, "recreate" );

    fOutputEventTree  = new TTree( GetName(), GetTitle() );
    if( GetVerboseLevel() == REST_Debug ) cout << "Creating tree : " << fOutputEventTree << endl;
}

void TRestRun::CloseOutputFile( )
{
    cout << __PRETTY_FUNCTION__ << endl;
    time_t  timev;
    time(&timev);

    fEndTime = (Double_t) timev;

    fOutputFile->cd();

    char tmpString[256];
    if( fMetadata.size() > 0 )
    {
        for( unsigned int i = 0; i < fMetadata.size(); i++ )
        {
            cout << "Writting metadata (" << fMetadata[i]->GetName() << ") : " << fMetadata[i]->GetTitle() << endl;
            sprintf( tmpString, "M%d. %s", i,  fMetadata[i]->GetName() );
            fMetadata[i]->Write( tmpString );
        }
    }

    if( fHistoricMetadata.size() > 0 )
    {
        for( unsigned int i = 0; i < fHistoricMetadata.size(); i++ )
        {
            cout << "Writting historic metadata (" << fHistoricMetadata[i]->GetName() << ") : " << fHistoricMetadata[i]->GetTitle() << endl;
            sprintf( tmpString, "HM%d. %s", i,  fHistoricMetadata[i]->GetName() );
            fHistoricMetadata[i]->Write( tmpString );
        }
    }

    if( fEventProcess.size() > 0 )
    {
        for( unsigned int i = 0; i < fEventProcess.size(); i++ )
        {
            cout << "Writting process (" << fEventProcess[i]->GetName() << ") : " << fEventProcess[i]->GetTitle() << endl;
            sprintf( tmpString, "P%d. %s", i,  fEventProcess[i]->GetName() );
            fEventProcess[i]->Write( tmpString );
        }
    }

    if( fHistoricEventProcess.size() > 0 )
    {
        for( unsigned int i = 0; i < fHistoricEventProcess.size(); i++ )
        {
            cout << "Writting historic process (" << fHistoricEventProcess[i]->GetName() << ") : " << fHistoricEventProcess[i]->GetTitle() << endl;
            sprintf( tmpString, "HP%d. %s", i,  fHistoricEventProcess[i]->GetName() );
            fHistoricEventProcess[i]->Write( tmpString );
        }
    }

    //else { if( GetVerboseLevel() >= REST_Warning ) cout << "WARNNNNING : No Geometry found" << endl; }

    if( fOutputEventTree != NULL ) { cout << "Writting output tree" << endl; fOutputEventTree->Write(); }

    if( fGeometry != NULL ){ cout << "Writting geometry" << endl; fGeometry->Write(); cout << "End writting" << endl; }

    this->Write();

    cout << "Closing output file : " << endl;
    fOutputFile->Close();
    cout << fOutputFilename << endl;
}

void TRestRun::SetVersion()
{

    char originDirectory[255];
    sprintf( originDirectory, "%s", get_current_dir_name() );

    char buffer[255];
    sprintf( buffer, "%s", getenv( "REST_PATH" ) );
    chdir( buffer );

    // Reading the version of libcore.so
    FILE *fV = popen("git rev-list --count --first-parent HEAD", "r");
    int nbytes;
    string versionStr;
    while ((nbytes = fread(buffer, 1, 255, fV)) > 0)
    {
        versionStr = buffer;
        size_t last = versionStr.find("\n");
        versionStr = versionStr.substr(0, last );
    }

    pclose( fV );

    chdir( originDirectory );

    fVersion = versionStr;
}


TString TRestRun::GetTime( Double_t runTime )
{
       time_t tt = (time_t) runTime;
       struct tm *tm = localtime( &tt);

       char time[256];
       strftime(time, sizeof(time), "%H:%M:%S", tm);

       return time;
}

TString TRestRun::GetDateForFilename( Double_t runTime )
{
       time_t tt = (time_t) runTime;
       struct tm *tm = localtime( &tt);

       char date[256];
       strftime(date, sizeof(date), "%Y%m%d", tm);

       return date;
}

TString TRestRun::GetDateFormatted( Double_t runTime )
{
       time_t tt = (time_t) runTime;
       struct tm *tm = localtime( &tt);

       char date[256];
       strftime(date, sizeof(date), "%Y-%B-%d", tm);

       return date;
}

void TRestRun::PrintStartDate()
{
       cout.precision(10);

       cout << "------------------------" << endl;
       cout << "---- Run start date ----" << endl;
       cout << "------------------------" << endl;
       cout << "Unix time : " << fStartTime << endl;
       time_t tt = (time_t) fStartTime;
       struct tm *tm = localtime( &tt);

       char date[20];
       strftime(date, sizeof(date), "%Y-%m-%d", tm);
       cout << "Date : " << date << endl;

       char time[20];
       strftime(time, sizeof(time), "%H:%M:%S", tm);
       cout << "Time : " << time << endl;
       cout << "++++++++++++++++++++++++" << endl;
}

void TRestRun::PrintEndDate()
{
       cout << "----------------------" << endl;
       cout << "---- Run end date ----" << endl;
       cout << "----------------------" << endl;
       cout << "Unix time : " << fEndTime << endl;
       time_t tt = (time_t) fEndTime;
       struct tm *tm = localtime( &tt);

       char date[20];
       strftime(date, sizeof(date), "%Y-%m-%d", tm);
       cout << "Date : " << date << endl;

       char time[20];
       strftime(time, sizeof(time), "%H:%M:%S", tm);
       cout << "Time : " << time << endl;
       cout << "++++++++++++++++++++++++" << endl;
}

Double_t TRestRun::GetRunLength()
{
    if( fEndTime-fStartTime == -1 )
        cout << "Run time is not set" << endl;
    return fEndTime-fStartTime;
}

//______________________________________________________________________________
void TRestRun::InitFromConfigFile()
{
    cout << __PRETTY_FUNCTION__ << endl;

   // Initialize the metadata members from a configfile
   fRunEvents = StringToInteger( GetParameter( "Nevents" ) );

   if( GetParameter( "user" ) != "system" )
       fRunUser = GetParameter( "user" );

   fRunType = GetParameter( "runType" );

   fRunDescription = GetParameter( "runDescription" );

   fRunNumber = StringToInteger ( GetParameter( "runNumber" ) );
   fExperimentName = GetParameter( "experiment" );

   fRunTag = GetParameter( "runTag" );
}

void TRestRun::SetRunFilenameAndIndex()
{ 

    string expName = RemoveWhiteSpaces( (string) GetExperimentName() );
    string runType = RemoveWhiteSpaces( (string) GetRunType() );
    char runIndexStr[256];
    sprintf( runIndexStr, "%03d", fRunIndex );
    char runNumberStr[256];
    sprintf( runNumberStr, "%03d", fRunNumber );

    fOutputFilename = GetDataPath() + "/Run_" + expName + "_"+ fRunUser + "_"  
        + runType + "_" + fRunTag + "_" + (TString) runIndexStr + "_r" + fVersion + ".root";

    while( fileExists( (string) fOutputFilename ) )
    {
        fRunIndex++;
        sprintf( runIndexStr, "%03d", fRunIndex );
        fOutputFilename = GetDataPath() + "/Run_" + expName + "_"+ fRunUser + "_"  
            + runType + "_" + fRunTag + "_" + (TString) runIndexStr + "_r" + fVersion + ".root";
    }
}


void TRestRun::PrintInfo( )
{

        cout.precision(10);
        cout << endl;
        cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
        cout << "TRestRun content" << endl;
        cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
        cout << "Config file : " << fConfigFileName << endl;
        cout << "Section name : " << fSectionName << endl;        // section name given in the constructor of TRestSpecificMetadata
        cout << "---------------------------------------" << endl;
        cout << "Name : " << GetName() << endl;
        cout << "Title : " << GetTitle() << endl;
        cout << "---------------------------------------" << endl;
        cout << "Run number : " << GetRunNumber() << endl; 
        cout << "Run index : " << GetRunIndex() << endl; 
        cout << "Run type : " << GetRunType() << endl;
        cout << "Run tag : " << GetRunTag() << endl;
        cout << "Run user : " << GetRunUser() << endl;
        cout << "Run description : " << GetRunDescription() << endl;
        cout << "Run events : " << GetNumberOfEvents() << endl;
        cout << "Start timestamp : " << GetStartTimestamp() << endl;
        cout << "Date/Time : " << GetDateFormatted( GetStartTimestamp() ) << " / " << GetTime( GetStartTimestamp() ) << endl;
        cout << "End timestamp : " << GetEndTimestamp() << endl;
        cout << "Date/Time : " << GetDateFormatted( GetEndTimestamp() ) << " / " << GetTime( GetEndTimestamp() ) << endl;
        cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;

}

void TRestRun::PrintProcessedEvents( Int_t rateE){

if(fOutputEventTree->GetEntries()%rateE ==0)printf("%d processed events now...\r",(int)fOutputEventTree->GetEntries());
fflush(stdout);

}






