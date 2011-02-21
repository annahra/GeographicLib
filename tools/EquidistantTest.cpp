/**
 * \file EquidistantTest.cpp
 * \brief Command line utility for azimuthal equidistant and Cassini-Soldner
 * projections
 *
 * Copyright (c) Charles Karney (2009, 2010, 2011) <charles@karney.com>
 * and licensed under the LGPL.  For more information, see
 * http://geographiclib.sourceforge.net/
 *
 * Compile with -I../include and link with Geodesic.o GeodesicLine.o
 * AzimuthalEquidistant.o Gnomonic.o CassiniSoldner.o
 *
 * See the <a href="EquidistantTest.1.html">man page</a> for usage
 * information.
 *
 * $Id$
 **********************************************************************/

#include "GeographicLib/Geodesic.hpp"
#include "GeographicLib/AzimuthalEquidistant.hpp"
#include "GeographicLib/CassiniSoldner.hpp"
#include "GeographicLib/Gnomonic.hpp"
#include "GeographicLib/DMS.hpp"
#include <iostream>
#include <sstream>

#include "EquidistantTest.usage"

int main(int argc, char* argv[]) {
  using namespace GeographicLib;
  typedef Math::real real;
  bool azimuthal = false, cassini = false, gnomonic = false, reverse = false;
  real lat0 = 0, lon0 = 0;
  real
    a = Constants::WGS84_a<real>(),
    r = Constants::WGS84_r<real>();
  for (int m = 1; m < argc; ++m) {
    std::string arg(argv[m]);
    if (arg == "-r")
      reverse = true;
    else if (arg == "-c" || arg == "-z" || arg == "-g") {
      cassini = arg == "-c";
      azimuthal = arg == "-z";
      gnomonic = arg == "-g";
      if (m + 2 >= argc) return usage(1, true);
      try {
        DMS::DecodeLatLon(std::string(argv[m + 1]), std::string(argv[m + 2]),
                          lat0, lon0);
      }
      catch (const std::exception& e) {
        std::cerr << "Error decoding arguments of " << arg << ": "
                  << e.what() << "\n";
        return 1;
      }
      m += 2;
    } else if (arg == "-e") {
      if (m + 2 >= argc) return usage(1, true);
      try {
        a = DMS::Decode(std::string(argv[m + 1]));
        r = DMS::Decode(std::string(argv[m + 2]));
      }
      catch (const std::exception& e) {
        std::cerr << "Error decoding arguments of -e: " << e.what() << "\n";
        return 1;
      }
      m += 2;
    } else if (arg == "--version") {
      std::cout << PROGRAM_NAME << ": $Id$\n"
                << "GeographicLib version " << GEOGRAPHICLIB_VERSION << "\n";
      return 0;
    } else
      return usage(!(arg == "-h" || arg == "--help"), arg != "--help");
  }

  if (!(azimuthal || cassini || gnomonic)) {
    std::cerr << "Must specify \"-z lat0 lon0\" or "
              << "\"-c lat0 lon0\" or \"-g lat0 lon0\"\n";
    return 1;
  }

  const Geodesic geod(a, r);
  const CassiniSoldner cs = cassini ?
    CassiniSoldner(lat0, lon0, geod) : CassiniSoldner(geod);
  const AzimuthalEquidistant az(geod);
  const Gnomonic gn(geod);

  std::string s;
  int retval = 0;
  std::cout << std::fixed;
  while (std::getline(std::cin, s)) {
    try {
      std::istringstream str(s);
      real lat, lon, x, y, azi, rk;
      std::string stra, strb;
      if (!(str >> stra >> strb))
        throw GeographicErr("Incomplete input: " + s);
      if (reverse) {
        x = DMS::Decode(stra);
        y = DMS::Decode(strb);
      } else
        DMS::DecodeLatLon(stra, strb, lat, lon);
      std::string strc;
      if (str >> strc)
        throw GeographicErr("Extraneous input: " + strc);
      if (reverse) {
        if (cassini)
          cs.Reverse(x, y, lat, lon, azi, rk);
        else if (azimuthal)
          az.Reverse(lat0, lon0, x, y, lat, lon, azi, rk);
        else
          gn.Reverse(lat0, lon0, x, y, lat, lon, azi, rk);
        std::cout << DMS::Encode(lat, 15, DMS::NUMBER) << " "
                  << DMS::Encode(lon, 15, DMS::NUMBER) << " "
                  << DMS::Encode(azi, 15, DMS::NUMBER) << " "
                  << DMS::Encode(rk, 16, DMS::NUMBER) << "\n";
      } else {
        if (cassini)
          cs.Forward(lat, lon, x, y, azi, rk);
        else if (azimuthal)
          az.Forward(lat0, lon0, lat, lon, x, y, azi, rk);
        else
          gn.Forward(lat0, lon0, lat, lon, x, y, azi, rk);
        std::cout << DMS::Encode(x, 10, DMS::NUMBER) << " "
                  << DMS::Encode(y, 10, DMS::NUMBER) << " "
                  << DMS::Encode(azi, 15, DMS::NUMBER) << " "
                  << DMS::Encode(rk, 16, DMS::NUMBER) << "\n";
      }
    }
    catch (const std::exception& e) {
      std::cout << "ERROR: " << e.what() << "\n";
      retval = 1;
    }
  }

  return retval;
}
