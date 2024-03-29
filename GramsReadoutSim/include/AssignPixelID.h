
#ifndef AssignPixelID_h
#define AssignPixelID_h

#include <vector>

namespace gramsreadoutsim {

    class AssignPixelID
    {
    public:

      // Constructor.
      AssignPixelID(int channels_x, int channels_y, double x_dim, double y_dim, double offset_x, double offset_y , bool verbose=false, bool debug=false);

      // Destructor.
      virtual ~AssignPixelID();

      // Accept the (x,y) positions of the electron clusters at the
      // anode. For each cluster, determine its (x,y) pixel ID
      // numbers.

      // Note that the routine returns two vectors at once, in the
      // form of a C++ tuple.
      std::tuple<std::vector<int>, std::vector<int>> 
        Assign(std::vector<double>*, std::vector<double>*);

    private:

      bool m_verbose;
      bool m_debug;

      // The variables that define the geometry of the pixel readout. 
      double m_pixel_sizex;
      double m_pixel_sizey;
      double m_offset_x;
      double m_offset_y;
      
      std::vector<int> m_pixel_idx;
      std::vector<int> m_pixel_idy;
    };
}

#endif
