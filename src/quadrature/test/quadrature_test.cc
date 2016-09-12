//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   quadrature/quadrature_test.cc
 * \author Kent G. Budge
 * \brief  Define class quadrature_test
 * \note   Copyright (C) 2016 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//

#include <iomanip>
#include <iostream>
#include <numeric>

#include "quadrature_test.hh"

#include "parser/String_Token_Stream.hh"
#include "parser/utilities.hh"

namespace rtt_quadrature {
using namespace std;
using namespace rtt_parser;

//----------------------------------------------------------------------------//
void test_either(UnitTest &ut, SP<Ordinate_Space> const &ordinate_space,
                 Quadrature &quadrature, unsigned const expansion_order) {
  vector<Ordinate> const &ordinates = ordinate_space->ordinates();
  unsigned const number_of_ordinates = ordinates.size();

  rtt_mesh_element::Geometry const geometry = ordinate_space->geometry();
  unsigned const dimension = ordinate_space->dimension();

  if (ordinate_space->moments()[0] == Moment(0, 0)) {
    ut.passes("first moment is correct");
  } else {
    ut.failure("first moment is NOT correct");
  }

  if (number_of_ordinates == ordinate_space->alpha().size()) {
    ut.passes("alpha size is correct");
  } else {
    ut.failure("alpha size is NOT correct");
  }

  if (ordinate_space->ordinates().size() == ordinate_space->tau().size()) {
    ut.passes("tau size is correct");
  } else {
    ut.failure("tau size is NOT correct");
  }

  if (ordinate_space->expansion_order() == static_cast<int>(expansion_order)) {
    ut.passes("expansion order is correct");
  } else {
    ut.failure("expansion_order is NOT correct");
  }

  vector<unsigned> const &first_angles = ordinate_space->first_angles();
  unsigned const number_of_levels = quadrature.number_of_levels();

  if (geometry == rtt_mesh_element::SPHERICAL) {
    if (first_angles.size() == 1) {
      ut.passes("first angles is correct");
    } else {
      ut.failure("first angles is NOT correct");
    }

    if (ordinate_space->bookkeeping_coefficient(number_of_ordinates - 1) <=
        0.0) {
      ut.failure("bookkeeping coefficient is NOT plausible");
    }

    ordinate_space->psi_coefficient(number_of_ordinates - 1);
    ordinate_space->source_coefficient(number_of_ordinates - 1);
    // check that throws no exception
  } else if (geometry == rtt_mesh_element::AXISYMMETRIC) {
    if ((dimension > 1 && first_angles.size() == number_of_levels) ||
        (dimension == 1 && 2 * first_angles.size() == number_of_levels)) {
      ut.passes("first angles is correct");
    } else {
      ut.failure("first angles is NOT correct");
    }

    if (ordinate_space->bookkeeping_coefficient(number_of_ordinates - 1) <=
        0.0) {
      ut.failure("bookkeeping coefficient is NOT plausible");
    }

    ordinate_space->psi_coefficient(number_of_ordinates - 1);
    ordinate_space->source_coefficient(number_of_ordinates - 1);
    // check that throws no exception

    vector<unsigned> const &levels = ordinate_space->levels();
    if (levels.size() == number_of_ordinates) {
      ut.passes("levels size is correct");
    } else {
      ut.failure("levels size is NOT correct");
    }
    for (unsigned i = 0; i < number_of_ordinates; ++i) {
      if (levels[i] >= number_of_levels) {
        ut.failure("levels is NOT in bounds");
        return;
      }
    }

    vector<unsigned> const &moments_per_order =
        ordinate_space->moments_per_order();

    if (moments_per_order.size() == expansion_order + 1) {
      ut.passes("moments_per_order size is correct");
    } else {
      ut.failure("moments_per_order size is NOT correct");
    }
    for (unsigned i = 0; i <= expansion_order; ++i) {
      if ((dimension == 1 && moments_per_order[i] != i / 2 + 1) ||
          (dimension > 1 && moments_per_order[i] != i + 1)) {
        ut.failure("moments_per_order is NOT correct");
        return;
      }
    }

    if ((dimension == 1 &&
         number_of_levels == 2 * ordinate_space->number_of_levels()) ||
        (dimension > 1 &&
         number_of_levels == ordinate_space->number_of_levels())) {
      ut.passes("number of levels is consistent");
    } else {
      ut.failure("number of levels is NOT consistent");
    }
  } else {
    if (ordinate_space->first_angles().size() == 0) {
      ut.passes("first angles is correct");
    } else {
      ut.failure("first angles is NOT correct");
    }
  }

  vector<unsigned> const &reflect_mu = ordinate_space->reflect_mu();
  if (reflect_mu.size() == number_of_ordinates) {
    ut.passes("reflect_mu is correct size");
  } else {
    ut.failure("reflect_mu is NOT correct size");
  }
  for (unsigned i = 0; i < number_of_ordinates; ++i) {
    if (reflect_mu[i] >= number_of_ordinates) {
      ut.failure("reflect_mu is out of bounds");
      return;
    }
    if (ordinates[i].wt() != 0.0 && reflect_mu[reflect_mu[i]] != i) {
      ut.failure("reflect_mu is inconsistent");
      return;
    }
  }

  if (dimension > 1) {
    vector<unsigned> const &reflect_eta = ordinate_space->reflect_eta();
    if (reflect_eta.size() == number_of_ordinates) {
      ut.passes("reflect_eta is correct size");
    } else {
      ut.failure("reflect_eta is NOT correct size");
    }
    for (unsigned i = 0; i < number_of_ordinates; ++i) {
      if (reflect_eta[i] >= number_of_ordinates) {
        ut.failure("reflect_eta is out of bounds");
        return;
      }
      if (reflect_eta[reflect_eta[i]] != i) {
        ut.failure("reflect_eta is inconsistent");
        return;
      }
    }

    if (dimension > 2) {
      vector<unsigned> const &reflect_xi = ordinate_space->reflect_xi();
      if (reflect_xi.size() == number_of_ordinates) {
        ut.passes("reflect_xi is correct size");
      } else {
        ut.failure("reflect_xi is NOT correct size");
      }
      for (unsigned i = 0; i < number_of_ordinates; ++i) {
        if (reflect_xi[i] >= number_of_ordinates) {
          ut.failure("reflect_xi is out of bounds");
          return;
        }
        if (reflect_xi[reflect_xi[i]] != i) {
          ut.failure("reflect_xi is inconsistent");
          return;
        }
      }
    }
  }

  // See if count matches class

  unsigned L = quadrature.number_of_levels();
  unsigned N = ordinate_space->ordinates().size();
  switch (quadrature.quadrature_class()) {
  case TRIANGLE_QUADRATURE:
    if (dimension == 1) {
      if (geometry == rtt_mesh_element::CARTESIAN && L != N)
        ut.failure("ordinate count is wrong for triangular quadrature");
    } else if (dimension == 3) {
      if (L * (L + 2) != N)
        ut.failure("ordinate count is wrong for triangular quadrature");
    }
    break;

  case SQUARE_QUADRATURE:
    if (dimension == 3) {
      if (2 * L * L != N) {
        ut.failure("ordinate count is wrong for square quadrature");
      }
    }
    break;

  default:
    if (dimension == 3) {
      if (4 * L > N) {
        ut.failure("ordinate count is too small for level count");
      }
    }
    break;
  }

  // Test that mean and flux are correct

  {
    vector<Ordinate> const &ordinates = ordinate_space->ordinates();
    unsigned const N = ordinates.size();
    double J = 0.0;
    double Fx = 0.0, Fy = 0.0, Fz = 0.0;
    double Fx2 = 0.0, Fy2 = 0.0, Fz2 = 0.0;
    double const MAGIC = 2.32; // avoid numerical coincidences

    for (unsigned i = 0; i < N; ++i) {
      J += MAGIC * ordinates[i].wt();
      Fx += MAGIC * ordinates[i].mu() * ordinates[i].wt();
      Fx2 += MAGIC * ordinates[i].mu() * ordinates[i].mu() * ordinates[i].wt();
      Fy += MAGIC * ordinates[i].eta() * ordinates[i].wt();
      Fy2 +=
          MAGIC * ordinates[i].eta() * ordinates[i].eta() * ordinates[i].wt();
      Fz += MAGIC * ordinates[i].xi() * ordinates[i].wt();
      Fz2 += MAGIC * ordinates[i].xi() * ordinates[i].xi() * ordinates[i].wt();
    }

    if (soft_equiv(J, MAGIC)) {
      ut.passes("J okay");
    } else {
      ut.failure("J NOT okay");
    }
    if (soft_equiv(Fx, 0.0)) {
      ut.passes("Fx okay");
    } else {
      std::cout << "Fx = " << Fx << std::endl;
      ut.failure("Fx NOT okay");
    }
    if (soft_equiv(Fx2, MAGIC / 3.0)) {
      ut.passes("Fx2 okay");
    } else {
      cout << "Fx2 = " << Fx2 << ", expected " << (MAGIC / 3.0) << endl;
      ut.failure("Fx2 NOT okay");
    }
    if (dimension > 1) {
      if (soft_equiv(Fy, 0.0)) {
        ut.passes("Fy okay");
      } else {
        ut.failure("Fy NOT okay");
      }
      if (soft_equiv(Fy2, MAGIC / 3.0)) {
        ut.passes("Fy2 okay");
      } else {
        cout << "Fy2 = " << Fz2 << ", expected " << (MAGIC / 3.0) << endl;
        ut.failure("Fy2 NOT okay");
      }
    }
    if (dimension > 2) {
      if (soft_equiv(Fz, 0.0)) {
        ut.passes("Fz okay");
      } else {
        ut.failure("Fz NOT okay");
      }
      if (soft_equiv(Fz2, MAGIC / 3.0)) {
        ut.passes("Fz2 okay");
      } else {
        cout << "Fz2 = " << Fz2 << ", expected " << (MAGIC / 3.0) << endl;
        ut.failure("Fz2 NOT okay");
      }
    }

    // Look at the moment to discrete and discrete to moment operator

    vector<double> M = ordinate_space->M();
    vector<double> D = ordinate_space->D();

    unsigned number_of_moments = ordinate_space->number_of_moments();

    if (M.size() == number_of_moments * number_of_ordinates) {
      ut.passes("M has right size");
    } else {
      ut.failure("M does NOT have right size");
    }
    if (D.size() == number_of_moments * number_of_ordinates) {
      ut.passes("D has right size");
    } else {
      ut.failure("D does NOT have right size");
    }

    if ((ordinate_space->quadrature_interpolation_model() == GQ1) ||
        (ordinate_space->quadrature_interpolation_model() == GQ2)) {
      for (unsigned m = 0; m < number_of_moments; ++m) {
        for (unsigned n = 0; n < number_of_moments; ++n) {
          double sum = 0.0;
          for (unsigned a = 0; a < number_of_ordinates; ++a) {
            sum +=
                D[a + number_of_ordinates * m] * M[n + a * number_of_moments];
          }
          if (m == n) {
            if (!soft_equiv(sum, 1.0)) {
              ut.failure("diagonal element of M*D NOT 1");
              return;
            }
          } else {
            if (!soft_equiv(sum, 0.0)) {
              ut.failure("off-diagonal element of M*D NOT 0");
              return;
            }
          }
        }
      }
    }
  }

  // Test flux maps

  {
    unsigned MtF_map[3];
    double MtF_fact[3];
    ordinate_space->moment_to_flux(MtF_map, MtF_fact);

    unsigned FtM_map[3];
    double FtM_fact[3];
    ordinate_space->flux_to_moment(FtM_map, FtM_fact);

    // See that these are inverses
    for (unsigned d = 0; d < dimension; ++d) {
      ut.check(MtF_map[d] < 3, "flux map in range", true);
      ut.check(FtM_map[MtF_map[d]] == d, "inversion index of flux map");
      ut.check(soft_equiv(FtM_fact[MtF_map[d]] * MtF_fact[d], 1.0),
               "inversion factor of flux map");
    }
  }
}

//----------------------------------------------------------------------------//
void test_no_axis(UnitTest &ut, Quadrature &quadrature,
                  unsigned const dimension,
                  rtt_mesh_element::Geometry const geometry,
                  unsigned const expansion_order,
                  string const &ordinate_interpolation_model,
                  bool const add_extra_directions,
                  Ordinate_Set::Ordering const ordering) {
  // Parse the interpolation model

  QIM qim = END_QIM;
  String_Token_Stream stokens(ordinate_interpolation_model);
  parse_quadrature_interpolation_model(stokens, qim);

  // Build an angle operator

  SP<Ordinate_Space> ordinate_space =
      quadrature.create_ordinate_space(dimension, geometry, expansion_order,
                                       add_extra_directions, ordering, qim);

  test_either(ut, ordinate_space, quadrature, expansion_order);
}

//----------------------------------------------------------------------------//
void test_axis(UnitTest &ut, Quadrature &quadrature, unsigned const dimension,
               rtt_mesh_element::Geometry const geometry,
               unsigned const expansion_order,
               string const &ordinate_interpolation_model,
               bool const add_extra_directions,
               Ordinate_Set::Ordering const ordering, unsigned const mu_axis,
               unsigned const eta_axis) {
  // Parse the interpolation model

  QIM qim = END_QIM;
  String_Token_Stream stokens(ordinate_interpolation_model);
  parse_quadrature_interpolation_model(stokens, qim);

  // Build an angle operator

  SP<Ordinate_Space> ordinate_space = quadrature.create_ordinate_space(
      dimension, geometry, expansion_order, mu_axis, eta_axis,
      add_extra_directions, ordering, qim);

  test_either(ut, ordinate_space, quadrature, expansion_order);
}

//----------------------------------------------------------------------------//
void quadrature_integration_test(UnitTest & /*ut*/, Quadrature &quadrature) {

  if (quadrature.quadrature_class() != INTERVAL_QUADRATURE) {
    // Build an ordinate set
    SP<Ordinate_Set> ordinate_set =
        quadrature.create_ordinate_set(3U, // dimension
                                       rtt_mesh_element::CARTESIAN,
                                       1.0,   // norm,
                                       false, // add_starting_directions
                                       false, // add_extra_directions,
                                       Ordinate_Set::LEVEL_ORDERED);

    vector<Ordinate> const &ordinates = ordinate_set->ordinates();
    unsigned const N = ordinates.size();

    double test_int8 = 0.0;
    double test_int6 = 0.0;
    double test_int4 = 0.0;
    double test_int2 = 0.0;

    std::cout << "Testing S-" << quadrature.sn_order()
              << " quadrature integration" << std::endl;

    for (unsigned i = 0; i < N; ++i) {
      //cout << "  mu = " << setprecision(10) << ordinates[i].mu()
      //     << "  eta = " << setprecision(10) << ordinates[i].eta()
      //     << "  xi = " << setprecision(10) << ordinates[i].xi()
      //     << " weight = " << setprecision(10) << ordinates[i].wt()
      //     << endl;

      if (ordinates[i].xi() > 0) {
        test_int2 += ordinates[i].mu() * ordinates[i].mu() *
                     ordinates[i].eta() * ordinates[i].eta() *
                     ordinates[i].wt();
        test_int4 += ordinates[i].mu() * ordinates[i].mu() * ordinates[i].mu() *
                     ordinates[i].mu() * ordinates[i].eta() *
                     ordinates[i].eta() * ordinates[i].eta() *
                     ordinates[i].eta() * ordinates[i].wt();
        test_int6 += ordinates[i].mu() * ordinates[i].mu() * ordinates[i].mu() *
                     ordinates[i].mu() * ordinates[i].mu() * ordinates[i].mu() *
                     ordinates[i].eta() * ordinates[i].eta() *
                     ordinates[i].eta() * ordinates[i].eta() *
                     ordinates[i].eta() * ordinates[i].eta() *
                     ordinates[i].wt();
        test_int8 +=
            ordinates[i].mu() * ordinates[i].mu() * ordinates[i].mu() *
            ordinates[i].mu() * ordinates[i].mu() * ordinates[i].mu() *
            ordinates[i].mu() * ordinates[i].mu() * ordinates[i].eta() *
            ordinates[i].eta() * ordinates[i].eta() * ordinates[i].eta() *
            ordinates[i].eta() * ordinates[i].eta() * ordinates[i].eta() *
            ordinates[i].eta() * ordinates[i].wt();
      }
    }
    std::cout << " test int (2) = " << test_int2 << "( "
              << test_int2 * 4 * 3.1415926535897932384 << " )" << std::endl;
    std::cout << " test int (4) = " << test_int4 << "( "
              << test_int4 * 4 * 3.1415926535897932384 << " )" << std::endl;
    std::cout << " test int (6) = " << test_int6 << "( "
              << test_int6 * 4 * 3.1415926535897932384 << " )" << std::endl;
    std::cout << " test int (8) = " << test_int8 << "( "
              << test_int8 * 4 * 3.1415926535897932384 << " )" << std::endl;
  }
}

//----------------------------------------------------------------------------//
void quadrature_test(UnitTest &ut, Quadrature &quadrature) {
  cout << "Testing quadrature " << quadrature.name() << endl;
  cout << "  Parse name: " << quadrature.parse_name() << endl;
  switch (quadrature.quadrature_class()) {
  case INTERVAL_QUADRATURE:
    cout << "  This is an interval quadrature." << endl;
    break;

  case TRIANGLE_QUADRATURE:
    cout << "  This is a triangle quadrature." << endl;
    {
      unsigned L = quadrature.number_of_levels();
      if (L) {
        cout << "  Number of level sets = " << L << endl;
      } else {
        ut.failure("no level sets are defined.");
      }
    }
    break;

  case SQUARE_QUADRATURE:
    cout << "  This is a square quadrature." << endl;
    {
      unsigned L = quadrature.number_of_levels();
      if (L) {
        cout << "  Number of level sets = " << L << endl;
      } else {
        ut.failure("no level sets are defined.");
      }
    }
    break;

  case OCTANT_QUADRATURE:
    cout << "  This is an octant quadrature." << endl;
    if (quadrature.number_of_levels()) {
      cout << "  Number of level sets = " << quadrature.number_of_levels()
           << endl;
    } else {
      cout << "  No level sets are defined." << endl;
    }
    break;

  default:
    ut.failure("Bad value for quadrature class");
    return;
  }

  // Test moment comparison.

  if (Moment(1, 1) == Moment(0, 0)) {
    ut.failure("moment comparison NOT correct");
  }
  if (Moment(1, 1) == Moment(1, 0)) {
    ut.failure("moment comparison NOT correct");
  }

  // Test default moment initialization

  if (Moment(1, 1) == Moment()) {
    ut.failure("moment comparison NOT correct");
  }

  // Test ordinate comparison.

  if (Ordinate(0.4, 0.3, sqrt(1.0 - 0.4 * 0.4 - 0.3 * 0.3), 0.5) ==
      Ordinate(0.3, 0.4, sqrt(1.0 - 0.4 * 0.4 - 0.3 * 0.3), 0.5)) {
    ut.failure("moment comparison NOT correct");
  }
  if (!(Ordinate(0.4, 0.3, sqrt(1.0 - 0.4 * 0.4 - 0.3 * 0.3), 0.5) ==
        Ordinate(0.4, 0.3, sqrt(1.0 - 0.4 * 0.4 - 0.3 * 0.3), 0.5))) {
    ut.failure("moment comparison NOT correct");
  }

  // Test ordinate access.

  if (Ordinate(1.0, 0.0, 0.0, 0.0).cosines()[0] != 1.0) {
    ut.failure("Ordinate::cosines NOT right");
  }

  // Test textifying and parsing.

  string text = quadrature.as_text("\n");
  String_Token_Stream tokens(text);
  SP<Quadrature> parsed_quadrature = parse_class<Quadrature>(tokens);

  if (tokens.error_count()) {
    ut.failure("Textification and parse did NOT succeed");
  }

  string text2 = parsed_quadrature->as_text("\n");
  if (text2 != text) {
    ut.failure("Textification and parse did NOT give identical results");
  }

  // ***** Test various geometry, dimensionaly, and interpolation model
  // ***** options.

  // Test 1-D options. These requre that the axes have not been reassigned.

  if (!quadrature.has_axis_assignments()) {

    // Build an ordinate set

    SP<Ordinate_Set> ordinate_set =
        quadrature.create_ordinate_set(1U, // dimension
                                       rtt_mesh_element::CARTESIAN,
                                       1.0,   // norm,
                                       false, // add_starting_directions
                                       false, // add_extra_directions,
                                       Ordinate_Set::LEVEL_ORDERED);

    if (ordinate_set->ordinates().size() >= 2) {
      ut.passes("Ordinate count is plausible");
    } else {
      ut.failure("Ordinate count is NOT plausible");
    }

    if (soft_equiv(ordinate_set->norm(), 1.0)) {
      ut.passes("Ordinate norm is correct");
    } else {
      ut.failure("Ordinate norm is NOT correct");
    }

    ordinate_set->display();

    test_no_axis(ut, quadrature,
                 1U, // dimension,
                 rtt_mesh_element::CARTESIAN,
                 1U, // expansion_order,
                 "SN",
                 false, // add_extra_directions,
                 Ordinate_Set::LEVEL_ORDERED);

    test_no_axis(ut, quadrature,
                 1U, // dimension,
                 rtt_mesh_element::CARTESIAN,
                 1U, // expansion_order,
                 "GQ1",
                 false, // add_extra_directions,
                 Ordinate_Set::LEVEL_ORDERED);

    if (quadrature.is_open_interval()) {
      // Our curvilinear angular operator algorithm doesn't work with
      // closed interval quadratures (those for which mu=-1 is part of the
      // set).
      test_no_axis(ut, quadrature,
                   1U, // dimension,
                   rtt_mesh_element::SPHERICAL,
                   1U, // expansion_order,
                   "SN",
                   false, // add_extra_directions,
                   Ordinate_Set::LEVEL_ORDERED);

      test_no_axis(ut, quadrature,
                   1U, // dimension,
                   rtt_mesh_element::SPHERICAL,
                   1U, // expansion_order,
                   "GQ1",
                   false, // add_extra_directions,
                   Ordinate_Set::LEVEL_ORDERED);
    }
  }

  if (quadrature.quadrature_class() != INTERVAL_QUADRATURE)
  // Multidimensional cases required a non-interval quadrature.
  {
    // Build an ordinate set

    SP<Ordinate_Set> ordinate_set =
        quadrature.create_ordinate_set(3U, // dimension
                                       rtt_mesh_element::CARTESIAN,
                                       1.0,   // norm,
                                       false, // add_starting_directions
                                       false, // add_extra_directions,
                                       Ordinate_Set::LEVEL_ORDERED);

    if (ordinate_set->ordinates().size() >= 2) {
      ut.passes("Ordinate count is plausible");
    } else {
      ut.failure("Ordinate count is NOT plausible");
    }

    if (soft_equiv(ordinate_set->norm(), 1.0)) {
      ut.passes("Ordinate norm is correct");
    } else {
      ut.failure("Ordinate norm is NOT correct");
    }

    ordinate_set->display();

    test_no_axis(ut, quadrature,
                 2U, // dimension,
                 rtt_mesh_element::CARTESIAN,
                 8U, // expansion_order,
                 "SN",
                 false, // add_extra_directions,
                 Ordinate_Set::OCTANT_ORDERED);

    test_no_axis(ut, quadrature,
                 3U, // dimension,
                 rtt_mesh_element::CARTESIAN,
                 8U, // expansion_order,
                 "SN",
                 false, // add_extra_directions,
                 Ordinate_Set::OCTANT_ORDERED);

    if (quadrature.quadrature_class() == TRIANGLE_QUADRATURE) {
      test_no_axis(ut, quadrature,
                   2U, // dimension,
                   rtt_mesh_element::CARTESIAN,
                   // expansion_order
                   min(8U, quadrature.number_of_levels()), "GQ1",
                   false, // add_extra_directions,
                   Ordinate_Set::OCTANT_ORDERED);

      test_no_axis(ut, quadrature,
                   3U, // dimension,
                   rtt_mesh_element::CARTESIAN,
                   // expansion_order
                   quadrature.number_of_levels() - 1, "GQ1",
                   false, // add_extra_directions,
                   Ordinate_Set::LEVEL_ORDERED);
    }

    if (!quadrature.has_axis_assignments()) {
      // Axisymmetric is hosed if axes have been reassigned, since the
      // levels are only guaranteed on the xi axis.
      if (false && quadrature.quadrature_class() == TRIANGLE_QUADRATURE) {
        test_no_axis(ut, quadrature,
                     1U, // dimension,
                     rtt_mesh_element::AXISYMMETRIC,
                     // expansion_order
                     min(8U, quadrature.number_of_levels()), "GQ1",
                     false, // add_extra_directions,
                     Ordinate_Set::LEVEL_ORDERED);
      }

      test_no_axis(ut, quadrature,
                   1U, // dimension,
                   rtt_mesh_element::AXISYMMETRIC,
                   8U, // expansion_order,
                   "SN",
                   false, // add_extra_directions,
                   Ordinate_Set::LEVEL_ORDERED);

      test_no_axis(ut, quadrature,
                   2U, // dimension,
                   rtt_mesh_element::AXISYMMETRIC,
                   8U, // expansion_order,
                   "SN",
                   false, // add_extra_directions,
                   Ordinate_Set::LEVEL_ORDERED);
    }

    // Test overriding axis assignments

    test_axis(ut, quadrature,
              3U, // dimension,
              rtt_mesh_element::CARTESIAN,
              8U, // expansion_order,
              "SN",
              false, // add_extra_directions,
              Ordinate_Set::LEVEL_ORDERED,
              1, // mu_axis
              2);
  }
}

} // end namespace rtt_quadrature

