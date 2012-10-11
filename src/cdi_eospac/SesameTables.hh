//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   cdi_eospac/SesameTables.hh
 * \author Kelly Thompson
 * \date   Fri Apr  6 08:57:48 2001
 * \brief  Header file for SesameTables (mapping material IDs
 *         to Sesame table indexes).
 * \note   Copyright (C) 2001-2012 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef __cdi_eospac_SesameTables_hh__
#define __cdi_eospac_SesameTables_hh__

#include "eos_Interface.h"
#include <vector>

namespace rtt_cdi_eospac
{
 
//===========================================================================//
/*!
 * \class SesameTables
 * 
 * \brief This is a helper class for Eospac.  It tells Eospac what Sesame data
 *        is being requested and what lookup tables to use.
 *
 * \sa The web page for <a 
 *     href="http://laurel.lanl.gov/XCI/PROJECTS/DATA/eos/eos.html">EOSPAC</a>.
 *
 * \sa The web page for <a
 *     href="http://int.lanl.gov/projects/sdm/win/materials/">Eos Material
 *     Identifiers</a>.  This web site also does dynamic plotting of EoS
 *     values.
 *
 * Each sesame material definition has 16 data tables (actually material
 * identifiers) that define its state.  At least one table must be defined for
 * this to be a valid object.  This list of tables is used by the Eospac
 * constructor to determine what Sesame table data to cache.  There are 37
 * return types defined by EOSPAC.  Some data tables provide information for
 * more than one return type.
 *
 * \example cdi_eospac/test/tEospac.cc
 */

// revision history:
// -----------------
// 0) original
// 
//===========================================================================//
    
class SesameTables 
{
    // DATA
	
    //! There are 305 return types defined by EOSPAC (see eos_Interface.h)
    unsigned const numReturnTypes; // should be 305 

    /*!
     * \brief Map from EOSPAC data type to material identifier.
     *
     * Each of the enumerated EOSPAC data types can have a different
     * SesameTable material identifier.  This vector contains a list of these
     * material IDs.
     */
    std::vector< unsigned > matMap;

    //! Toggle list to identify which data types have been requested.
    std::vector< EOS_INTEGER > rtMap;

  public:
	
    // CREATORS
	
    SesameTables(void);
	
    // ACCESSORS

    // set functions

    //! Thermoelectric Coefficient (1/cm^2/s)
    SesameTables& B_DT( unsigned matID );
    //! Shear Modulus (Gpa)
    SesameTables& Gs_D( unsigned matID );
    //! Electron Conducitive Opacity (cm^2/g)
    SesameTables& Kc_DT( unsigned matID );
    //! Electrical Conductivity (1/s)
    SesameTables& Kec_DT( unsigned matID );
    //! Electron Conductive Opacity (cm^2/g)
    SesameTables& Keo_DT( unsigned matID );
    //! Planck Mean Opacity (cm^2/g)
    SesameTables& Kp_DT(  unsigned matID );
    //! Rosseland Mean Opacity (cm^2/g)
    SesameTables& Kr_DT( unsigned matID );
    //! Thermal Conductivity (1/cm/s)
    SesameTables& Ktc_DT( unsigned matID );
    //! Pressure Cold Curve (GPa)
    SesameTables& Pc_D( unsigned matID );
    //! Electron Pressure (GPa)
    SesameTables& Pe_DT( unsigned matID );
    //! Electron Pressure (GPa)
    SesameTables& Pe_DUe( unsigned matID );
    //! Freeze Pressure (GPa)
    SesameTables& Pf_D( unsigned matID );
    //! Ion Pressure plus Cold Curve Pressure (GPa)
    SesameTables& Pic_DT( unsigned matID );
    //! Ion Pressure plus Cold Curve Pressure (GPa)
    SesameTables& Pic_DUic( unsigned matID );
    //! Melt Pressure (GPa)
    SesameTables& Pm_D(  unsigned matID );
    //! Total Pressure (GPa)
    SesameTables& Pt_DT( unsigned matID );
    //! Total Pressure (Gpa)
    SesameTables& Pt_DUt( unsigned matID );
    //! Temperature (K)
    SesameTables& T_DPe( unsigned matID );
    //! Temperature (K)
    SesameTables& T_DPic( unsigned matID );
    //! Temperature (K)
    SesameTables& T_DPt( unsigned matID );
    //! Temperature (K)
    SesameTables& T_DUe( unsigned matID );
    //! Temperature (K)
    SesameTables& T_DUic( unsigned matID ); 
    //! Temperature (K)
    SesameTables& T_DUt( unsigned matID );
    //! Freeze Temperature (eV)
    SesameTables& Tf_D( unsigned matID );
    //! Melt Temperature (K)
    SesameTables& Tm_D(  unsigned matID );
    //! Specific-Internal-Energy Cold Curve (MJ/kg)
    SesameTables& Uc_D( unsigned matID );
    //! Electron Specific-Internal-Energy (MJ/kg)
    SesameTables& Ue_DPe( unsigned matID );
    //! Electron Specific-Internal-Energy (MJ/kg)
    SesameTables& Ue_DT( unsigned matID ); 
    //! Freeze Specific-Internal-Energy (MJ/kg)
    SesameTables& Uf_D( unsigned matID );
    //! Ion Specific-Internal-Energy plus Cold Curve Specific-Internal-Energy (MJ/kg)
    SesameTables& Uic_DPic( unsigned matID ); 
    //! Ion Specific-Internal-Energy plus Cold Curve Specific-Internal-Energy (MJ/kg)
    SesameTables& Uic_DT( unsigned matID );
    //! Melt Specific-Internal-Energy (MJ/kg)
    SesameTables& Um_D(  unsigned matID );
    //! Melt Specific-Internal-Energy (MJ/kg)
    SesameTables& Ut_DPt( unsigned matID );
    //! Melt Specific-Internal-Energy (MJ/kg)
    SesameTables& Ut_DT( unsigned matID );
    //! Mean Ion Charge (Conductivity Model) (free electrons per atom)
    SesameTables& Zfc_DT( unsigned matID );
    //! Mean Ion Charge (Opacity Modlel) (free electrons per atom)
    SesameTables& Zfo_DT( unsigned matID );

    // More Aliases

    SesameTables& Cve(   unsigned matID ) { return Ue_DT( matID );}
    SesameTables& Cvi(   unsigned matID ) { return Uic_DT( matID );}
// 	SesameTables& zfree( unsigned matID ) { return Zfc_DT( matID );}
// 	SesameTables& chie(  unsigned matID ) { return Ktc_DT( matID );}

    // Get functions
	
    //! Return the material identifier associated with a Sesame return type.
    unsigned matID( EOS_INTEGER returnType ) const;

    //! Return the enumerated data type associated with the integer index.
    EOS_INTEGER returnTypes( unsigned index ) const;

    //! Return the number of return types 
    unsigned getNumReturnTypes() const { return numReturnTypes; }
        
}; // end class SesameTables
    
} // end namespace rtt_cdi_eospac

#endif  // __cdi_eospac_SesameTables_hh__

//---------------------------------------------------------------------------//
// end of cdi_eospac/SesameTables.hh
//---------------------------------------------------------------------------//
