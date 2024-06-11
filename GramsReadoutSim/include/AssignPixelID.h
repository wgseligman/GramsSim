
#ifndef AssignPixelID_h
#define AssignPixelID_h

// From GramsDataObj
#include "ReadoutID.h"
#include "ElectronClusters.h"

namespace gramsreadoutsim {

    class AssignPixelID
    {
    public:

      // Constructor.
      AssignPixelID();

      // Destructor.
      virtual ~AssignPixelID();

      // Accept the (x,y) positions of an electron cluster at the
      // anode. Determine the readout cell associated with that coordinate.

      const grams::ReadoutID Assign(const grams::ElectronCluster& ec);

    private:

      bool m_verbose;
      bool m_debug;

      // The variables that define the geometry of the pixel readout. 
      double m_pixel_sizex;
      double m_pixel_sizey;
      double m_offset_x;
      double m_offset_y;
    };
}

#endif
