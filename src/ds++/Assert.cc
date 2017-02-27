//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   ds++/Assert.cc
 * \brief  Helper functions for the Assert facility.
 * \note   Copyright (C) 2016-2017 Los Alamos National Security, LLC.
 *         All rights reserved.
 * \version $Id$
 */
//---------------------------------------------------------------------------//

#include "Assert.hh"
#include "SystemCall.hh" // draco_gethostname(), draco_getpid()
#include <iostream>
#include <sstream>

namespace rtt_dsxx {

//===========================================================================//
// ASSERTION CLASS MEMBERS
//===========================================================================//

/*!
 * Build the error string (private member function).
 * \param cond Condition (test) that failed.
 * \param file The name of the file where the assertion was tested.
 * \param line The line number in the file where the assertion was tested.
 * \retval myMessage A string that contains the failed condition, the file and
 * the line number of the error.
 */
std::string assertion::build_message(std::string const &cond,
                                     std::string const &file, int const line) {
  std::ostringstream myMessage;
  myMessage << "Assertion: " << cond << ", failed in " << file << ", line "
            << line << "." << std::endl;
  return myMessage.str();
}

/*
 * Leave this definition in the .cc file!  This is a work-around for building
 * on Cielo.  Without this defintion in the .cc file, clubimc will not build
 * because it cannot resolve this symbol: undefined reference to
 * `__T_Q2_8rtt_dsxx9assertion'
 */
assertion::~assertion() throw() { /* empty */
}

//===========================================================================//
// FREE FUNCTIONS
//===========================================================================//
/*!
 * \brief Throw a rtt_dsxx::assertion for Require, Check, Ensure macros.
 * \return Throws an assertion.
 * \note We do not provide unit tests for functions whose purpose is to throw
 * or exit.
 */
void toss_cookies(std::string const &cond, std::string const &file,
                  int const line) {
  throw assertion(cond, file, line);
}

/*!
 * \brief Throw a rtt_dsxx::assertion for Require, Check, Ensure macros.
 * \return Throws an assertion.
 */
void toss_cookies_ptr(char const *const cond, char const *const file,
                      int const line) {
  throw assertion(cond, file, line);
}

/*!
 * \brief Throw a rtt_dsxx::assertion for Require, Check, Ensure macros.
 *
 * This version defers the branch, and so is preferred for testing code coverage by the test
 * suite. However, it has the performance penalty of an extra function call.
 *
 * \return Throws an assertion.
 */
void check_cookies(bool const cond, char const *const cond_text,
                   char const *const file, int const line) {
  if (!cond)
    throw assertion(cond_text, file, line);
}

/*!
 * \brief Print error message generated by Require, Check, Ensure macros.
 * \return void
 */
void show_cookies(std::string const &cond, std::string const &file,
                  int const line) {
  std::cerr << assertion::build_message(cond, file, line) << std::endl;
}

//---------------------------------------------------------------------------//
/*!
 * \brief Throw a rtt_dsxx::assertion for Insist macros.
 */
void insist(std::string const &cond, std::string const &msg,
            std::string const &file, int const line) {
  std::ostringstream myMessage;
  myMessage << "Insist: " << cond << ", failed in " << file << ", line " << line
            << "." << std::endl
            << "The following message was provided:" << std::endl
            << "\"" << msg << "\"" << std::endl;
  throw assertion(myMessage.str());
}

//---------------------------------------------------------------------------//
/*!
 * \brief Throw a rtt_dsxx::assertion for Insist_ptr macros.
 *
 * Having a (non-inlined) version that takes pointers prevents the compiler
 * from having to construct std::strings from the pointers each time.  This
 * is particularly important for things like rtt_dsxx::SP::operator->, that
 * (a) have an insist in them, (b) don't need complicated strings and (c) are
 * called frequently.
 */
void insist_ptr(char const *const cond, char const *const msg,
                char const *const file, int const line) {
  // Call the other insist for consistency
  insist(cond, msg, file, line);
}

//---------------------------------------------------------------------------//
/*!
 * \brief Conditionally throw a rtt_dsxx::assertion for Insist macros.
 */
void check_insist(bool const cond, char const *const condstr,
                  std::string const &msg, char const *const file,
                  int const line) {
  if (!cond) {
    std::ostringstream myMessage;
    myMessage << "Insist: " << condstr << ", failed in " << file << ", line "
              << line << "." << std::endl
              << "The following message was provided:" << std::endl
              << "\"" << msg << "\"" << std::endl;
    throw assertion(myMessage.str());
  }
}

//---------------------------------------------------------------------------//
/*!
 * \brief Conditionally throw a rtt_dsxx::assertion for Insist_ptr macros.
 *
 * Having a (non-inlined) version that takes pointers prevents the compiler
 * from having to construct std::strings from the pointers each time.  This
 * is particularly important for things like rtt_dsxx::SP::operator->, that
 * (a) have an insist in them, (b) don't need complicated strings and (c) are
 * called frequently.
 */
void check_insist_ptr(bool const cond, char const *const condstr,
                      char const *const msg, char const *const file,
                      int const line) {
  // Call the other insist for consistency
  if (!cond)
    check_insist(cond, condstr, msg, file, line);
}

//---------------------------------------------------------------------------//
/*! \brief Add hostname and pid to error messages.
 *
 * Several of the errors that might be reported by DACS_Device could be
 * specific to one or a few nodes (filesystems not mounted, etc.).
 * verbose_error adds the hostname and pid to error messages.
 */
std::string verbose_error(std::string const &message) {
  int pid = draco_getpid();
  std::ostringstream errstr;
  errstr << "Host " << draco_gethostname() << ", PID ";
  if (pid < 0)
    errstr << "(unknown)";
  else
    errstr << pid;
  errstr << ": " << message;
  return errstr.str();
} // verbose_error

} // end of rtt_dsxx

//---------------------------------------------------------------------------//
// end of Assert.cc
//---------------------------------------------------------------------------//
