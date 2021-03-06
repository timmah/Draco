//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   cdi/OdfmgOpacity.hh
 * \author Kelly Thompson
 * \date   Mon Jan 8 14:58:55 2001
 * \brief  OdfmgOpacity class header file (an abstract class)
 * \note   Copyright (C) 2016-2017 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef __cdi_OdfmgOpacity_hh__
#define __cdi_OdfmgOpacity_hh__

#include "OpacityCommon.hh"
#include "ds++/config.h"
#include <string>
#include <vector>

namespace rtt_cdi {

//========================================================================
/*!
 * \class OdfmgOpacity
 *
 * \brief This is a pure virtual class that defines a standard
 * interface for all derived OdfmgOpacity objects.
 *
 * Any derived OdfmgOpacity object must provide as a minimum the
 * functionality outlined in this routine.  This functionality includes
 * access to the data grid and the ability to return interpolated opacity
 * values.
 */
/*!
 * \example cdi/test/tDummyOpacity.cc
 * \example cdi/test/tCDI.cc
 */
//========================================================================

class DLL_PUBLIC_cdi OdfmgOpacity {
  // DATA

  // There is no data for a pure virtual object.  This class
  // provides an interface and does not preserve state.

public:
  // ---------- //
  // Destructor //
  // ---------- //

  /*!
   * \brief Default Opacity() destructor.
   *
   * This is required to correctly release memory when any
   * object derived from OdfmgOpacity is destroyed.
   */
  virtual ~OdfmgOpacity(){/*empty*/};

  // --------- //
  // Accessors //
  // --------- //

  /*!
   * \brief Opacity accessor that returns a 2-D vector of opacities (
   *     groups * bands ) that correspond to the
   *     provided temperature and density. 
   *
   * \param targetTemperature The temperature value for which
   *     these opacity values are being requested (keV).
   * \param targetDensity The density value for which these opacity 
   *     values are being requested (g/cm^3)
   * \return A vector of opacities (a single opacity for each group).
   */
  virtual std::vector<std::vector<double>>
  getOpacity(double targetTemperature, double targetDensity) const = 0;

  /*!
   * \brief Opacity accessor that returns a vector of multigroupband
   *     opacity 2-D vectors that correspond to the provided vector of
   *     temperatures and a single density value.
   *
   * \param targetTemperature A vector of temperature values for
   *     which opacity values are being requested (keV).
   *
   * \param targetDensity The density value for which an opacity 
   *     value is being requested (g/cm^3).
   *
   * \return A vector of multiband, multigroup opacity vectors (cm^2/g).
   */
  virtual std::vector<std::vector<std::vector<double>>>
  getOpacity(const std::vector<double> &targetTemperature,
             double targetDensity) const = 0;

  /*!
   * \brief Opacity accessor that returns a vector of multigroupband
   *     opacity 2-D vectors that correspond to the provided vector of
   *     densities and a single temperature value.
   *
   * \param targetTemperature The temperature value for which an 
   *     opacity value is being requested (keV).
   * \param targetDensity A vector of density values for which
   *     opacity values are being requested (g/cm^3).
   * \return A vector of multigroup opacity vectors (cm^2/g).
   */
  virtual std::vector<std::vector<std::vector<double>>>
  getOpacity(double targetTemperature,
             const std::vector<double> &targetDensity) const = 0;

  /*!
   * \brief Query whether the data is in tables or functional form.
   */
  virtual bool data_in_tabular_form() const = 0;

  /*!
   * \brief Query to determine the reaction model.
   */
  virtual rtt_cdi::Reaction getReactionType() const = 0;

  /*!
   * \brief Query to determine the physics model.
   */
  virtual rtt_cdi::Model getModelType() const = 0;

  /*!
   * \brief Returns a string that describes the EnergyPolicy.
   *     Currently this will return either "mg" or "gray."
   */
  virtual std::string getEnergyPolicyDescriptor() const = 0;

  /*!
   * \brief Returns a "plain English" description of the opacity
   *     data that this class references. (e.g. "Gray Rosseland
   *     Scattering".) 

   */
  virtual std::string getDataDescriptor() const = 0;

  /*!
   * \brief Returns the name of the associated data file (if any).
   */
  virtual std::string getDataFilename() const = 0;

  /*!
   * \brief Returns a vector of temperatures that define the cached
   *     opacity data table.
   */
  virtual std::vector<double> getTemperatureGrid() const = 0;

  /*!
   * \brief Returns a vector of densities that define the cached
   *     opacity data table.
   */
  virtual std::vector<double> getDensityGrid() const = 0;

  /*!
   * \brief Returns a vector of energy values (keV) that define the
   *     energy boundaries of the cached multigroup opacity data
   *     table.  
   */
  virtual std::vector<double> getGroupBoundaries() const = 0;

  /*!
   * \brief Returns the size of the temperature grid.
   */
  virtual size_t getNumTemperatures() const = 0;

  /*! 
   * \brief Returns the size of the density grid.
   */
  virtual size_t getNumDensities() const = 0;

  /*!
   * \brief Returns the number of group boundaries found in the
   *     current multigroup data set.
   */
  virtual size_t getNumGroupBoundaries() const = 0;

  /*!
   * \brief Returns the number of energy groups 
   * ( getNumGroupBoundaries() - 1 ).
   */
  virtual size_t getNumGroups() const = 0;

  /*!
   * \brief Returns a vector of points along the cumulative opacity 
   * 		distribution that mark the fraction of each band
   */
  virtual std::vector<double> getBandBoundaries() const = 0;

  /*!
   * \brief Returns the number of band boundaries found in the
   *     current multigroup data set.
   */
  virtual size_t getNumBandBoundaries() const = 0;

  /*!
   * \brief Returns the number of opacity bands 
   */
  virtual size_t getNumBands() const = 0;

  /*!
   * \brief Interface for packing a derived OdfmgOpacity object.
   *
   * Note, the user hands the return value from this function to a derived
   * OdfmgOpacity constructor.  Thus, even though one can pack a
   * OdfmgOpacity through a base class pointer, the client must know
   * the derived type when unpacking.
   */
  virtual std::vector<char> pack() const = 0;

  /*!
   * \brief Returns the general opacity model type (Analytic or Gandolf),
   * defined in OpacityCommon.hh
   */
  virtual rtt_cdi::OpacityModelType getOpacityModelType() const = 0;

}; // end of class OdfmgOpacity

} // end namespace rtt_cdi

#endif // __cdi_OdfmgOpacity_hh__

//---------------------------------------------------------------------------//
// end of cdi/OdfmgOpacity.hh
//---------------------------------------------------------------------------//
