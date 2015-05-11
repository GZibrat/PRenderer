// SimplexNoise1234
// Copyright © 2003-2011, Stefan Gustavson
//
// Contact: stegu@itn.liu.se
//
// This library is public domain software, released by the author
// into the public domain in February 2011. You may do anything
// you like with it. You may even remove all attributions,
// but of course I'd appreciate it if you kept my name somewhere.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

/** \file
\brief Declares the SimplexNoise1234 class for producing Perlin simplex noise.
\author Stefan Gustavson (stegu@itn.liu.se)
*/

/*
* This is a clean, fast, modern and free Perlin Simplex noise class in C++.
* Being a stand-alone class with no external dependencies, it is
* highly reusable without source code modifications.
*
*
* Note:
* Replacing the "double" type with "double" can actually make this run faster
* on some platforms. A templatized version of SimplexNoise1234 could be useful.
*/

class SimplexNoise1234 {

public:
	SimplexNoise1234() {}
	SimplexNoise1234(double _persistence, double _frequency, double _amplitude, int _octaves, int _randomseed);
	~SimplexNoise1234() {}

	/** 1D, 2D, 3D and 4D double Perlin noise
	*/
	double noise(double x);
	double noise(double x, double y);
	double noise(double x, double y, double z);
	double noise(double x, double y, double z, double w);

	double GetHeight(double x, double y, double k);

	double Total(double i, double j, double k);

private:
	double  grad(int hash, double x);
	double  grad(int hash, double x, double y);
	double  grad(int hash, double x, double y, double z);
	double  grad(int hash, double x, double y, double z, double t);

	double persistence, frequency, amplitude;
	int octaves, randomseed;

};