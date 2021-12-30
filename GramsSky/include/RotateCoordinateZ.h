// RotateCoordinateZ.h
// 24-Dec-2021 WGS

// First, what does this function do?

// From ROOT's TVector3 class documentation:
//
// Transformation from rotated frame
// ROOT::Math::XYZVector direction = v.Unit()
// v1.RotateUz(direction); // direction must be a 3-vector of unit length
//
// Transforms v1 from the rotated frame (z' parallel to direction, x'
// in the theta plane and y' in the xy plane as well as perpendicular
// to the theta plane) to the (x,y,z) frame.

// From CLHEP's ThreeVector (Hep3Vector) documentation:
//
// Rotates reference frame from Uz to newUz (unit vector) (Geant4).

// And that's _all_ I understand about what this function does. 

// If there's already a RotateUz function as part of the above vector
// classes, why implement this separate function? The documentation
// for ROOT's TVector3 class says that TVector3 is now a legacy class,
// and that users should switch to ROOT::Math::XYZVector. 

// The issue in this case is that neither ROOT::Math::XYZVector nor
// any of ROOT's other modern rotation-related utility classes
// (ROOT::Math::VectorUtil, ROOT::Math::AxisAngle) implements an
// explicit RotateUz function. I'm not facile enough with rotations in
// 3-space to figure out what combination of the VectorUtil and
// AxisAngle methods corresponds to RotateUz.

// So this function is essentially a copy-and-paste of the RotateUz
// code in TVector3, with a couple of minor changes, for the
// ROOT::Math::XYZVector class.

// Note that this function modifies the vector in the first argument,
// because that's how RotateUz does it.

#ifndef _RotateCoordinateZ_h_
#define _RotateCoordinateZ_h_

#include <Math/Vector3D.h> // defines ROOT::Math::XYZVector
#include <cmath>

namespace gramssky {

  void RotateCoordinateZ(ROOT::Math::XYZVector& v, const ROOT::Math::XYZVector& uz)
  {
    auto uzUnit = uz.unit();
    const double u1 = uzUnit.x();
    const double u2 = uzUnit.y();
    const double u3 = uzUnit.z();
    const double up2 = u1*u1 + u2*u2;
    
    if ( up2 > 0 ) {
      const double up = std::sqrt(up2);
      const double vx = v.x();
      const double vy = v.y();
      const double vz = v.z();
      v.SetX( u1*u3*vx/up + u2*u3*vy/up - up*vz);
      v.SetY(-u2*vx/up    + u1*vy/up           );
      v.SetZ( u1*vx       + u2*vy       + u3*vz);
    }
    else if ( u3 < 0. ) {
      // phi=0, theta=pi
      v.SetX(-v.x());
      v.SetZ(-v.z());
    }
  }

} // namespace gramssky

#endif // _RotateCoordinateZ_h_
