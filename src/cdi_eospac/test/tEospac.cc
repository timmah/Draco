//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   cdi_eospac/test/tEospac.cc
 * \author Kelly Thompson
 * \date   Mon Apr 2 14:20:14 2001
 * \brief  Implementation file for tEospac
 * \note   Copyright (C) 2001-2012 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "../Eospac.hh"
#include "../SesameTables.hh"
#include "../EospacException.hh"
#include "ds++/ScalarUnitTest.hh"
#include "ds++/Release.hh"
#include "ds++/Soft_Equivalence.hh"
#include "ds++/SP.hh"
#include "ds++/Assert.hh"

#include <iostream>
#include <cmath>
#include <vector>
#include <sstream>

#define PASSMSG(m) ut.passes(m)
#define FAILMSG(m) ut.failure(m)
#define ITFAILS    ut.failure( __LINE__, __FILE__ )

namespace rtt_cdi_eospac_test
{

using rtt_dsxx::SP;
using rtt_dsxx::soft_equiv;

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

//!  Tests the Eospac constructor and access routines.
void cdi_eospac_test( rtt_dsxx::UnitTest & ut )
{
    // Start the test.

    std::cout << std::endl
	      << "Test of C++ code calling EOSPAC routines" 
	      << std::endl << std::endl;
    
    // ---------------------------- //
    // Create a SesameTables object //
    // ---------------------------- //
    
    // The user must create a SesameTables object that links material ID
    // numbers to EOSPAC data types (each SesameTables object only contains
    // lookups for one material).  If the user needs heat capacity values for
    // Al then he/she must create a SesameTables object for Aluminum and then
    // assign an aluminum material ID (e.g. 3717) to the enelc EOSPAC data
    // type.  See the tests below for more details.
    
    // Set the material identifier
    // This one is for Aluminum (03717) 
    // Category-1 data (0) + Mat# 371 (Al) + Version # 7
    
    // See http://xweb.lanl.gov/projects/data/ for material ID information.
    
    // This matID for Al has lookup tables for prtot, entot, tptot, tntot,
    // pntot, eptot, prelc, enelc, tpelc, tnelc pnelc, epelc, prcld, and encld
    // (see SesameTables.hh for an explanantion of these keywords).  I need
    // the table that contains enion lookups so that I can query for Cve()
    // values.

    // Sesame Number 3717 provides data tables: 101 102 201 301 303 304 305 306 401
    int const Al3717 = 3717;

    // I also want to lookup the mean ion charge (EOS_Zfc_DT) which is found
    // in sesame table 601.  Add sesame number 23714 which provides data
    // tables: 101 102 201 601 602 603 604
    int const Al23714 = 23714;
    
    // Create a SesameTables object for Aluminum.
    rtt_cdi_eospac::SesameTables AlSt;
    
    // Assign matID Al3717 to enion lookups (used for Cvi) for AlSt.  We can
    // also assign these tables when the Eospac object is created (see example
    // below).
    
    // Also assign matID Al23714 for temperature-based electron thermal
    // conductivity (tconde).
    AlSt.Uic_DT( Al3717 ).Ktc_DT( Al23714 );
    
    // Verify that the assignments were made correctly.
    
    // Cvi (returnType=EOS_Uic_DT (=ES4enion)) should point to matID 3717.
    // The user should never need to access this function.  However Eospac.cc
    // does and we need to test this funcitonality.
    
    if ( AlSt.matID( EOS_Uic_DT ) != 3717 )
        FAILMSG("AlSt.matID(EOS_Uic_DT) points to the wrong matID.");
    
    // The temperature-based electorn thermal conductivity
    // (returnType=27=EOS_Ktc_DT) should point to matID 23714.  The user should
    // never need to access this function.  However Eospac.cc does and we need
    // to test this funcitonality.
    
    if ( AlSt.matID( EOS_Ktc_DT ) != 23714 )
        FAILMSG("AlSt.matID(27) points to the wrong matID.");
    
    
    // ----------------------- //
    // Create an Eospac object //
    // ----------------------- //
    
    // An Eospac object allows the user to access EoS information about a
    // material that has been constructed in a SesameTable object.  The
    // constructor for Eospac takes one argument: a SesameTables object.
    
    rtt_dsxx::SP< rtt_cdi_eospac::Eospac const > spEospac;
    
    // Try to instantiate the new Eospac object.  Simultaneously, we are
    // assigned material IDs to more SesameTable values.

    spEospac = new rtt_cdi_eospac::Eospac( 
        AlSt.Ue_DT( Al3717 ).Zfc_DT( Al23714 ) );

    if( spEospac )
    {
        PASSMSG("SP to new Eospac object created.");
    }
    else
    {
        FAILMSG("Unable to create SP to new Eospac object.");
        
        // if construction fails, there is no reason to continue testing...
        return;
    }

    // CAUTION!!!  CAUTION!!!  CAUTION!!!  CAUTION!!!  CAUTION!!!  CAUTION!!!
    
    // Adding this block breaks Eospac as currently implemented.  Effectively,
    // the destructor for spEospacAlt destroys all of the table handles for
    // libeospac.  This is a flaw in libeospac, but we may be forced to manage
    // it in cdi_eospac by using a Singleton Eospac object that uses reference
    // counting for each material+data tuple and calls eos_DestroyTables()
    // instead of eos_DestroyAll().  A redesign is also required to make this
    // package thread safe!
    
    // {   // Test alternate ctor method:
        
    //     // Alternatively, we can avoid carrying around the AlSt object.  We
    //     // can, instead, create a temporary version that is only used here in
    //     // the constructor of Eospac().

    //     rtt_dsxx::SP< rtt_cdi_eospac::Eospac const > spEospacAlt;
    //     spEospacAlt = new rtt_cdi_eospac::Eospac(
    //         rtt_cdi_eospac::SesameTables().Ue_DT( Al3717 ).Zfc_DT( Al23714
    //             ).Uic_DT( Al3717 ).Ktc_DT( Al23714 ) );
        
    //     if ( spEospacAlt )
    //         PASSMSG("SP to new Eospac object created (Alternate ctor).");
    //     else
    //         FAILMSG("Unable to create SP to new Eospac object (Alternate ctor).");
    // }
    
    // --------------------------- //
    // Test scalar access routines //
    // --------------------------- //
    
    double const K2keV = 1.0/1.1604412E+7; // keV/Kelvin
    
    // All of these tests request an EoS value given a single temperature and
    // a single density.
    
    // Retrieve an Electron internal energy value;
    
    double density     = 1.0;  // g/cm^3
    double temperature = 5800; // K
    // double density = 2.8; // g/cm^3
    // double temperature = 273; // K = 0 C
    temperature *= K2keV;      // convert temps to keV
    
    double refValue = 1.0507392783;  // kJ/g
    
    double specificElectronInternalEnergy =
        spEospac->getSpecificElectronInternalEnergy(
            temperature, density );
    double const tol(1.0e-10);
    
    if ( soft_equiv( specificElectronInternalEnergy, refValue, tol ) )
        PASSMSG("getSpecificElectronInternalEnergy() test passed.");
    else
    {
        std::cout.precision(12);
        std::cout
            << "refValue = " << refValue
            << "\ntabValue = " << specificElectronInternalEnergy << std::endl;
        FAILMSG("getSpecificElectronInternalEnergy() test failed.");
    }

    // Retrieve an electron heat capacity (= dE/dT)	    
    
    // old refValue = 3146.719924188898; // kJ/g/keV
    refValue = 4101.9991645; // kJ/g/keV
    
    double heatCapacity =
        spEospac->getElectronHeatCapacity( temperature,
        				   density );
    
    if ( soft_equiv(  heatCapacity, refValue, tol ) )
        PASSMSG("getElectronHeatCapacity() test passed.");
    else
        FAILMSG("getElectronHeatCapacity() test failed.");
    
    // Retrive an Ion Internal Energy
    
    refValue = 5.23391652028; // kJ/g
    
    double specificIonInternalEnergy = 
        spEospac->getSpecificIonInternalEnergy( 
            temperature, density );
    
    if ( soft_equiv( specificIonInternalEnergy, refValue, tol ) )
        PASSMSG("getSpecificIonInternalEnergy() test passed.");
    else
        FAILMSG("getSpecificIonInternalEnergy() test failed.");
    
    // Retrieve an ion based heat capacity
    
    refValue = 6748.7474603; // kJ/g/keV
    
    heatCapacity =
        spEospac->getIonHeatCapacity( temperature, density );
    
    if ( soft_equiv( heatCapacity, refValue, tol ) )
        PASSMSG("getIonHeatCapacity() test passed.");
    else
        FAILMSG("getIonHeatCapacity() test failed.");
    
    // Retrieve the number of free electrons per ion
    
    refValue = 12.8992087458; // electrons per ion
    
    double nfree =
        spEospac->getNumFreeElectronsPerIon( 
            temperature, density );
    
    if ( soft_equiv( nfree, refValue, tol ) )
        PASSMSG("getNumFreeElectronsPerIon() test passed.");
    else
        FAILMSG("getNumFreeElectronsPerIon() test failed.");
    
    // Retrieve the electron based thermal conductivity
    
    refValue =  1.38901721467e+29; // 1/s/cm
    
    double chie = 
        spEospac->getElectronThermalConductivity(
            temperature, density );
    
    if ( soft_equiv( chie, refValue, tol ) )
        PASSMSG("getElectronThermalConductivity() test passed.");
    else
        FAILMSG("getElectronThermalConductivity() test failed.");

    // --------------------------- //
    // Test vector access routines //
    // --------------------------- //
    
    // Set up simple temp and density vectors.  vtemp(i) will always be
    // associated with vdensities(i).  In this case both tuples have identical
    // data so that the returned results will also be identical.
    
    std::vector< double > vtemps(2);
    std::vector< double > vdensities(2);
    
    vtemps[0] = temperature;
    vtemps[1] = temperature;
    vdensities[0] = density;
    vdensities[1] = density;
    
    // Retrieve electron based heat capacities for each set of (density,
    // temperature) values.
    
    std::vector< double > vCve(2);
    vCve = spEospac->getElectronHeatCapacity( vtemps,
        				      vdensities );
    
    // Since the i=0 and i=1 tuples of density and temperature are identical
    // the two returned heat capacities should also soft_equiv.
    
    if ( soft_equiv( vCve[0], vCve[1], tol ) )
        PASSMSG("getElectronHeatCapacity() test passed for vector state values.");
    else
    {
        std::cout.precision(12);
        std::cout
            << "refValue = " << refValue
            << "\ntabValue = " << vCve[0] << std::endl;
        FAILMSG("getElectronHeatCapacity() test failed for vector state values.");
    }
    
    // This result should also match the scalar value
    // calculated above.
    
    heatCapacity = spEospac->getElectronHeatCapacity( temperature, density );
    
    if ( soft_equiv( vCve[0], heatCapacity, tol ) )
        PASSMSG("getElectronHeatCapacity() test passed for vector state values.");
    else
        FAILMSG("getElectronHeatCapacity() test failed for vector state values.");
    
} // end of runTest()

} // end of namespace rtt_cdi_eospac_test


//---------------------------------------------------------------------------//

int main(int argc, char *argv[])
{
    rtt_dsxx::ScalarUnitTest ut( argc, argv, rtt_dsxx::release );
    try
    {
	// >>> UNIT TESTS
        rtt_cdi_eospac_test::cdi_eospac_test(ut);
    }
    catch (rtt_cdi_eospac::EospacException &err )
    {
        std::cout << "EospacException ERROR: While testing " << argv[0] << ", "
                  << err.what() << std::endl;
        ut.numFails++;
    }
    catch (rtt_dsxx::assertion &err)
    {
        std::cout << "ERROR: While testing " << argv[0] << ", "
                  << err.what() << std::endl;
        ut.numFails++;
    }
    catch( ... )
    {
        std::cout << "ERROR: While testing " << argv[0] << ", " 
                  << "An unknown exception was thrown on processor "
                  << std::endl;
        ut.numFails++;
    }
    return ut.numFails;
}   

//---------------------------------------------------------------------------//
// end of tEospac.cc
//---------------------------------------------------------------------------//
