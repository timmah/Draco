//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   cdi_analytic/Analytic_Models.hh
 * \author Thomas M. Evans
 * \date   Wed Aug 29 16:46:52 2001
 * \brief  Analytic_Model definitions
 * \note   Copyright (C) 2001-2010 Los Alamos National Security, LLC.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef __cdi_analytic_Analytic_Models_hh__
#define __cdi_analytic_Analytic_Models_hh__

#include "ds++/Assert.hh"
#include <cmath>
#include <vector>

namespace rtt_cdi_analytic
{

//===========================================================================//
// ENUMERATIONS
//===========================================================================//

/*!
 * \brief Enumeration describing the opacity models that are available.
 *
 * Only opacity models that have been registered here can be unpacked by the
 * Analytic_Gray_Opacity and Analytic_Multigroup_Opacity classes.  The
 * enumeration names should be the same as the derived class names.
 */
enum Opacity_Models
{
    CONSTANT_ANALYTIC_OPACITY_MODEL,
    POLYNOMIAL_ANALYTIC_OPACITY_MODEL
};

/*!
 * \brief Enumeration describing the eos  models that are available.
 *
 * Only EoS models that have been registered here can be unpacked by the
 * Analytic_EoS classes. The enumeration names should be the same as the
 * derived class names.
 */
enum EoS_Models
{
    POLYNOMIAL_SPECIFIC_HEAT_ANALYTIC_EOS_MODEL
};

//===========================================================================//
/*!
 * \class Analytic_Opacity_Model
 * \brief Analytic_Opacity_Model base class.
 *
 * This is a base class that defines the interface given to
 * Analytic_Gray_Opacity or Analytic_MultiGroup_Opacity constructors.  The
 * user can define any derived model class that will work with these analtyic
 * opacity generation classes as long as it contains the following function:
 * (declared pure virtual in this class).
 *
 * \arg double calculate_opacity(double T, double rho)
 *
 * To enable packing functionality, the class must be registered in the
 * Opacity_Models enumeration.  Also, it must contain the following pure
 * virtual function:
 *
 * \arg vector<char> pack() const;
 *
 * This class is a pure virtual base class. 
 *
 * The returned opacity should have units of cm^2/g.
 *
 */
//===========================================================================//

class Analytic_Opacity_Model
{
  public:
    // Typedefs.
    typedef std::vector<char>   sf_char;
    typedef std::vector<double> sf_double;

  public:
    //! Virtual destructor for proper inheritance destruction.
    virtual ~Analytic_Opacity_Model() {/*...*/}

    //! Interface for derived analytic opacity models.
    virtual double calculate_opacity(double T, double rho) const = 0;

    //! Interface for derived analytic opacity models.
    virtual double calculate_opacity(double T, double rho, double /*nu*/) const
    {
        return calculate_opacity(T, rho); 
    };

    //! Return parameters.
    virtual sf_double get_parameters() const = 0;

    //! Return a char string of packed data.
    virtual sf_char pack() const = 0;
};

//---------------------------------------------------------------------------//
/*!
 * \class Constant_Analytic_Opacity_Model
 * \brief Derived Analytic_Opacity_Model class that defines a constant
 *  opacity.
 *
 * The opacity is defined:
 *
 * \arg opacity = a
 *
 * where the coefficient has the following units:
 *
 * \arg a = [cm^2/g]
 *
 */
class Constant_Analytic_Opacity_Model : public Analytic_Opacity_Model
{
  private:
    // Constant opacity.
    double sigma;

  public:
    //! Constructor, sig has units of cm^2/g.
    explicit Constant_Analytic_Opacity_Model(double sig) 
	: sigma(sig) 
    { 
	Require (sigma >= 0.0); 
    }

    //! Constructor for packed state.
    explicit Constant_Analytic_Opacity_Model(const sf_char &packed);

    //! Calculate the opacity in units of cm^2/g.
    double calculate_opacity(double /*T*/, double /*rho*/) const
    {
	return sigma;
    }

    //! Calculate the opacity in units of cm^2/g.
    double calculate_opacity(double /*T*/, double /*rho*/, double /*nu*/) const
    {
        return sigma;
    }

    //! Return the model parameters.
    sf_double get_parameters() const;

    //! Pack up the class for persistence.
    sf_char pack() const;
};

//---------------------------------------------------------------------------//
/*!
 * \class Polynomial_Analytic_Opacity_Model
 * \brief Derived Analytic_Opacity_Model class that defines a polymomial
 * function for the opacity.
 *
 * The opacity is defined:
 *
 * \arg opacity = (a + b * T^c * nu^e) * rho^d
 *
 * where the coefficients have the following units:
 *
 * \arg a = [cm^2/g * (cm^3/g)^d]
 * \arg b = [keV^(-c) * cm^2/g * (cm^3/g)^d]
 *
 */
class Polynomial_Analytic_Opacity_Model : public Analytic_Opacity_Model
{
  private: 
    // Coefficients
    double a;  // constant [cm^2/g * (cm^3/g)^d]
    double b;  // temperature multiplier [keV^(-c) * cm^2/g * (cm^3/g)^d]
    double c;  // temperature power
    double d;  // density power
    double e;  // frequency power

