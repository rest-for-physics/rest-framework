/*************************************************************************
* This file is part of the REST software framework.                     *
*                                                                       *
* Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
* For more information see http://gifna.unizar.es/trex                  *
*                                                                       *
* REST is free software: you can redistribute it and/or modify          *
* it under the terms of the GNU General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or     *
* (at your option) any later version.                                   *
*                                                                       *
* REST is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
* GNU General Public License for more details.                          *
*                                                                       *
* You should have a copy of the GNU General Public License along with   *
* REST in $REST_PATH/LICENSE.                                           *
* If not, see http://www.gnu.org/licenses/.                             *
* For the list of contributors see $REST_PATH/CREDITS.                  *
*************************************************************************/


//////////////////////////////////////////////////////////////////////////
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// \class      TRestEventProcess
/// A base class for any REST event process
///
/// One of the core classes of REST. Absract class
/// from which all REST "event process classes" must derive.
/// An event process in REST is an atomic operation on one single
/// (input) event, potentially producing a different (output) event.
/// Which type of (input/output) events will depent on the type of
/// process. Processes can be tailored via metadata classes.
/// All this needs to be defined in the inherited class.
/// TRestEventProcess provides the basic structure (virtual functions)
///
///
/// History of developments:
///
/// 2014-june: First concept. As part of conceptualization of previous REST
///       code (REST v2) 
///       Igor G. Irastorza
///
//////////////////////////////////////////////////////////////////////////


#include "TRestEventProcess.h"
#include "TRestRun.h"
#include "TClass.h"
#include "TDataMember.h"
#include "TBuffer.h"
using namespace std;

ClassImp(TRestEventProcess)

//////////////////////////////////////////////////////////////////////////
/// TRestEventProcess default constructor

TRestEventProcess::TRestEventProcess()
{
	fObservableNames.clear();
	fSingleThreadOnly = false;
}

//////////////////////////////////////////////////////////////////////////
/// TRestEventProcess destructor

TRestEventProcess::~TRestEventProcess()
{
}

//______________________________________________________________________________
vector<string> TRestEventProcess::ReadObservables()
{
	TiXmlElement* e = GetElement("addObservable");
	vector<string> obsnames;
	while (e != NULL)
	{
		const char* obschr = e->Attribute("type");
		if (obschr != NULL) {
			string obsstring = (string)obschr;
			debug << this->ClassName() << " : setting following observables " << obsstring << endl;
			obsnames = Spilt(obsstring, ":");
			for (int i = 0; i < obsnames.size(); i++) {
				TStreamerElement* se = GetDataMemberWithName(obsnames[i]);
				if (se != NULL)
				{
					if (fAnalysisTree->AddObservable((TString)GetName() + "_" + obsnames[i], (double*)GetDataMemberRef(se)))
						fObservableNames.push_back((TString)GetName() + "_" + obsnames[i]);
				}
				else
				{
					warning << "In " << this->ClassName() << " : observal \"" << obsnames[i] << "\" is not defined as data member, skipping..." << endl;
				}
			}
		}
		e = e->NextSiblingElement("addObservable");
	}
	return obsnames;
}

void TRestEventProcess::SetAnalysisTree(TRestAnalysisTree *tree)
{
	debug << "setting analysis tree for " << this->ClassName() << endl;
	fAnalysisTree = tree;

	
}


void TRestEventProcess::SetFriendProcess(TRestEventProcess*p) 
{
	if (p == NULL)
		return;
	for (int i = 0; i < fFriendlyProcesses.size(); i++) {
		if (fFriendlyProcesses[i]->GetName() == p->GetName())
			return;
	}
	fFriendlyProcesses.push_back(p);
}

void TRestEventProcess::ConfigAnalysisTree() {
	if (fAnalysisTree == NULL)return;

	if (fOutputLevel >= Observable)ReadObservables();
	if (fOutputLevel >= Internal_Var)fAnalysisTree->Branch(this->GetName(), this);

}


Int_t TRestEventProcess::LoadSectionMetadata()
{
	TRestMetadata::LoadSectionMetadata();
	REST_Process_Output lvl;
	string s = GetParameter("outputLevel", "full_output");
	if (s == "no_output") {
		lvl = No_Output;
	}
	else if (s == "observable")
	{
		lvl = Observable;
	}
	else if (s == "internalval")
	{
		lvl = Internal_Var;
	}
	else
	{
		lvl = Full_Output;
	}
	SetOutputLevel(lvl);
	return 0;
}

