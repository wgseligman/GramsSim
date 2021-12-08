// 08-Dec-2021 WGS 

// An extremely simple data class for transferring particle
// information between routines in GramsSky.

#ifndef Grams_ParticleInfo_h
#define Grams_ParticleInfo_h

namespace gramssky {

  class ParticleInfo
  {
  public:

    // Constructor. 
    ParticleInfo()
      : m_x(0.)
      , m_y(0.)
      , m_z(0.)
      , m_t(0.)
      , m_px(0.)
      , m_py(0.)
      , m_pz(0.)
      , m_e(0.)
      , m_pdg(0)
      , m_polPhi(0.)
      , m_polTheta(0.)
    {}

    // Destructor. It's standard practice to declare all destructors
    // as virtual in C++, though it's only relevant if we were ever to
    // create a class that inherits from this one.
    virtual ~ParticleInfo() {}

    // Setters and getters.
    void SetX(const double x)            { m_x = x; }
    void SetY(const double y)            { m_y = y; }
    void SetZ(const double z)            { m_z = z; }
    void SetT(const double t)            { m_t = t; }
    void SetPx(const double px)          { m_px = px; }
    void SetPy(const double py)          { m_py = py; }
    void SetPz(const double pz)          { m_pz = pz; }
    void SetE(const double e)            { m_e = e; }
    void SetPDG(const int id)            { m_pdg = id; }
    void SetPolPhi(const double phi)     { m_polPhi = phi; }
    void SetPolTheta(const double theta) { m_polTheta = theta; }

    double GetX() const        { return m_x; }
    double GetY() const        { return m_y; }
    double GetZ() const        { return m_z; }
    double GetT() const        { return m_t; }
    double GetPx() const       { return m_px; }
    double GetPy() const       { return m_py; }
    double GetPz() const       { return m_pz; }
    double GetE() const        { return m_e; }
    int    GetPDG() const      { return m_pdg; }
    double GetPolPhi() const   { return m_polPhi; }
    double GetPolTheta() const { return m_polPhi; }

  private:

    double m_x;
    double m_y;
    double m_z;
    double m_t;
    double m_px;
    double m_py;
    double m_pz;
    double m_e;
    int m_pdg;
    double m_polPhi;
    double m_polTheta;
  };

} // namespace gramssky

#endif // Grams_ParticleInfo_h