  public:
    /*!
     * \brief Constructor.
     * \param a_ constant [cm^2/g (cm^3/g)^d]
     * \param b_ temperature multiplier [keV^(-c) cm^2/g (cm^3/g)^d]
     * \param c_ temperature power
     * \param d_ density power
     * \param e_ frequency power
     */
    Polynomial_Analytic_Opacity_Model(double a_, double b_, double c_,
				      double d_, double e_=0)
	: a(a_), b(b_), c(c_), d(d_), e(e_)
    {
	/*...*/
    }

    //! Constructor for packed state.
    explicit Polynomial_Analytic_Opacity_Model(const sf_char &packed);

    //! Calculate the opacity in units of cm^2/g
    double calculate_opacity(double T, double rho, double nu) const
    {
	Require (c < 0.0 ? T > 0.0 : T >= 0.0);
	Require (rho >= 0.0);
	Require (nu >= 0.0);

	double T_power   = std::pow(T,   c);
	double rho_power = std::pow(rho, d);
	double nu_power  = std::pow(nu,  e);

	double opacity   = (a + b * T_power * nu_power) * rho_power;

	Ensure (opacity >= 0.0);
	return opacity;
    }

    //! Calculate the opacity in units of cm^2/g
    double calculate_opacity(double T, double rho) const
    {
	Require (c < 0.0 ? T > 0.0 : T >= 0.0);
	Require (rho >= 0.0);

	double T_power   = std::pow(T,   c);
	double rho_power = std::pow(rho, d);

	double opacity   = (a + b * T_power) * rho_power; 

	Ensure (opacity >= 0.0);
	return opacity;
    }

    //! Return the model parameters.
    sf_double get_parameters() const;

    //! Pack up the class for persistence.
    sf_char pack() const;
};

//===========================================================================//
/*!
 * \class Analytic_EoS_Model
 * \brief Analytic_EoS_Model base class.
 *
 * This is a base class that defines the interface given to Analytic_EoS
 * constructors.  The user can define any derived Analytic_EoS class to give
 * to an analytic EoS class as long as it contains the following functions:
 * (declared virtual in this class).
 *
 * \arg double calculate_electron_internal_energy(double T, double rho)
 * \arg double calculate_electron_heat_capacity(double T, double rho)
 * \arg double calculate_ion_internal_energy(double T, double rho)
 * \arg double calculate_ion_heat_capacity(double T, double rho)
 * \arg double calculate_num_free_elec_per_ion(double T, double rho)
 * \arg double calculate_elec_thermal_conductivity(double T, double rho)
 *
 * The units for each output are:
 * 
 * \arg electron internal energy      = kJ/g
 * \arg electron heat capacity        = kJ/g/keV
 * \arg ion internal energy           = kJ/g
 * \arg ion heat capacity             = kJ/g/keV
 * \arg electron thermal conductivity = /s/cm
 * 
 * These units correspond to the units defined by the rtt_cdi::EoS base
 * class. 
 *
 * To enable packing functionality, the class must be registered in the
 * EoS_Models enumeration.  Also, it must contain the following pure
 * virtual function:
 *
 * \arg vector<char> pack() const;
 *
 * This class is a pure virtual base class. 
 */
//===========================================================================//

class Analytic_EoS_Model
{
  public:
    // Typedefs.
    typedef std::vector<char>   sf_char;
    typedef std::vector<double> sf_double;

  public:
    //! Virtual destructor for proper inheritance destruction.
    virtual ~Analytic_EoS_Model() {/*...*/}

    //! Calculate the electron internal energy
    virtual double calculate_electron_internal_energy(double T, double rho) 
	const = 0;

    //! Calculate the electron heat capacity.
    virtual double calculate_electron_heat_capacity(double T, double rho) 
	const = 0;

    //! Calculate the ion internal energy.
    virtual double calculate_ion_internal_energy(double T, double rho)
	const = 0;

    //! Calculate the ion heat capacity.
    virtual double calculate_ion_heat_capacity(double T, double rho)
	const = 0;
    
    //! Calculate the number of electrons per ion.
    virtual double calculate_num_free_elec_per_ion(double T, double rho)
	const = 0;

    //! Calculate the electron thermal conductivity.
    virtual double calculate_elec_thermal_conductivity(double T, double rho)
	const = 0;

    //! Return the model parameters.
    virtual sf_double get_parameters() const = 0;

