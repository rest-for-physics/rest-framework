///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestG4Event.h
///
///             G4 Event class to store results from Geant4 REST simulation  
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestG4Event
#define RestCore_TRestG4Event

#include <iostream>

#include <TObject.h>
#include <TH1D.h>
#include <TRestEvent.h>
#include <TVector3.h>
#include <TRestG4Track.h>

class TRestG4Event: public TRestEvent {

    protected:
        TVector3 fPrimaryEventOrigin;

        std::vector <TVector3> fPrimaryEventDirection;
        std::vector <Double_t> fPrimaryEventEnergy;
        
        Double_t fTotalDepositedEnergy;
        Double_t fSensitiveVolumeEnergy;

        Int_t fNVolumes;
        std::vector <Int_t> fVolumeStored;
        std::vector <Double_t> fVolumeDepositedEnergy;

        Int_t fNTracks;
        std::vector <TRestG4Track> fTrack;

        Int_t fMaxSubEventID;


    public:

        TVector3 GetPrimaryEventDirection( int n ) { return fPrimaryEventDirection[n]; }
        TVector3 GetPrimaryEventOrigin() { return fPrimaryEventOrigin; }
        Double_t GetPrimaryEventEnergy( int n ) { return fPrimaryEventEnergy[n]; }

        Int_t GetNumberOfHits();
        Int_t GetNumberOfTracks() { return fNTracks; }
        Int_t GetNumberOfPrimaries() { return fPrimaryEventDirection.size(); }
        Int_t GetNumberOfActiveVolumes() { return fNVolumes; }

        Int_t isVolumeStored( int n ) { return fVolumeStored[n]; }
        TRestG4Track *GetTrack( int n ) { return &fTrack[n]; }
        TRestG4Track *GetTrackByID( int id );
        Int_t GetNumberOfSubEventIDTracks() { return fMaxSubEventID+1; }

        Double_t GetTotalDepositedEnergy() { return fTotalDepositedEnergy; }
        Double_t GetTotalDepositedEnergyFromTracks();
        Double_t GetEnergyDepositedInVolume( Int_t volID ) { return fVolumeDepositedEnergy[volID]; }
        Double_t GetSensitiveVolumeEnergy( ) { return fSensitiveVolumeEnergy; }

        void SetPrimaryEventOrigin( TVector3 pos ) { fPrimaryEventOrigin = pos; }
        void SetPrimaryEventDirection( TVector3 dir ) { fPrimaryEventDirection.push_back( dir ); }
        void SetPrimaryEventEnergy( Double_t en ) { fPrimaryEventEnergy.push_back( en ); }
        void ActivateVolumeForStorage( Int_t n ) {  fVolumeStored[n] = 1; }
        void DisableVolumeForStorage( Int_t n ) {  fVolumeStored[n] = 0; }

        void AddActiveVolume( );
        void ClearVolumes( );
        void AddEnergyDepositToVolume( Int_t volID, Double_t eDep );
        void AddEnergyToSensitiveVolume( Double_t en ) { fSensitiveVolumeEnergy += en; }

        void SetSensitiveVolumeEnergy( Double_t en ) { fSensitiveVolumeEnergy = en; }

        Int_t GetLowestTrackID( )
        {
            Int_t lowestID;
            if( fNTracks > 0 ) lowestID = GetTrack( 0 )->GetTrackID();

            for( int i = 0; i < fNTracks; i++ )
            {
                TRestG4Track *tr = GetTrack( i );
                if( tr->GetTrackID() < lowestID ) lowestID = tr->GetTrackID();
            }

            return lowestID;
        }

        void SetTrackSubEventID( Int_t n, Int_t id );
        void AddTrack( TRestG4Track trk );

        Bool_t isRadiactiveDecay( )
        {
            for( int n = 0; n < GetNumberOfTracks(); n++ )
                if( GetTrack( n )->isRadiactiveDecay( ) ) return true;
            return false;
        }

        Bool_t isPhotoElectric( )
        {
            for( int n = 0; n < GetNumberOfTracks(); n++ )
                if( GetTrack( n )->isPhotoElectric( ) ) return true;
            return false;
        }
        Bool_t isCompton( )
        {
            for( int n = 0; n < GetNumberOfTracks(); n++ )
                if( GetTrack( n )->isCompton( ) ) return true;
            return false;
        }
        Bool_t isBremstralung( )
        {
            for( int n = 0; n < GetNumberOfTracks(); n++ )
                if( GetTrack( n )->isBremstralung( ) ) return true;
            return false;
        }

        void Initialize();

        void PrintEvent();


        //Construtor
        TRestG4Event();
        //Destructor
        virtual ~ TRestG4Event();

        ClassDef(TRestG4Event, 1);     // REST event superclass
};
#endif
