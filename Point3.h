
#ifndef Point3_h
#define Point3_h

struct PointForTree {
    double vec[3];
    int seg;
    double normalX;
    double normalY;
    double normalZ;
    PointForTree() { vec[0]= vec[1] = vec[2] = 0; }
    PointForTree (double x, double y, double z) { vec[0]=x; vec[1]=y; vec[2]=z;}
    double x() const { return vec[ 0 ]; }
    double y() const { return vec[ 1 ]; }
    double z() const { return vec[ 2 ]; }
    int getSeg() const{return seg;}
    double normx() const { return normalX; }
    double normy() const { return normalY; }
    double normz() const { return normalZ; }
    double& x() { return vec[ 0 ]; }
    double& y() { return vec[ 1 ]; }
    double& z() { return vec[ 2 ]; }
    bool operator==(const PointForTree& p) const
    {
        return (x() == p.x()) && (y() == p.y()) && (z() == p.z())  ;
    }
    bool  operator!=(const PointForTree& p) const { return ! (*this == p); }
}; //end of class
struct Construct_coord_iterator {
    typedef  const double* result_type;
    const double* operator()(const PointForTree& p) const
    { return static_cast<const double*>(p.vec); }
    const double* operator()(const PointForTree& p, int)  const
    { return static_cast<const double*>(p.vec+3); }
};


#endif /* Point_h */