TRestMetadata *TRestEventProcess::GetMetadata(string name)
{
	TRestMetadata*m= fRunInfo->GetMetadata(name);
	if (m == NULL)m = fRunInfo->GetMetadataClass(name);
	return m;
}


vector<string> TRestEventProcess::GetAvailableObservals()
{
	vector<string> result;
	result.clear();
	int n = GetNumberOfDataMember();
	for (int i = 1; i < n; i++) {
		TStreamerElement *se = GetDataMemberWithID(i);
		if (se->GetType() == 8)
		{
			result.push_back(se->GetFullName());
		}
	}
	return result;
}


void TRestEventProcess::StampOutputEvent(TRestEvent *inEv)
{
	fOutputEvent->Initialize();

	fOutputEvent->SetID(inEv->GetID());
	fOutputEvent->SetSubID(inEv->GetSubID());
	fOutputEvent->SetSubEventTag(inEv->GetSubEventTag());

	fOutputEvent->SetRunOrigin(inEv->GetRunOrigin());
	fOutputEvent->SetSubRunOrigin(inEv->GetSubRunOrigin());

	fOutputEvent->SetTime(inEv->GetTime());
}

/*
//______________________________________________________________________________
void TRestEventProcess::InitProcess()
{
// virtual function to be executed once at the beginning of process
// (before starting the process of the events)
cout << GetName() << ": Process initialization..." << endl;
}

//______________________________________________________________________________
void TRestEventProcess::ProcessEvent( TRestEvent *eventInput )
{
// virtual function to be executed for every event to be processed
}

//______________________________________________________________________________
void TRestEventProcess::EndProcess()
{
// virtual function to be executed once at the end of the process
// (after all events have been processed)
cout << GetName() << ": Process ending..." << endl;
}
*/

//////////////////////////////////////////////////////////////////////////
/// Show default console output at starting of process
///

void TRestEventProcess::BeginPrintProcess()
{
	essential.setcolor(COLOR_BOLDGREEN);
	essential << endl;
	essential << "=" << endl;
	essential << "Process : " << ClassName() << endl;
	essential << "-" << endl;
	essential << "Name: " << GetName() << "  Title: " << GetTitle() << "  VerboseLevel: " << GetVerboseLevelString() << endl;
	essential << "-" << endl;
	string inputeventstr = (fInputEvent == NULL) ? "" : fInputEvent->ClassName();
	string outputeventstr = (fOutputEvent == NULL) ? "" : fOutputEvent->ClassName();
	essential << "InputEvent: " << inputeventstr << "  OutputEvent: " << outputeventstr << endl;
	essential << "=" << endl;
	essential << " " << endl;

	if (fObservableNames.size() > 0)
	{
		essential << " Analysis tree observables added by this process " << endl;
		essential << " +++++++++++++++++++++++++++++++++++++++++++++++ " << endl;
	}

	for (unsigned int i = 0; i < fObservableNames.size(); i++)
	{
		essential << " ++ " << fObservableNames[i] << endl;
	}

	if (fObservableNames.size() > 0)
	{
		essential << " +++++++++++++++++++++++++++++++++++++++++++++++ " << endl;
		essential << " " << endl;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Show default console output at finish of process
///



void TRestEventProcess::EndPrintProcess()
{
	essential << " " << endl;
	essential << "=" << endl;
	essential << endl;
	essential << endl;
	essential.resetcolor();
}

Double_t TRestEventProcess::GetDoubleParameterFromClass(TString className, TString parName)
{
	for (size_t i = 0; i < fFriendlyProcesses.size(); i++)
		if (fFriendlyProcesses[i]->ClassName() == className)
			return StringToDouble(fFriendlyProcesses[i]->GetParameter((string)parName));

	return PARAMETER_NOT_FOUND_DBL;
}

//////////////////////////////////////////////////////////////////////////
/// Retrieve parameter with name parName from metadata className
///
/// \param className string with name of metadata class to access
/// \param parName  string with name of parameter to retrieve
///

Double_t TRestEventProcess::GetDoubleParameterFromClassWithUnits(TString className, TString parName)
{
	for (size_t i = 0; i < fFriendlyProcesses.size(); i++)
		if (fFriendlyProcesses[i]->ClassName() == className)
			return fFriendlyProcesses[i]->GetDblParameterWithUnits((string)parName);

	return PARAMETER_NOT_FOUND_DBL;
}