//----------------------------------------------------------------------------//
// This test gets called FROM Fortran to ensure that we can successfully create
// and assign data into a "quadrature_data" type.  See
// ftest/tstquadrature_interfaces.f90
//----------------------------------------------------------------------------//
extern "C" DLL_PUBLIC_quadrature_test void
rtt_test_quadrature_interfaces(const quadrature_data &quad, int &error_code) {
  using std::cout;
  using std::endl;
  using rtt_dsxx::soft_equiv;

  cout << "In C++, checking validity of quadrature_data..." << endl;
  check_quadrature_validity(quad);
  cout << "If we got here, it should be valid." << endl << endl;

  cout << "The quadrature_data type has dimension " << quad.dimension << endl
       << "The type is \t" << quad.type << endl
       << "The order is \t" << quad.order << endl
       << "The geometry is\t" << quad.geometry << endl
       << "The first ordinate is " << quad.mu[0] << "\t" << quad.eta[0] << "\t"
       << quad.xi[0] << "\t" << quad.weights[0] << endl
       << endl;

  // Error checking
  error_code = 0;

  if (quad.dimension != 2) {
    error_code = 1;
    return;
  } else if (quad.type != 1) {
    error_code = 2;
    return;
  } else if (quad.order != 4) {
    error_code = 3;
    return;
  } else if (quad.geometry != 0) {
    error_code = 4;
    return;
  }

  double tcl_mu[12] = {-0.3594747925, 0.3594747925,  -0.8688461434,
                       -0.3598878562, 0.3598878562,  0.8688461434,
                       -0.8688461434, -0.3598878562, 0.3598878562,
                       0.8688461434,  -0.3594747925, 0.3594747925};
  double tcl_eta[12] = {-0.8611363116, -0.8611363116, -0.3399810436,
                        -0.3399810436, -0.3399810436, -0.3399810436,
                        0.3399810436,  0.3399810436,  0.3399810436,
                        0.3399810436,  0.8611363116,  0.8611363116};

  double tcl_wt[12] = {0.08696371128, 0.08696371128, 0.08151814436,
                       0.08151814436, 0.08151814436, 0.08151814436,
                       0.08151814436, 0.08151814436, 0.08151814436,
                       0.08151814436, 0.08696371128, 0.08696371128};

  for (size_t i = 0; i < 12; ++i) {
    if (!soft_equiv(tcl_mu[i], quad.mu[i], 1e-8))
      error_code = 5;
    if (!soft_equiv(tcl_eta[i], quad.eta[i], 1e-8))
      error_code = 6;
    if (!soft_equiv(tcl_wt[i], quad.weights[i], 1e-8))
      error_code = 7;
  }

  return;
}

//---------------------------------------------------------------------------//
// end of quadrature/quadrature_test.cc
//---------------------------------------------------------------------------//