    //! Return a char string of packed data.
    virtual sf_char pack() const = 0;
};

//---------------------------------------------------------------------------//
/*!
 * \class Polynomial_Specific_Heat_Analytic_EoS_Model
 * \brief Derived Analytic_EoS_Model class that defines polymomial
 * functions for EoS specific heat data.
 *
 * The electron and ion specific heats are defined:
 * 
 * \arg elec specific heat = a + bT^c
 * \arg ion specific heat  = d + eT^f
 *
 * where the coefficients have the following units:
 *
 * \arg a,d = [kJ/g/keV]
 * \arg b,e = [kJ/g/keV^(c+1,f+1)]
 *
 * The additional data that is required by the Analytic_EoS_Model base class
 * is set to zero by default. The Polynomial_Specific_Heat_Analytic_EoS_Model
 * class is intended to be used by radiation-only packages for testing and
 * verification purposes.  More complex analytic EoS models can be easily
 * defined if they are required; however, radiation-only packages (without
 * Compton scatter) only require specfic heat data.
 * 
 */
class Polynomial_Specific_Heat_Analytic_EoS_Model : public Analytic_EoS_Model
{
  private:
    // Coefficients.   
    double a; // electron Cv constant [kJ/g/keV]
    double b; // electron Cv temperature multiplier [kJ/g/keV^(c+1)]
    double c; // electron Cv temperature power
    double d; // ion Cv constant [kJ/g/keV]
    double e; // ion Cv temperature multiplier [kJ/g/keV^(c+1)]
    double f; // ion Cv temperature power

  public:
    /*!
     * \brief Constructor.
     * \param a_ electron Cv constant [kJ/g/keV]
     * \param b_ electron Cv temperature multiplier [kJ/g/keV^(c+1)]
     * \param c_ electron Cv temperature power
     * \param d_ ion Cv constant [kJ/g/keV]
     * \param e_ ion Cv temperature multiplier [kJ/g/keV^(c+1)]
     * \param f_ ion Cv temperature power
     */
    Polynomial_Specific_Heat_Analytic_EoS_Model(double a_, double b_,
						double c_, double d_, 
						double e_, double f_)
	: a(a_), b(b_), c(c_), d(d_), e(e_), f(f_)
    {
	Insist(c>=0.0, "The Cve temperature exponent must be nonnegative");
	Insist(f>=0.0, "The Cvi temperature exponent must be nonnegative");

	/*...*/
    }

    //! Constructor for packed state.
    explicit Polynomial_Specific_Heat_Analytic_EoS_Model(const sf_char &);

    //! Calculate the electron heat capacity in kJ/g/keV.
    double calculate_electron_heat_capacity(double T, double Remember(rho) ) const
    {
	Require (T >= 0.0);
	Require (rho >= 0.0);

	double T_power = std::pow(T, c);
	double Cv      = (a + b * T_power);

	Ensure (Cv >= 0.0);
	return Cv;
    }

    //! Calculate the ion heat capacity in kJ/g/keV.
    double calculate_ion_heat_capacity(double T, double Remember(rho) ) const
    {
	Require (T >= 0.0);
	Require (rho >= 0.0);

	double T_power = std::pow(T, f);
	double Cv      = (d + e * T_power);

	Ensure (Cv >= 0.0);
	return Cv;
    }


    /*! Calculate the electron specific internal energy.
     *
     * This is done by integrating the specific heat capacity at constant
     * density from T=0 to the specified temperature.
     *
     * \param T 
     * Temperature (keV) for which the specific internal energy is to be
     * evaluated. 
     * \param rho
     * Density (g/cm^3) for which the specific internal energy is to be
     * evaluated. This parameter is not actually used.
     *
     * \return Electron specific internal energy (kJ/g)
     *
     * \pre \c T>=0
     * \pre \c rho>=0
     *
     * \post \c U>=0
     */
    double calculate_electron_internal_energy(double T, double Remember(rho) ) const
    { 
	Require (T >= 0.0);
	Require (rho >= 0.0);

	Check(c>=0.0);
	double T_power = std::pow(T, c+1.0);
	double U       = a*T + b*T_power/(c+1.0);
	
	Ensure(U >= 0.0);
	return U;
    }

    /*! Calculate the ion specific internal energy.
     *
     * This is done by integrating the specific heat capacity at constant
     * density from T=0 to the specified temperature.
     *
     * \param T 
     * Temperature (keV) for which the specific internal energy is to be
     * evaluated. 
     * \param rho
     * Density (g/cm^3) for which the specific internal energy is to be
     * evaluated. This parameter is not actually used.
     *
     * \return Ion specific internal energy (kJ/g)
     *
     * \pre \c T>=0
     * \pre \c rho>=0
     *
     * \post \c U>=0
     */
    double calculate_ion_internal_energy(double T, double Remember(rho) ) const
    {
	Require (T >= 0.0);
	Require (rho >= 0.0);

	Check(f>=0.0);
	double T_power = std::pow(T, f+1.0);
	double U       = d*T + e*T_power/(f+1.0);

	Ensure (U >= 0.0);
	return U;
    }
    
    //! Return 0 for the number of electrons per ion.
    double calculate_num_free_elec_per_ion(double /*T*/, double /*rho*/) const
    { return 0.0; }

    //! Return 0 for the electron thermal conductivity.
    double calculate_elec_thermal_conductivity(double /*T*/, double /*rho*/) const
    { return 0.0; }

    //! Return the model parameters.
    sf_double get_parameters() const;

    //! Pack up the class for persistence.
    sf_char pack() const;
};

} // end namespace rtt_cdi_analytic

#endif  // __cdi_analytic_Analytic_Models_hh__

//---------------------------------------------------------------------------//
// end of cdi_analytic/Analytic_Models.hh
//---------------------------------------------------------------------------//
