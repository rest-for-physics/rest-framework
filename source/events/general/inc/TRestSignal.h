///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignal.h
///
///             Event class to store signals fromm simulation and acquisition events 
///
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 JuanAn Garcia/Javier Galan
///		nov 2015:
///		    Changed vectors fSignalTime and fSignalCharge from <Int_t> to <Float_t>
///	            JuanAn Garcia
///_______________________________________________________________________________


#ifndef RestCore_TRestSignal
#define RestCore_TRestSignal

#include <iostream>

#include <TObject.h>
#include <TString.h>
#include <TVector2.h>


class TRestSignal: public TObject {

    protected:
        
        Int_t fSignalID;
        
        std::vector <Float_t> fSignalTime;   //Vector with the time of the signal
        std::vector <Float_t> fSignalCharge; //Vector with the charge of the signal
        
        void AddPoint(TVector2 p);
        
    public:
	
        //Getters
        TVector2 GetPoint( Int_t n )
        {
            TVector2 vector2( GetTime(n), GetData(n) );

            return vector2;
        }
        
        Int_t GetSignalID( ) { return fSignalID; }
        
        Int_t GetNumberOfPoints()
        {
            if(fSignalTime.size()!=fSignalCharge.size())
            {
                std::cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
                std::cout<<"WARNING, the two vector sizes did not match"<<std::endl;
                std::cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
            }
            return fSignalTime.size(); 
        }

        Double_t GetIntegral( );
        Double_t GetIntegral( Int_t ni, Int_t nf );
        Double_t GetIntegralWithThreshold( Int_t ni, Int_t nf, Double_t threshold );

        Double_t GetAverage( Int_t start, Int_t end );
        Int_t GetMaxIndex();
        Int_t GetMaxPeakWidth();
        Double_t GetMaxPeakValue();

        Double_t GetData( Int_t index ) { return (double)fSignalCharge[index]; }
        Double_t GetTime( Int_t index ) { return (double)fSignalTime[index]; }
        Int_t GetTimeIndex(Double_t t);
        
        //Setters
        void SetSignalID(Int_t sID) { fSignalID = sID; }

        void AddPoint( Double_t t, Double_t d );
        void AddCharge( Double_t t, Double_t d );
        void AddDeposit( Double_t t, Double_t d );
        
        void MultiplySignalBy( Double_t factor );
        void ExponentialConvolution( Double_t fromTime, Double_t decayTime, Double_t offset = 0 );
        void SignalAddition( TRestSignal *inSgnl ); 

        Bool_t isSorted( );
        void Sort();

        void GetDifferentialSignal( TRestSignal *diffSgnl, Int_t smearPoints = 5 );
        void GetSignalDelayed( TRestSignal *delayedSignal, Int_t delay );
        void GetSignalSmoothed( TRestSignal *smthSignal, Int_t averagingPoints = 3 );

        void AddGaussianSignal( Double_t amp, Double_t sigma, Double_t time, Int_t N, Double_t fromTime, Double_t toTime );

        void Reset() { fSignalTime.clear();fSignalCharge.clear();}

        void WriteSignalToTextFile ( TString filename );
        void Print( );
                
        //Construtor
        TRestSignal();
        //Destructor
        ~TRestSignal();
        
        ClassDef(TRestSignal, 1);



};
#endif



