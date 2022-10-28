
#ifndef AssignPixelID_h
#define AssignPixelID_h

#include <vector>

namespace gramsreadoutsim {

    class AssignPixelID
    {
    public:

        AssignPixelID();

        virtual ~AssignPixelID();

        std::tuple<std::vector<int>, std::vector<int>> 
        Assign(std::vector<double>*, std::vector<double>*);

    private:

        double m_verbose;
        double m_debug;
        double m_pixel_sizex;
        double m_pixel_sizey;
        double m_offset_x;
        double m_offset_y;

        std::vector<int> m_pixel_idx;
        std::vector<int> m_pixel_idy;
    };
}

#endif
