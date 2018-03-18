#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

#include <CGAL/pca_estimate_normals.h>
#include <CGAL/mst_orient_normals.h>
#include <CGAL/property_map.h>
#include <map>
#include <sys/time.h>
#include <eigen/Eigen/Core>
#include <cmath>

#include "hrbf_core.h"
#include "hrbf_phi_funcs.h"
#include "CIsoSurface.h"
#include "Vectors.h"
#include "implicit_function_hrbf.h"
#include "implicit_function_crbf.h"
#include "HRBF_closed.h"
#include "Build_surface.h"
#include "Cell_inside.h"
#include "Surface_builder.h"
#include "implicit_function_csg.h"
#include "implicit_function_min.h"
#include "Surface_builder_dual.h"
//#include "takeUnitPolyhedron.h"
#include <utility>
#include <fstream>

#include <CGAL/Cartesian.h>
#include <CGAL/compute_average_spacing.h>
#include <CGAL/Poisson_reconstruction_function.h>
#include <CGAL/make_surface_mesh.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/IO/output_surface_facets_to_polyhedron.h>
#include <CGAL/trace.h>
#include <CGAL/Implicit_surface_3.h>
#include <CGAL/Surface_mesh_default_triangulation_3.h>
#include <CGAL/IO/read_xyz_points.h>
#include <CGAL/basic.h>
#include <CGAL/Inverse_index.h>
#include <CGAL/trace.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include <CGAL/Min_sphere_d.h>
#include <CGAL/Min_sphere_annulus_d_traits_3.h>
#include <CGAL/Min_sphere_d.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/Triangulation_data_structure_3.h>
#include <CGAL/Tetrahedron_3.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include <CGAL/property_map.h>

#include <CGAL/Polygon_mesh_processing/refine.h>
#include <CGAL/mesh_segmentation.h>
#include <CGAL/Nef_polyhedron_3.h>
#include <CGAL/Search_traits.h>
//#include <CGAL/point_generators_3.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include "Point3.h"  // defines types Point, Construct_coord_iterator
#include "Distance.h"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_mesh_processing/refine.h>
#include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>
#include <CGAL/Polygon_mesh_processing/triangulate_hole.h>
#include <boost/foreach.hpp>


#include <cstdlib>
#include <iostream>
#include <sstream>
#include <cassert>
#include <limits>
#include "Viewer.h"
#include "Image.h"
#include "TriMesh.h"
#include <chrono>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>
#endif

#ifdef CGAL_EIGEN3_ENABLED
#include <CGAL/Eigen_solver_traits.h>
#endif





int  Viewer::windowSize[2] = { 800, 800 };
bool Viewer::renderWireframe = false;
bool Viewer::renderSelected = false;
bool changedisp=false;
bool deformationSwitch=true;
//use in selectedVertDeformation
double d=0.1;

std::set<unsigned> Viewer::selectedVert;
TriMesh* Viewer::meshPtr = 0;
int    Viewer::verbose = 0;

GLuint Viewer::surfaceDL = 0;
Shader Viewer::shader;
Camera Viewer::camera;


//added
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef CGAL::Exact_predicates_exact_constructions_kernel exact_Kernel;
//typedef CGAL::Cartesian<double> Kernel;
typedef Kernel::Point_3 Point;
typedef Kernel::Vector_3 Vector;
typedef Eigen::Matrix<double,3,1> Vector3;
CIsoSurface <double> *ciso = new CIsoSurface <double> ();
TriMesh Viewer::marching;
typedef CGAL::Poisson_reconstruction_function<Kernel> Poisson_reconstruction_function;
typedef Kernel::FT FT;
typedef CGAL::Surface_mesh_default_triangulation_3 STr;
typedef CGAL::Surface_mesh_complex_2_in_triangulation_3<STr> C2t3;
typedef Kernel::Sphere_3 Sphere;
typedef CGAL::Implicit_surface_3<Kernel, Poisson_reconstruction_function> Surface_3;
typedef CGAL::Polyhedron_3<Kernel> SurfaceMesh;
typedef CGAL::Polyhedron_3<exact_Kernel> Polyhedron2;
typedef CGAL::Nef_polyhedron_3<exact_Kernel> Nef_polyhedron;
typedef CGAL::Point_with_normal_3<Kernel> Point_with_normal_3;
typedef std::vector<Point_with_normal_3> PointNormalList;
typedef std::vector<Point> PointList;
typedef std::pair<Point, Vector> PointVectorPair;
typedef CGAL::Surface_mesh_default_triangulation_3 Tr;
typedef Tr::Geom_traits GT;
typedef CGAL::Min_sphere_annulus_d_traits_3<Kernel> Traits;
typedef CGAL::Min_sphere_d<Traits>             Min_sphere;
typedef implicit_function_hrbf<FT,Point > Hrbf_function;
typedef implicit_function_crbf<FT,Point> Crbf_function;
typedef CGAL::Implicit_surface_3<Kernel, Hrbf_function> Surface_3_hrbf;
typedef CGAL::Implicit_surface_3<Kernel, Crbf_function> Surface_3_crbf;
typedef CGAL::Implicit_surface_3<Kernel, implicit_function_csg> Surface_3_csg;
typedef CGAL::Implicit_surface_3<Kernel, implicit_function_min> Surface_3_min;
typedef CGAL::Delaunay_triangulation_3<Kernel> Delaunay;
typedef Delaunay::Finite_cells_iterator Finite_cells_iterator;
typedef Delaunay::Finite_facets_iterator Finite_facets_iterator;
typedef Delaunay::All_facets_iterator All_facets_iterator;
typedef CGAL::Tetrahedron_3<Kernel> Tetrahedron_3;
typedef SurfaceMesh::HalfedgeDS             HalfedgeDS;
// Concurrency
#ifdef CGAL_LINKED_WITH_TBB
typedef CGAL::Parallel_tag Concurrency_tag;
#else
typedef CGAL::Sequential_tag Concurrency_tag;
#endif

//typedef CGAL::Surface_mesh_complex_2_in_triangulation_3<> C2t3_d;

typedef Delaunay::Cell_handle    Cell_handle;
typedef Delaunay::Vertex_handle  Vertex_handle;
typedef Delaunay::Locate_type    Locate_type;
typedef Delaunay::Vertex_handle    Vertex_handle;
typedef SurfaceMesh::Vertex_handle polyVertex_handle;
typedef Delaunay::Finite_cells_iterator Finite_cells_iterator;
typedef CGAL::Dimension_tag<3> D;
typedef CGAL::Search_traits<double, PointForTree, const double*, Construct_coord_iterator, D> SearchTraits;
typedef CGAL::Orthogonal_k_neighbor_search<SearchTraits, Distance> K_neighbor_search;
typedef K_neighbor_search::Tree Tree;

typedef CGAL::Eigen_solver_traits<> Eigen_solver;
int reconstructionValue;
double thr;
double sigma;
int nb_neighbors;
int nb_neighbors2;
FT sm_angle;
FT sm_radius;
FT sm_distance;
FT sm_sphere_radius;
int resamplingSwitch;
int mesher;
int gridsize;
double boundingBox;
double cellInsideValue;
bool segmentedColor=true;
int unionValue=0;
double epsilonForMarchingBounding=0.05;
int replaceSwitch;
std::vector<std::vector<Vector3> > pointsSets;
std::vector<std::vector<Vector3> > normalsSets;
std::vector<Poisson_reconstruction_function> functionSets;
std::vector<Vector3> minPoints;
std::vector<Vector3> minNormals;
std::vector<Poisson_reconstruction_function>  originalFunction;
std::vector<PointForTree> pointForTree;
// Precompute and keep the grids used by MC algorithm
struct MCGrid {
    std::vector<Vector3> structuredGrid;
    std::vector<double> results;
    
    unsigned int subx;
    unsigned int suby;
    unsigned int subz;
    
    float dx;
    float dy;
    float dz;
};
MCGrid mcgrid;





static void
createGrid(const Vector3& leftCorner,
           const Vector3& rightCorner,
           unsigned int nx, unsigned int ny, unsigned int nz,
           std::vector<Vector3>& grid)
{
    double dx = (rightCorner[0] - leftCorner[0]) / nx;
    double dy = (rightCorner[1] - leftCorner[1]) / ny;
    double dz = (rightCorner[2] - leftCorner[2]) / nz;
    
    double currentX = leftCorner[0];
    double currentY = leftCorner[1];
    double currentZ = leftCorner[2];
    
    for (unsigned int zsub = 0; zsub < nz; ++zsub) {
        currentY = leftCorner[1];
        for (unsigned int ysub = 0; ysub < ny; ++ysub) {
            currentX = leftCorner[0];
            for (unsigned int xsub = 0; xsub < nx; ++xsub) {
                grid.push_back(Vector3(currentX, currentY, currentZ));
                currentX = currentX + dx;
            }
            currentY = currentY + dy;
        }
        currentZ = currentZ + dz;
    }
}


static void
initMCGrid(double max_x,double min_x,double max_y,double min_y,double max_z,double min_z)
{
    //std::vector<Vector3> structuredGrid;
    Vector3 leftCorner(min_x,min_y,min_z);
    Vector3 rightCorner(max_x,max_y,max_z);
    mcgrid.subx = gridsize;
    mcgrid.suby = gridsize;
    mcgrid.subz = gridsize;
    createGrid(leftCorner, rightCorner,
               mcgrid.subx, mcgrid.suby, mcgrid.subz,
               mcgrid.structuredGrid);
    
    // Evaluate on the grid
    std::vector<double> results(mcgrid.structuredGrid.size());
    mcgrid.results = results;
    
    mcgrid.dx = (float)(rightCorner[0] - leftCorner[0]) / mcgrid.subx;
    mcgrid.dy = (float)(rightCorner[1] - leftCorner[1]) / mcgrid.suby;
    mcgrid.dz = (float)(rightCorner[2] - leftCorner[2]) / mcgrid.subz;
}


void
Viewer::keyboard(unsigned char c, int /*x*/, int /*y*/)
{
    switch(c)
    {
        case 27:
            exit(0);
            break;
        case 'w':
            renderWireframe = !renderWireframe;
            updateDisplayList();
            break;
        case 's':
            renderSelected = !renderSelected;
            std::cout<<renderSelected<<std::endl;
            updateDisplayList();
            break;
        case 'r':
            clearData();
            updateDisplayList();
            break;
            
        case '/':
            takeScreenshot();
            break;
        case 'c':
            changedisp=!changedisp;
            if(changedisp==true)std::cout<<"disp=-1 mode"<<std::endl;
            else std::cout<<"original mode"<<std::endl;
            updateDisplayList();
            break;
        case '+':
            d+=0.1;
            std::cout<<"value of d = "<<d<<std::endl;
            break;
        case '-':
            d-=0.1;
            std::cout<<"value of d = "<<d<<std::endl;
            break;
        case 'd':
            deformationSwitch=!deformationSwitch;
            if(deformationSwitch==true)std::cout<<"Deformation mode ON"<<std::endl;
            else std::cout<<"Deformation mode OFF"<<std::endl;
            break;
        default:
            break;
    }
}

void
Viewer::init(int argc, char** argv)
{
#if defined(HRBF) || defined(HRBF_CLOSED)
#endif
    
    initGLUT(argc, argv);
    initGLSL();
    setGL();
    updateDisplayList();
    glutMainLoop();
}

void
Viewer::initGLUT(int argc, char** argv)
{
    glutInitWindowSize(windowSize[0], windowSize[1]);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInit(&argc, argv);
    glutCreateWindow("Pick Test");
    
    glutMouseFunc   (Viewer::mouse   );
    glutKeyboardFunc(Viewer::keyboard);
    glutSpecialFunc (Viewer::special );
    glutMotionFunc  (Viewer::motion  );
    glutDisplayFunc (Viewer::display );
    glutIdleFunc    (Viewer::idle    );
}

void
Viewer::initGLSL()
{
    shader.loadVertex  ("vertex.glsl"  );
    shader.loadFragment("fragment.glsl");
}

void
Viewer::special(int i, int /*x*/, int /*y*/)
{
    switch (i)
    {
        case GLUT_KEY_UP:
            camera.zoomIn();
            break;
        case GLUT_KEY_DOWN:
            camera.zoomOut();
            break;
        case 27:
            exit(0);
            break;
        default:
            break;
    }
}

void
Viewer::mouse(int button, int state, int x, int y)
{
    
    if ((glutGetModifiers() & GLUT_ACTIVE_SHIFT) && (state == GLUT_UP))
    {
        pickVertex(x, y);
        return;
    }
    camera.mouse(button, state, x, y);
}

void
Viewer::motion(int x, int y)
{
    camera.motion(x, y);
}

void
Viewer::idle()
{
    glutPostRedisplay();
}

void
Viewer::display()
{
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.enable();
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    double aspect = double(viewport[2]) / double(viewport[3]);
    const double fovy = 50.;
    const double clipNear = .01;
    const double clipFar = 1000.;
    gluPerspective(fovy, aspect, clipNear, clipFar);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    
    Quaternion    eye(0., 0., 0.,-2.5*camera.zoom());
    Quaternion center(0., 0., 0., 0. );
    Quaternion     up(0., 0., 1., 0. );
    
    gluLookAt(   eye.x,    eye.y,    eye.z,
              center.x, center.y, center.z,
              up.x,     up.y,     up.z );
    
    GLint uniformEye = glGetUniformLocation(shader, "eye");
    Quaternion r = camera.computeCurrentRotation();
    eye = r.conjugate() * eye * r;
    glUniform3f(uniformEye, eye.x, eye.y, eye.z);
    
    GLint uniformLight = glGetUniformLocation( shader, "light" );
    Quaternion light(0., -1., 1., -10.);
    light = r.conjugate() * light * r;
    glUniform3f(uniformLight, light.x, light.y, light.z);
    
    camera.setGLModelView();
    
    callDisplayList();
    
    shader.disable();
    
    glutSwapBuffers();
    
}

void
Viewer::updateDisplayList()
{
    if (surfaceDL)
    {
        glDeleteLists(surfaceDL, 1);
        surfaceDL = 0;
    }
    
    surfaceDL = glGenLists(1);
    glNewList(surfaceDL, GL_COMPILE);
    drawScene();
    glEndList();
}

void Viewer::setGL()
{
    glClearColor( 1., 1., 1., 1. );
    setLighting();
}

void Viewer::setLighting()
{
    GLfloat position[4] = { 20., 30., 40., 0. };
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHT0);
    
    GLfloat  diffuse[4] = { .8, .5, .3, 1. };
    GLfloat specular[4] = { .3, .3, .3, 1. };
    GLfloat  ambient[4] = { .2, .2, .5, 1. };
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   diffuse );
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   ambient );
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 16.     );
}

void
Viewer::callDisplayList()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glCallList(surfaceDL);
    glPopAttrib();
}

void
Viewer::drawScene()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    glDisable(GL_POLYGON_OFFSET_FILL);
    if (renderWireframe) drawWireframe();
    if (renderSelected ) drawSelectedVerts();
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1., 1.);
    
    drawPolygons();
    
    glPopAttrib();
}

void
Viewer::drawPolygons()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    shader.disable();//This sentence needs for keeping colors.
    std::vector<Vec3> vertNormal;
    meshPtr->computeVertNormals(vertNormal);
    
    glBegin(GL_TRIANGLES);
    for (unsigned face = 0; face < meshPtr->numFaces(); ++face)
    {
        if (renderWireframe)
        {
            Vec3 n = meshPtr->computeFaceNormal(face);
            glNormal3d(n.x, n.y, n.z);
        }
        
        std::vector<unsigned> fVerts;
        meshPtr->getFaceVerts(face, fVerts);
        assert(fVerts.size() == 3);
        
        for (unsigned i = 0; i < 3; ++i)
        {
            unsigned vert = fVerts[i];
            Vec3 p = meshPtr->getVertPos(vert);
            if (!renderWireframe) glNormal3d(vertNormal[vert].x,vertNormal[vert].y, vertNormal[vert].z);
            if(segmentedColor==true){
                glColor3d(meshPtr->colors[face*3], meshPtr->colors[face*3+1], meshPtr->colors[face*3+2]);
            }
            else{
                double alpha = 0.5;
                glColor3d(alpha, alpha, alpha);
            }
            glVertex3d(p.x, p.y, p.z);
        }
    }
    glEnd();
    glDisable(GL_COLOR_MATERIAL);
}

void
Viewer::drawWireframe()
{
    shader.disable();
    glDisable(GL_LIGHTING);
    glColor4f(0., 0., 0., 1.);
    
    glBegin(GL_LINES);
    for (unsigned face = 0; face < meshPtr->numFaces(); ++face)
    {
        std::vector<unsigned> fVerts;
        meshPtr->getFaceVerts(face, fVerts);
        assert(fVerts.size() == 3);
        
        for (unsigned i = 0; i < 3; ++i)
        {
            Vec3 p0 = meshPtr->getVertPos(fVerts[(i+1)%3]);
            Vec3 p1 = meshPtr->getVertPos(fVerts[(i+2)%3]);
            glVertex3d(p0.x, p0.y, p0.z);
            glVertex3d(p1.x, p1.y, p1.z);
        }
    }
    glEnd();
}

void
Viewer::drawSelectedVerts( )
{
    shader.disable();
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    glEnable(GL_COLOR_MATERIAL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glColor3f(0.0, 0.5, 0.5);
    double h = 0.2 * meshPtr->computeMaxEdgeLength();
    for (std::set<unsigned>::const_iterator
         it = selectedVert.begin(); it != selectedVert.end(); ++it)
    {
        Vec3 p = meshPtr->getVertPos(*it);
        std::cout<<p.x<<" "<<p.y<<" "<<p.z<<std::endl;
        glPushMatrix();
        glTranslated(p.x, p.y, p.z);
        glutSolidSphere(h, 10, 10);
        
        glPopMatrix();
    }
    
    glPopAttrib();
}

void
Viewer::drawVerts()
{
    for (unsigned i = 0; i < meshPtr->numVerts(); ++i)
    {
        Vec3 p = meshPtr->getVertPos(i);
        glLoadName(i);
        glBegin(GL_POINTS);
        glVertex3d(p.x, p.y, p.z);
        glEnd();
    }
}

void
Viewer::pickVertex(int x, int y)
{
    std::cout<<"called"<<std::endl;
    int width  = glutGet(GLUT_WINDOW_WIDTH );
    int height = glutGet(GLUT_WINDOW_HEIGHT);
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    
    int bufSize = meshPtr->numVerts();
    GLuint* buf = new GLuint[bufSize];
    glSelectBuffer(bufSize, buf);
    
    GLint viewport[4];
    GLdouble projection[16];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    
    glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(0);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix(x, viewport[3]-y, 10, 10, viewport);
    glMultMatrixd(projection);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    drawVerts();
    glPopMatrix();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    
    glMatrixMode(GL_MODELVIEW);
    long hits = glRenderMode(GL_RENDER);
    std::cout<<hits<<std::endl;
    int index = -1;
    double min_z = 1.0e100;
    for( long i = 0; i < hits; ++i )
    {
        double distance = buf[4*i + 1];
        if (distance < min_z)
        {
            index = buf[4*i + 3];
            min_z = distance;
        }
    }
    delete[] buf;
    if (index < 0) return;
    
    std::set<unsigned>::iterator it = selectedVert.find(index);
    selectedVert.clear();
    selectedVert.insert(index);
    
    Vec3 p = meshPtr->getVertPos(index);
    std::vector<Vec3> vertNormal;
    meshPtr->computeVertNormals(vertNormal);
    Vec3 nm = vertNormal[index];
    std::cout<<p.x<<" "<<p.y<<" "<<p.z<<std::endl;
    selectedVertDeformation(p, nm);
    
    updateDisplayList();
}

void
Viewer::clearData()
{
    selectedVert.clear();
}

void
Viewer::pickCenter()
{
    unsigned index = meshPtr->numVerts();
    double minVal = std::numeric_limits<int>::max();
    for (unsigned vert = 0; vert < meshPtr->numVerts(); ++vert)
    {
        Vec3 p = meshPtr->getVertPos(vert);
        double val = p.norm();
        if (val < minVal)
        {
            minVal = val;
            index = vert;
        }
    }
    selectedVert.insert(index);
}


void Viewer::takeScreenshot()
{
    static int index = 0;
    
    GLint view[4];
    glGetIntegerv(GL_VIEWPORT, view);
    int w = view[2];
    int h = view[3];
    
    // get pixels
    Image image(w, h);
    glReadPixels(0, 0, w, h, GL_BGR, GL_FLOAT, &image(0,0));
    
    std::stringstream filename;
    filename << "snapshot" << index << ".tga";
    image.write(filename.str().c_str());
    std::cout << "snapshot " << index << " taken" << std::endl;
    
    index++;
}

//added

void createVTKFile(const std::string& outFileName,
                   unsigned int nx, unsigned int ny, unsigned int nz,
                   const std::vector<Vector3>& grid,
                   const std::vector<double>& data)
{
    std::ofstream out(outFileName.c_str());
    
    // header
    out << "# vtk DataFile Version 3.0" << std::endl;
    out << "vtk output" << std::endl;
    out << "ASCII" << std::endl;
    out << "DATASET STRUCTURED_GRID" << std::endl;
    out << "DIMENSIONS " <<
    nx << " " <<
    ny << " " <<
    nz << std::endl;
    out << "POINTS " << nx*ny*nz << " double" << std::endl;
    
    // structured grid
    std::vector<Vector3>::const_iterator it;
    for (it = grid.begin(); it != grid.end(); ++it) {
        Vector3 curr = *it;
        out << curr[0] << " " << curr[1] << " " << curr[2] << std::endl;
    }
    out << std::endl;
    
    // data
    // header
    out << std::endl;
    out << "POINT_DATA " << nx*ny*nz << std::endl;
    out << "SCALARS Density double" << std::endl;
    out << "LOOKUP_TABLE default" << std::endl;
    
    // data
    std::vector<double>::const_iterator datait;
    for (datait = data.begin(); datait != data.end(); ++datait) {
        out << *datait << std::endl;
    }
    
    out << std::endl;
    
    out.close();
}

/*void createVTKFile(const std::string& outFileName,
                   const POINT3D *m_ppt3dVertices,int m_nVertices,
                   const unsigned int *m_piTriangleIndices,int m_piTriangleIndices)
{
    std::ofstream out(outFileName.c_str());
    
    // header
    out << "# vtk DataFile Version 3.0" << std::endl;
    out << "vtk output" << std::endl;
    out << "ASCII" << std::endl;
    out << "DATASET STRUCTURED_GRID" << std::endl;
    out << "DIMENSIONS " <<

    out << "POINTS " << m_nVertices << " double" << std::endl;
    
    // structured grid
    double x,y,z;
    for(int i=0;i<m_nVertices;i++){
        x=(double)m_ppt3dVertices[i][0];
        y=(double)m_ppt3dVertices[i][1];
        z=(double)m_ppt3dVertices[i][2];
        out << x << " " << y << " " << z << std::endl;
    }
    out << std::endl;
    
    // data
    // header
    out << std::endl;
    out << "POINT_DATA " << m_nVertices << std::endl;
    out << "SCALARS Density double" << std::endl;
    out << "LOOKUP_TABLE default" << std::endl;
    
    // data
    for(int i=0;i<m_nTriangles;i++){
        std::vector<unsigned> face;
        face.push_back(m_piTriangleIndices[i*3]);
        face.push_back(m_piTriangleIndices[i*3+1]);
        face.push_back(m_piTriangleIndices[i*3+2]);
        out << m_piTriangleIndices[i*3] << " " << m_piTriangleIndices[i*3+1] << " " << m_piTriangleIndices[i*3+2] << std::endl;
    }
    
    out << std::endl;
    
    out.close();
}*/

void createOFFFile(const std::string& outFileName,std::vector<Vector3>& p, std::vector<Vector3>& n)
{
    std::vector<Vector3>::const_iterator it;
    std::vector<Vector3>::const_iterator it2;
    std::ofstream out(outFileName.c_str());
    it2=n.begin();
    for (it = p.begin(); it != p.end(); ++it) {
        Vector3 curr = *it;
        Vector3 curr2= *it2;
        out << curr[0] << " " << curr[1] << " " << curr[2] <<" "<<curr2[0]<<" "<<curr2[1]<<" "<<curr2[2]<< std::endl;
        it2++;
    }
    
    out.close();
}

void createOFFFile2(const std::string& outFileName){
    std::ofstream out(outFileName.c_str());
    out<<"OFF"<<std::endl;
    out<<ciso->m_nVertices<<" "<<ciso->m_nTriangles<<" "<<0<<std::endl;
    for(unsigned int i=0;i<ciso->m_nVertices;i++){
        out<<ciso->m_ppt3dVertices[i][0]<<" "<<ciso->m_ppt3dVertices[i][1]<<" "<<ciso->m_ppt3dVertices[i][2]<<std::endl;
    }
    for(unsigned int i=0;i<ciso->m_nTriangles;i++){
        out<<"3"<<" "<<ciso->m_piTriangleIndices[i*3]<<" "<<ciso->m_piTriangleIndices[i*3+1]<<" "<<ciso->m_piTriangleIndices[i*3+2]<<std::endl;
    }
}


void createOFFFileForPolyhedron(const std::string& outFileName, SurfaceMesh& output_mesh){
    typedef typename SurfaceMesh::Vertex_const_iterator VCI;
    typedef typename SurfaceMesh::Facet_const_iterator FCI;
    typedef typename SurfaceMesh::Halfedge_around_facet_const_circulator HFCC;
    std::ofstream out(outFileName.c_str());
    out<<"OFF"<<std::endl;
    out<<output_mesh.size_of_vertices()<<" "<<output_mesh.size_of_facets()<<" "<<0<<std::endl;
    for (VCI vi = output_mesh.vertices_begin();vi != output_mesh.vertices_end();++vi){
        out<<CGAL::to_double(vi->point().x())<<" "<<CGAL::to_double(vi->point().y())<<" "<<CGAL::to_double(vi->point().z())<<std::endl;
    }
    typedef CGAL::Inverse_index<VCI> Index;
    Index index(output_mesh.vertices_begin(), output_mesh.vertices_end());
    for (FCI fi = output_mesh.facets_begin();fi != output_mesh.facets_end();++fi){
        HFCC hc = fi->facet_begin();
        HFCC hc_end = hc;
        out<<"3";
        do {
            out<<" ";
            out<<index[VCI(hc->vertex())];
            ++hc;
        } while(hc != hc_end);
        out<<std::endl;
    }
}

void createOBJFile(const std::string& outFileName, SurfaceMesh& output_mesh){
    typedef typename SurfaceMesh::Vertex_const_iterator VCI;
    typedef typename SurfaceMesh::Facet_const_iterator FCI;
    typedef typename SurfaceMesh::Halfedge_around_facet_const_circulator HFCC;
    std::ofstream out(outFileName.c_str());
    for (VCI vi = output_mesh.vertices_begin();vi != output_mesh.vertices_end();++vi){
        out<<"v "<<CGAL::to_double(vi->point().x())<<" "<<CGAL::to_double(vi->point().y())<<" "<<CGAL::to_double(vi->point().z())<<std::endl;
    }
    typedef CGAL::Inverse_index<VCI> Index;
    Index index(output_mesh.vertices_begin(), output_mesh.vertices_end());
    for (FCI fi = output_mesh.facets_begin();fi != output_mesh.facets_end();++fi){
        HFCC hc = fi->facet_begin();
        HFCC hc_end = hc;
        out<<"f";
        do {
            out<<" ";
            out<<index[VCI(hc->vertex())];
            ++hc;
        } while(hc != hc_end);
        out<<std::endl;
    }
}

void createOBJFile_marching(const std::string& outFileName){
    std::ofstream out(outFileName.c_str());
    for(unsigned int i=0;i<ciso->m_nVertices;i++){
        out<<"v "<<ciso->m_ppt3dVertices[i][0]<<" "<<ciso->m_ppt3dVertices[i][1]<<" "<<ciso->m_ppt3dVertices[i][2]<<std::endl;
    }
    for(unsigned int i=0;i<ciso->m_nTriangles;i++){
        out<<"f"<<" "<<ciso->m_piTriangleIndices[i*3]<<" "<<ciso->m_piTriangleIndices[i*3+1]<<" "<<ciso->m_piTriangleIndices[i*3+2]<<std::endl;
    }
}
void Viewer::colorCoding(std::vector<Point> f){
    int color=0;
    std::cout<<"f="<<f[0]<<std::endl;
    for(int i=0;i<meshPtr->numSeg();i++){
        for(int j=0;j<pointsSets[i].size();j++){
            if(f[0]==Point(pointsSets[i][j].x(),pointsSets[i][j].y(),pointsSets[i][j].z())){
                color=i;
                std::cout<<"color="<<color<<std::endl;
            }
        }
    }
    if(color==0){
        meshPtr->colors.push_back(1.0);
        meshPtr->colors.push_back(1.0);
        meshPtr->colors.push_back(1.0);
    }
    else if(color==1){
        meshPtr->colors.push_back(1.0);
        meshPtr->colors.push_back(0.0);
        meshPtr->colors.push_back(0.0);
    }
    else if(color==2){
        meshPtr->colors.push_back(0.0);
        meshPtr->colors.push_back(1.0);
        meshPtr->colors.push_back(0.0);
    }
    else if(color==3){
        meshPtr->colors.push_back(0.0);
        meshPtr->colors.push_back(0.0);
        meshPtr->colors.push_back(1.0);
    }
    else if(color==4){
        meshPtr->colors.push_back(1.0);
        meshPtr->colors.push_back(0.0);
        meshPtr->colors.push_back(1.0);
    }
    else if(color==5){
        meshPtr->colors.push_back(1.0);
        meshPtr->colors.push_back(1.0);
        meshPtr->colors.push_back(0.0);
    }
    else if(color==6){
        meshPtr->colors.push_back(0.0);
        meshPtr->colors.push_back(1.0);
        meshPtr->colors.push_back(1.0);
    }
    else if(color==7){
        meshPtr->colors.push_back(1.0);
        meshPtr->colors.push_back(0.5);
        meshPtr->colors.push_back(0.0);
    }
    else if(color==8){
        meshPtr->colors.push_back(0.0);
        meshPtr->colors.push_back(1.0);
        meshPtr->colors.push_back(0.5);
    }
    else if(color==9){
        meshPtr->colors.push_back(0.5);
        meshPtr->colors.push_back(1.0);
        meshPtr->colors.push_back(0.0);
    }
}
 void
Viewer::setMeshFromPolyhedron(SurfaceMesh& output_mesh,
                      TriMesh* meshPtr)
{
    typedef typename SurfaceMesh::Vertex_const_iterator VCI;
    typedef typename SurfaceMesh::Facet_const_iterator FCI;
    typedef typename SurfaceMesh::Halfedge_around_facet_const_circulator HFCC;
    
    std::vector<Vec3> vertices;
    std::vector< std::vector<unsigned> > faces;
    const auto startTime = std::chrono::system_clock::now();
    for (VCI vi = output_mesh.vertices_begin();
         vi != output_mesh.vertices_end();
         ++vi)
    {
        Vec3 v(CGAL::to_double(vi->point().x()),
               CGAL::to_double(vi->point().y()),
               CGAL::to_double(vi->point().z()));
        vertices.push_back(v);

    }
    

    typedef CGAL::Inverse_index<VCI> Index;
    Index index(output_mesh.vertices_begin(), output_mesh.vertices_end());
    
    for (FCI fi = output_mesh.facets_begin();
         fi != output_mesh.facets_end();
         ++fi)
    {
        HFCC hc = fi->facet_begin();
        HFCC hc_end = hc;
        std::vector<unsigned> f;
        std::vector<Point> fForColorCoding;
        do {
            f.push_back(index[VCI(hc->vertex())]);
            //fForColorCoding.push_back(hc->vertex()->point());
            ++hc;
        } while(hc != hc_end);
        //Viewer::colorCoding(fForColorCoding);
        //fForColorCoding.erase(fForColorCoding.begin(),fForColorCoding.end());
        faces.push_back(f);
    }
    std::cout << "# of vertices and faces: " << std::endl;
    std::cout << vertices.size() << std::endl;
    std::cout << faces.size() << std::endl;
    const auto endTime = std::chrono::system_clock::now();
    const auto timeSpan = endTime - startTime;
    std::cout << "setMeshFromPolyhedoron's time:" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
    meshPtr->setData(vertices, faces);
    //meshPtr->normalize();
}
void Viewer::normalize()
{
    Vector3 c(0.0,0.0,0.0);
    int count=0;
    for(int i=0;i<meshPtr->numSeg();i++){
        for(int j=0;j<pointsSets[i].size();j++){
            c += pointsSets[i][j];
            count++;
        }
    }
    
    c /= count;
    for(int i=0;i<meshPtr->numSeg();i++){
        for(int j=0;j<pointsSets[i].size();j++){
            pointsSets[i][j] -= c;
            count++;
        }
    }
    
    double scale = std::sqrt(meshPtr->computeTotalArea());
    for(int i=0;i<meshPtr->numSeg();i++){
        for(int j=0;j<pointsSets[i].size();j++){
            pointsSets[i][j] /= scale;
        }
    }
}

static void
setMeshFromPolyhedron(Polyhedron2& output_mesh,TriMesh* meshPtr)
{
    typedef typename Polyhedron2::Vertex_const_iterator VCI;
    typedef typename Polyhedron2::Facet_const_iterator FCI;
    typedef typename Polyhedron2::Halfedge_around_facet_const_circulator HFCC;
    
    std::vector<Vec3> vertices;
    std::vector< std::vector<unsigned> > faces;
    
    for (VCI vi = output_mesh.vertices_begin();
         vi != output_mesh.vertices_end();
         ++vi)
    {
        Vec3 v(CGAL::to_double(vi->point().x()),
               CGAL::to_double(vi->point().y()),
               CGAL::to_double(vi->point().z()));
    }
    
    typedef CGAL::Inverse_index<VCI> Index;
    Index index(output_mesh.vertices_begin(), output_mesh.vertices_end());
    
    for (FCI fi = output_mesh.facets_begin();
         fi != output_mesh.facets_end();
         ++fi)
    {
        HFCC hc = fi->facet_begin();
        HFCC hc_end = hc;
        std::vector<unsigned> f;
        do {
            f.push_back(index[VCI(hc->vertex())]);
            ++hc;
        } while(hc != hc_end);
        
        faces.push_back(f);
    }
    std::cout << "# of vertices and faces: " << std::endl;
    std::cout << vertices.size() << std::endl;
    std::cout << faces.size() << std::endl;
    
    meshPtr->setData(vertices, faces);
    meshPtr->normalize();
}

void Viewer::read(const char* filename){
    std::string line;
    std::ifstream in(filename);
    while(getline(in, line))
    {
        std::stringstream ss(line);
        std::string token;
        ss >> token;
        if (token == "reconstruction(Hrbf=0,closedHrbf=1,Poisson=2)")
        {
            ss >> reconstructionValue;
            std::cout<<"reconstructionValue"<<reconstructionValue<<std::endl;
            continue;
        }
        if(token=="threshold")
        {
            ss>>thr;
            continue;
        }
        if(token=="sigma"){
            ss>>sigma;
            continue;
        }
        if(token=="nb_neighbors"){
            ss>>nb_neighbors;
            continue;
        }
        if(token=="nb_neighbors2"){
            ss>>nb_neighbors2;
            continue;
        }
        if(token=="sm_angle"){
            double angle=0;
            ss>>angle;
            sm_angle=angle;
            continue;
        }
        if(token=="sm_radius"){
            double radius=0;
            ss>>radius;
            sm_radius=radius;
            continue;
        }
        if(token=="sm_distance"){
            double distance=0;
            ss>>distance;
            sm_distance=distance;
            continue;
        }
        if(token=="resampling(off=0,on=1)"){
            ss>>resamplingSwitch;
            continue;
        }
        if(token=="mesher(marching=0,OurDelaunay=1,CGAL’sDelaunay=2)"){
            ss>>mesher;
            continue;
        }
        if(token=="gridsize"){
            ss>>gridsize;
            continue;
        }
        if(token=="Cell_inside_value"){
            ss>>cellInsideValue;
            continue;
        }
        if(token=="bounding_box_size"){
            ss>>boundingBox;
            continue;
        }
        
        if(token=="union(R-function=0,min=1,mymin=2)"){
            ss>>unionValue;
            continue;
        }
        if(token=="epsilonForMarchingBounding"){
            ss>>epsilonForMarchingBounding;
            continue;
        }
        if(token=="replaceSwitch"){
            ss>>replaceSwitch;
            continue;
        }
    }
    
    in.close();
}


Delaunay resampling(SurfaceMesh& output_mesh,TriMesh* meshPtr,double length,Delaunay dl)
{
    typedef typename SurfaceMesh::Vertex_const_iterator VCI;
    typedef typename SurfaceMesh::Facet_const_iterator FCI;
    typedef typename SurfaceMesh::Halfedge_around_facet_const_circulator HFCC;
    
    std::vector<Vec3> vertices;
    std::vector< std::vector<unsigned> > faces;
    
    for (VCI vi = output_mesh.vertices_begin();
         vi != output_mesh.vertices_end();
         ++vi)
    {
        Vec3 v(CGAL::to_double(vi->point().x()),
               CGAL::to_double(vi->point().y()),
               CGAL::to_double(vi->point().z()));
        vertices.push_back(v);
    }
    
    typedef CGAL::Inverse_index<VCI> Index;
    Index index(output_mesh.vertices_begin(), output_mesh.vertices_end());
    
    for (FCI fi = output_mesh.facets_begin();
         fi != output_mesh.facets_end();
         ++fi)
    {
        HFCC hc = fi->facet_begin();
        HFCC hc_end = hc;
        std::vector<unsigned> f;
        do {
            f.push_back(index[VCI(hc->vertex())]);
            ++hc;
        } while(hc != hc_end);
        
        faces.push_back(f);
    }
    double pointX,pointY,pointZ;
    double dist0;
    double dist1;
    double dist2;
    for(std::size_t i=0;i<vertices.size();i++){
        
        dist0=sqrt(std::pow(vertices[faces[i][0]].x-vertices[faces[i][1]].x,2)+std::pow(vertices[faces[i][0]].y-vertices[faces[i][1]].y,2)+std::pow(vertices[faces[i][0]].z-vertices[faces[i][1]].z,2));
        dist1=sqrt(pow(vertices[faces[i][1]].x-vertices[faces[i][2]].x,2)+std::pow(vertices[faces[i][1]].y-vertices[faces[i][2]].y,2)+std::pow(vertices[faces[i][1]].z-vertices[faces[i][2]].z,2));
        dist2=sqrt(std::pow(vertices[faces[i][2]].x-vertices[faces[i][0]].x,2)+std::pow(vertices[faces[i][2]].y-vertices[faces[i][0]].y,2)+std::pow(vertices[faces[i][2]].z-vertices[faces[i][0]].z,2));
        if(dist0>length||dist1>length||dist2>length){
            pointX=vertices[faces[i][0]].x+vertices[faces[i][1]].x+vertices[faces[i][2]].x/3;
            pointY=vertices[faces[i][0]].y+vertices[faces[i][1]].y+vertices[faces[i][2]].y/3;
            pointZ=vertices[faces[i][0]].z+vertices[faces[i][1]].z+vertices[faces[i][2]].z/3;
            dl.insert(Delaunay::Point(pointX, pointY,pointZ));
        }
    }
    // setMeshFromPolyhedron(output_mesh, meshPtr);
    return dl;
    
}



Hrbf_function HRBF_reconstruction(std::vector<Vector3>& points,std::vector<Vector3>& normals,TriMesh* meshPtr){
    std::cout<<"HRBF mode"<<std::endl;
    HRBF_fit<double, 3, Rbf_pow3<double> > hrbf;
    hrbf.hermite_fit(points, normals);
    PointList pt;
    for(std::size_t i=0;i<points.size();i++){
        pt.push_back(Point(points[i][0], points[i][1], points[i][2]));
    }
    Hrbf_function function(hrbf);
    if(mesher==0){
        const auto startTime = std::chrono::system_clock::now();
        std::cout<<"Marching start"<<std::endl;
        for (size_t i = 0; i < mcgrid.structuredGrid.size(); ++i) {
            mcgrid.results[i] = hrbf.eval(mcgrid.structuredGrid[i]);
        }
        
        double *resultarray= new double[mcgrid.results.size()];
        for(int i=0;i<mcgrid.results.size();i++){
            resultarray[i] = mcgrid.results[i];
        }
        
        ciso->GenerateSurface(resultarray, 0,
                              mcgrid.subx-1, mcgrid.suby-1, mcgrid.subz-1,
                              mcgrid.dx, mcgrid.dy, mcgrid.dz);
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "Marching cube's time:" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
        delete[] resultarray;
        
        meshPtr->setData(ciso->m_ppt3dVertices, ciso->m_nVertices,
                         ciso->m_piTriangleIndices, ciso->m_nTriangles);
        //meshPtr->normalize();
        createOFFFile2("out.off");
        //createOBJFile_marching("out.obj");
    }
    return function;
}

Crbf_function HRBF_closed_reconstruction(std::vector<PointVectorPair>& points,std::vector<Vector3>& points2,std::vector<Vector3>& normals2,TriMesh* meshPtr){
    std::cout<<"closed rbf mode"<<std::endl;
    double rho = CGAL::to_double(CGAL::compute_average_spacing<Concurrency_tag>(points.begin(), points.end(),CGAL::First_of_pair_property_map<PointVectorPair>(),nb_neighbors2));
    rho = rho * 5;
    double eta = 50.0 / (rho*rho);
    std::cout << "eta: " << eta << " ; rho: " << rho << std::endl;
    // Evaluate on the grid
    
    HRBF_closed crbf(mcgrid.structuredGrid, normals2, points2, rho, eta);
    PointList pt;
    for(std::size_t i=0;i<points2.size();i++){
        pt.push_back(Point(points2[i][0], points2[i][1], points2[i][2]));
    }
    Crbf_function function(crbf);
    if(mesher==0){
        const auto startTime = std::chrono::system_clock::now();
        std::cout<<"Marching start"<<std::endl;
        for (size_t i = 0; i < mcgrid.structuredGrid.size(); ++i) {
            mcgrid.results[i] = crbf.eval(mcgrid.structuredGrid[i]);
        }
        double *resultarray= new double[mcgrid.results.size()];
        for(unsigned int i=0;i<mcgrid.results.size();i++){
            resultarray[i] = mcgrid.results[i];
        }
        
        ciso->GenerateSurface(resultarray, 0,
                              mcgrid.subx-1, mcgrid.suby-1, mcgrid.subz-1,
                              mcgrid.dx, mcgrid.dy, mcgrid.dz);
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "Marching cube's time:" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
        delete[] resultarray;
        
        meshPtr->setData(ciso->m_ppt3dVertices, ciso->m_nVertices,
                         ciso->m_piTriangleIndices, ciso->m_nTriangles);
       // meshPtr->normalize();
        createOFFFile2("out.off");
        //createOBJFile_marching("out.obj");
    }
    return function;
}
Poisson_reconstruction_function Poisson_reconstruction(std::vector<Vector3>& points,std::vector<Vector3>& normals,TriMesh* meshPtr){
    std::cout<<"Poisson mode"<<std::endl;
    PointNormalList pwn;
    for (std::size_t i = 0; i < points.size(); ++i) {

        Point pt(points[i][0], points[i][1], points[i][2]);
        Vector nm(normals[i][0], normals[i][1], normals[i][2]);
        Point_with_normal_3 pn(pt, nm);
        pwn.push_back(pn);
    }

    Poisson_reconstruction_function
    function(pwn.begin(), pwn.end(),
             CGAL::make_normal_of_point_with_normal_pmap(PointList::value_type()));
    if (!function.compute_implicit_function())exit(EXIT_FAILURE);
   /* if(mesher==0){
        const auto startTime = std::chrono::system_clock::now();
        std::cout<<"Marching start"<<std::endl;
        for (size_t i = 0; i < mcgrid.structuredGrid.size(); ++i) {
            mcgrid.results[i] = CGAL::to_double(function(Point(mcgrid.structuredGrid[i](0),mcgrid.structuredGrid[i](1),mcgrid.structuredGrid[i](2))));
            std::cout<<"marching "<<mcgrid.results[i]<<std::endl;
        }
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "Marching cube's time:" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
        double *resultarray= new double[mcgrid.results.size()];
        for(unsigned int i=0;i<mcgrid.results.size();i++){
            resultarray[i] = mcgrid.results[i];
        }
        ciso->GenerateSurface(resultarray, 0,
                              mcgrid.subx-1, mcgrid.suby-1, mcgrid.subz-1,
                              mcgrid.dx, mcgrid.dy, mcgrid.dz);
        /*const auto endTime = std::chrono::system_clock::now();
         const auto timeSpan = endTime - startTime;
         std::cout << "Marching cube's time:" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;*/
       // delete[] resultarray;
        
        //meshPtr->setData(ciso->m_ppt3dVertices, ciso->m_nVertices,
                        // ciso->m_piTriangleIndices, ciso->m_nTriangles);
        //meshPtr->normalize();
        
        //createOFFFile2("out.off");
        //createOBJFile_marching("out.obj");
        
    //}
    
    return function;
}
Poisson_reconstruction_function Poisson_reconstruction(TriMesh* mesh){
    
    std::vector<PointVectorPair> points;
    for (unsigned i = 0; i < mesh->numVerts(); ++i) {
        Vec3 v = mesh->getVertPos(i);
        Point p(v.x, v.y, v.z);
        Vector tmp(0,0,0);
        points.push_back(std::make_pair(p, tmp));
    }

    
    CGAL::pca_estimate_normals<Concurrency_tag>(points.begin(), points.end(), CGAL::First_of_pair_property_map<PointVectorPair>(), CGAL::Second_of_pair_property_map<PointVectorPair>(), 18);
    std::vector<PointVectorPair>::iterator unoriented_points_begin =
    CGAL::mst_orient_normals(points.begin(), points.end(),
                             CGAL::First_of_pair_property_map<PointVectorPair>(),
                             CGAL::Second_of_pair_property_map<PointVectorPair>(),
                             18);
    
    points.erase(unoriented_points_begin, points.end());
    
    PointNormalList pwn;

    for (std::size_t i = 0; i < points.size(); ++i) {
        Point pt(points[i].first);
        Vector nm(points[i].second);
        Point_with_normal_3 pn(pt, nm);
        pwn.push_back(pn);
    }


      Eigen_solver solver;
    Poisson_reconstruction_function
    function(pwn.begin(), pwn.end(),
             CGAL::make_normal_of_point_with_normal_pmap(PointList::value_type()));
        std::cout<<"mesh->numVerts="<<mesh->numVerts()<<std::endl;
    if (!function.compute_implicit_function(true))exit(EXIT_FAILURE);
            std::cout<<"mesh->numVerts="<<mesh->numVerts()<<std::endl;
        return function;
}

void create_extra_points(TriMesh& mesh, std::vector<Point>& extra_points) {
    
    std::vector<PointVectorPair> points;
    for (unsigned i = 0; i < mesh.numVerts(); ++i) {
        Vec3 v = mesh.getVertPos(i);
        Point p(v.x, v.y, v.z);
        Vector tmp(0,0,0);
        points.push_back(std::make_pair(p, tmp));
    }
    
    CGAL::pca_estimate_normals<Concurrency_tag>(points.begin(), points.end(), CGAL::First_of_pair_property_map<PointVectorPair>(), CGAL::Second_of_pair_property_map<PointVectorPair>(), 18);
    
    std::vector<PointVectorPair>::iterator unoriented_points_begin =
    CGAL::mst_orient_normals(points.begin(), points.end(),
                             CGAL::First_of_pair_property_map<PointVectorPair>(),
                             CGAL::Second_of_pair_property_map<PointVectorPair>(),
                             18);
    
    points.erase(unoriented_points_begin, points.end());
    
    for (std::size_t i = 0; i < points.size(); ++i) {
        Point pt(points[i].first);
        Vector nm(points[i].second);
        Point pt2(pt.x() + nm.x()*5.0, pt.y() + nm.y()*5.0, pt.z() + nm.z());
        extra_points.push_back(pt2);
    }
    
}




void Viewer::MarchingCubesForEachFunction(){
    const auto startTime = std::chrono::system_clock::now();
    mcgrid.results.erase(mcgrid.results.begin(),mcgrid.results.end());
    for(int count=0;count<functionSets.size();count++){
    for (size_t i = 0; i < mcgrid.structuredGrid.size(); ++i) {
        mcgrid.results.push_back(functionSets[count](Point(mcgrid.structuredGrid[i](0),mcgrid.structuredGrid[i](1),mcgrid.structuredGrid[i](2))));
    }
        char filename[20];
        snprintf(filename,sizeof(filename),"MC_poisson+%d.vtk",count);
    createVTKFile(filename,mcgrid.subx,mcgrid.suby,mcgrid.subz,mcgrid.structuredGrid,mcgrid.results);
    std::cout<<"drawed"<<std::endl;
                std::cout<<"mcgrid.results.size()="<<mcgrid.results.size()<<std::endl;
        mcgrid.results.erase(mcgrid.results.begin(),mcgrid.results.end());
        std::cout<<"mcgrid.results.size()="<<mcgrid.results.size()<<std::endl;
    }
}
void fill_poly_1(Polyhedron2& poly)
{
    std::ifstream input("out.off");
    if ( !input || !(input >> poly) || poly.empty() ) {
        std::cerr << "Not a valid off file." << std::endl;
        //  return EXIT_FAILURE;
    }
}
void fill_poly_2(Polyhedron2& poly)
{
    std::ifstream input("meshForReplacePolyhedron.off");
    if ( !input || !(input >> poly) || poly.empty() ) {
        std::cerr << "Not a valid off file." << std::endl;
        //  return EXIT_FAILURE;
    }
    
}

Polyhedron2 takeUnitPolyhedron(){
    Polyhedron2 poly1,poly2,outpoly;
    
    fill_poly_1(poly1);
    fill_poly_2(poly2);
    
    Nef_polyhedron nef1(poly1);
    Nef_polyhedron nef2(poly2);
    Nef_polyhedron nef=nef1+nef2;
    if(nef.is_simple()) {
        nef.convert_to_polyhedron(outpoly);
    }
    else
        std::cerr << "N1 is not a 2-manifold." << std::endl;
    return outpoly;
}

void OutputSelectedPart(const std::string& outFileName, int segnum){
    std::ofstream out(outFileName.c_str());
    out<<"txt"<<std::endl;
    for (int i=0;i<pointsSets[segnum].size();++i){
        out<<pointsSets[segnum][i].x()<<" "<<pointsSets[segnum][i].y()<<" "<<pointsSets[segnum][i].z()<<std::endl;
    }
}


void Viewer::initSeg(){
       std::vector<PointVectorPair> points;
       std::vector<Vec3> vec3Points;
    for(unsigned i=0;i<meshPtr->numVerts();i++){
        Vec3 p_neighbor = meshPtr->getVertPos(i);
        vec3Points.push_back(p_neighbor);
    }
    meshPtr->replacePoints(vec3Points);
    meshPtr->makeMap();
    
    std::set<Point> settmp;
    for(int i=0;i<meshPtr->numSeg();i++){
        std::cout<<"meshPtr->numSeg()"<<i<<std::endl;
        std::pair<std::multimap<int,Point>::iterator, std::multimap<int,Point>::iterator> p = meshPtr->getEqual_range(i);
        for(std::multimap<int,Point>::iterator it = p.first;it!=p.second;it++){
            //std::cout<<"segpoints[i]="<<it->second<<std::endl;
            
            settmp.insert(it->second);
        }
        //segPoints.push_back(settmp);
        
        
        for(auto itr=settmp.begin();itr!=settmp.end();itr++){
            points.push_back(std::make_pair(*itr, Vector(0,0,0)));
        }
        
        CGAL::pca_estimate_normals<Concurrency_tag>(points.begin(), points.end(), CGAL::First_of_pair_property_map<PointVectorPair>(), CGAL::Second_of_pair_property_map<PointVectorPair>(), nb_neighbors);
        
        std::vector<PointVectorPair>::iterator unoriented_points_begin =
        CGAL::mst_orient_normals(points.begin(), points.end(),
                                 CGAL::First_of_pair_property_map<PointVectorPair>(),
                                 CGAL::Second_of_pair_property_map<PointVectorPair>(),
                                 nb_neighbors);
        std::vector<Vector3> points2;
        std::vector<Vector3> normals2;
        for(int j=0;j<points.size();j++){
            points2.push_back(Vector3(points[j].first.x(),points[j].first.y(),points[j].first.z()));
            normals2.push_back(Vector3(points[j].second.x(),points[j].second.y(),points[j].second.z()));
        }
        pointsSets.push_back(points2);
        normalsSets.push_back(normals2);
        points.erase(points.begin(),points.end());
        points2.erase(points2.begin(),points2.end());
        normals2.erase(normals2.begin(),normals2.end());
        settmp.erase(settmp.begin(),settmp.end());
    }
    
    for(int i=0;i<meshPtr->numSeg();i++){
        for(int j=0;j<pointsSets[i].size();j++){
            PointForTree pointtree;
            pointtree.vec[0]=pointsSets[i][j].x();
            pointtree.vec[1]=pointsSets[i][j].y();
            pointtree.vec[2]=pointsSets[i][j].z();
            pointtree.seg=i;
            pointtree.normalX=normalsSets[i][j].x();
            pointtree.normalY=normalsSets[i][j].y();
            pointtree.normalZ=normalsSets[i][j].z();
            pointForTree.push_back(pointtree);
        }
    }
    std::vector<Vector3> pointsVector3;
    std::vector<Vector3> normalsVector3;
    for(int i=0;i<meshPtr->numSeg();i++){
        for(int j=0;j<pointsSets[i].size();j++){
            pointsVector3.push_back(pointsSets[i][j]);
            normalsVector3.push_back(normalsSets[i][j]);
        }
        std::cout<<"i="<<i<<std::endl;
        functionSets.push_back(Poisson_reconstruction(pointsVector3,normalsVector3,meshPtr));
        pointsVector3.erase(pointsVector3.begin(),pointsVector3.end());
        normalsVector3.erase(normalsVector3.begin(),normalsVector3.end());
    }
    for(int i=0;i<meshPtr->numSeg();i++){
        for(int j=0;j<pointsSets[i].size();j++){
            std::cout<<"func="<<functionSets[i](Point(pointsSets[i][j].x(),pointsSets[i][j].y(),pointsSets[i][j].z()))<<std::endl;
           // if(functionSets[i](Point(pointsSets[i][j].x(),pointsSets[i][j].y(),pointsSets[i][j].z()))<=0){
                
                minPoints.push_back(pointsSets[i][j]);
                minNormals.push_back(normalsSets[i][j]);
         //   }
        }
    }
    std::cout<<"minpoints="<<minPoints.size()<<std::endl;
    
    std::vector<Point> points3;
    for(unsigned i=0; i<minPoints.size(); i++){
        points3.push_back(Point(minPoints[i].x(), minPoints[i].y(),
                                minPoints[i].z()));
    }
    
    double averagespacing = CGAL::compute_average_spacing<Concurrency_tag>(points3.begin(), points3.end(),CGAL::Identity_property_map<Point>() ,nb_neighbors2);
    originalFunction.push_back(Poisson_reconstruction(minPoints,minNormals,meshPtr));
}


void
Viewer::selectedVertDeformation(Vec3& selected_point,
                                Vec3& selected_normal)
{
     PointList vertices;
    std::vector<PointVectorPair> points,pointsForpart;
    int selectedSeg;
    //const unsigned int K=1;
    double selected_x = selected_point.x;
    double selected_y = selected_point.y;
    double selected_z = selected_point.z;
    
    double normal_x = selected_normal.x;
    double normal_y = selected_normal.y;
    double normal_z = selected_normal.z;
    double distance,disp;
    std::vector<Vector3> points2;
    std::vector<Vector3> normals2;

    std::vector<Vec3> vec3Points;

    
    meshPtr->returnVertposi();
    for(unsigned i=0;i<meshPtr->numVerts();i++){
        Vec3 p_neighbor = meshPtr->getVertPos(i);
        vec3Points.push_back(p_neighbor);
    }
    meshPtr->replacePoints(vec3Points);
    meshPtr->makeMap();
    std::cout<<"minpoints="<<minPoints.size()<<std::endl;
    
    std::vector<Point> points3;
    for(unsigned i=0; i<minPoints.size(); i++){
        points3.push_back(Point(minPoints[i].x(), minPoints[i].y(),
                                minPoints[i].z()));
    }
    pointForTree.erase(pointForTree.begin(),pointForTree.end());
    for(int i=0;i<meshPtr->numSeg();i++){
        for(int j=0;j<pointsSets[i].size();j++){
            PointForTree pointtree;
            pointtree.vec[0]=pointsSets[i][j].x();
            pointtree.vec[1]=pointsSets[i][j].y();
            pointtree.vec[2]=pointsSets[i][j].z();
            pointtree.seg=i;
            pointtree.normalX=normalsSets[i][j].x();
            pointtree.normalY=normalsSets[i][j].y();
            pointtree.normalZ=normalsSets[i][j].z();
            pointForTree.push_back(pointtree);
        }
    }
    Tree tree(pointForTree.begin(),pointForTree.end());
    PointForTree query(selected_x,selected_y,selected_z);
    K_neighbor_search search(tree,query);
    K_neighbor_search::iterator it = search.begin();
    selectedSeg=it->first.getSeg();
    selected_x = it->first.x();
    selected_y = it->first.y();
    selected_z = it->first.z();
    
    normal_x = it->first.normx();
    normal_y = it->first.normy();
    normal_z = it->first.normz();


 //   for(int i=0;i<meshPtr->numSeg();i++){
   //     for(int j=0;j<pointsSets[i].size();j++){
     //       if(pointsSets[i][j].x()==selected_x&&pointsSets[i][j].y()==selected_y&&pointsSets[i][j].z()==selected_z){
    //selectedSeg=i;
           // }
       // }
   // }
                std::cout<<"selected seg"<<selectedSeg<<std::endl;
    for(int i=0;i<pointsSets[selectedSeg].size();i++){
        if(deformationSwitch==true){
            distance=sqrt(std::pow((selected_x-pointsSets[selectedSeg][i].x()),2)+std::pow((selected_y-pointsSets[selectedSeg][i].y()),2)+std::pow((selected_z-pointsSets[selectedSeg][i].z()),2));


            if(distance>thr)disp=0;
            else if(changedisp==true)disp=-d*exp(-sigma*std::pow(distance,2));
            else if(changedisp==false)disp=d*exp(-sigma*std::pow(distance,2));
            
            Point p(pointsSets[selectedSeg][i].x()+disp*normal_x,
                    pointsSets[selectedSeg][i].y()+disp*normal_y,
                    pointsSets[selectedSeg][i].z()+disp*normal_z);
            vertices.push_back(p);
            std::cout<<p<<std::endl;
            Vector tmp(0, 0, 0);
            points.push_back(std::make_pair(p, tmp));
            pointsSets[selectedSeg][i].x()=pointsSets[selectedSeg][i].x()+disp*normal_x;
            pointsSets[selectedSeg][i].y()=pointsSets[selectedSeg][i].y()+disp*normal_y;
            pointsSets[selectedSeg][i].z()=pointsSets[selectedSeg][i].z()+disp*normal_z;
            PointForTree pointtree;
            pointtree.vec[0]=pointsSets[selectedSeg][i].x();
            pointtree.vec[1]=pointsSets[selectedSeg][i].y();
            pointtree.vec[2]=pointsSets[selectedSeg][i].z();
            pointtree.seg=i;
            pointtree.normalX=normalsSets[selectedSeg][i].x();
            pointtree.normalY=normalsSets[selectedSeg][i].y();
            pointtree.normalZ=normalsSets[selectedSeg][i].z();
            pointForTree.push_back(pointtree);
            
        }
        /*else{
            Point p(pointsSets[selectedSeg][i].x(),
                    pointsSets[selectedSeg][i].y(),
                    pointsSets[selectedSeg][i].z());
            vertices.push_back(p);
            Vector tmp(0, 0, 0);
            points.push_back(std::make_pair(p, tmp));
        }*/
    }
    if(deformationSwitch==true){
    CGAL::pca_estimate_normals<Concurrency_tag>(points.begin(), points.end(), CGAL::First_of_pair_property_map<PointVectorPair>(), CGAL::Second_of_pair_property_map<PointVectorPair>(), nb_neighbors);
    
    std::vector<PointVectorPair>::iterator unoriented_points_begin =
    CGAL::mst_orient_normals(points.begin(), points.end(),
                             CGAL::First_of_pair_property_map<PointVectorPair>(),
                             CGAL::Second_of_pair_property_map<PointVectorPair>(),
                             nb_neighbors);
    

    
    points.erase(unoriented_points_begin, points.end());
    for(unsigned i=0; i<points.size(); i++){
        Vector3 p_tmp(points[i].first.x(), points[i].first.y(),
                      points[i].first.z());
        Vector3 n_tmp(points[i].second.x(), points[i].second.y(),points[i].second.z());
        points2.push_back(p_tmp);
        normals2.push_back(n_tmp);
        //    set.insert(Point(points[i].first.x(), points[i].first.y(),points[i].first.z()));
    }
    }
    //read selected part and delete selected part
    if(replaceSwitch==1){
    pointsSets[8].erase(pointsSets[8].begin(),pointsSets[8].end());
    std::string line;
    std::ifstream in("rotate.txt");
    std::vector<Vector3> normalsForPart;
    std::vector<Vector3> points2Forpart;
    while(getline(in, line))
    {
        std::stringstream ss(line);
        double x, y, z;
        ss >> x >> y >> z;
        pointsSets[8].push_back(Vector3(x,y,z));
        pointsForpart.push_back(std::make_pair(Point(x,y,z),Vector(0,0,0)));
        continue;
    }
    CGAL::pca_estimate_normals<Concurrency_tag>(pointsForpart.begin(), pointsForpart.end(), CGAL::First_of_pair_property_map<PointVectorPair>(), CGAL::Second_of_pair_property_map<PointVectorPair>(), nb_neighbors);
    std::vector<PointVectorPair>::iterator unoriented_points_begin2 =
    CGAL::mst_orient_normals(pointsForpart.begin(), pointsForpart.end(),
                             CGAL::First_of_pair_property_map<PointVectorPair>(),
                             CGAL::Second_of_pair_property_map<PointVectorPair>(),
                             nb_neighbors);
    pointsForpart.erase(unoriented_points_begin2, pointsForpart.end());
    for(unsigned i=0; i<pointsForpart.size(); i++){
        Vector3 p_tmp(pointsForpart[i].first.x(), pointsForpart[i].first.y(),
                      pointsForpart[i].first.z());
        Vector3 n_tmp(pointsForpart[i].second.x(), pointsForpart[i].second.y(),pointsForpart[i].second.z());
        points2Forpart.push_back(p_tmp);
        normalsForPart.push_back(n_tmp);
        std::cout<<"points and normal="<<pointsForpart[i].first.x()<<" "<<pointsForpart[i].first.y()<<" "<<pointsForpart[i].first.z()<<" "<<pointsForpart[i].second.x()<<" "<<pointsForpart[i].second.y()<<" "<<pointsForpart[i].second.z()<<std::endl;
    }
    createOFFFile("selectPointcloud.off",points2Forpart,normalsForPart);
    
    Poisson_reconstruction_function functionForPart=Poisson_reconstruction(points2Forpart,normalsForPart,meshPtr);
    functionSets[8]=functionForPart;
    
    pointsSets[0].erase(pointsSets[0].begin(),pointsSets[0].end());
    std::string line0;
    std::ifstream in0("rotate0.txt");
    normalsForPart.erase(normalsForPart.begin(),normalsForPart.end());
    points2Forpart.erase(points2Forpart.begin(),points2Forpart.end());
    pointsForpart.erase(pointsForpart.begin(),pointsForpart.end());
    while(getline(in0, line0))
    {
        std::stringstream ss(line0);
        double x, y, z;
        ss >> x >> y >> z;
        pointsSets[0].push_back(Vector3(x,y,z));
        pointsForpart.push_back(std::make_pair(Point(x,y,z),Vector(0,0,0)));
        continue;
    }
    CGAL::pca_estimate_normals<Concurrency_tag>(pointsForpart.begin(), pointsForpart.end(), CGAL::First_of_pair_property_map<PointVectorPair>(), CGAL::Second_of_pair_property_map<PointVectorPair>(), nb_neighbors);
    std::vector<PointVectorPair>::iterator unoriented_points_begin3 =
    CGAL::mst_orient_normals(pointsForpart.begin(), pointsForpart.end(),
                             CGAL::First_of_pair_property_map<PointVectorPair>(),
                             CGAL::Second_of_pair_property_map<PointVectorPair>(),
                             nb_neighbors);
    pointsForpart.erase(unoriented_points_begin3, pointsForpart.end());
    for(unsigned i=0; i<pointsForpart.size(); i++){
        Vector3 p_tmp(pointsForpart[i].first.x(), pointsForpart[i].first.y(),
                      pointsForpart[i].first.z());
        Vector3 n_tmp(pointsForpart[i].second.x(), pointsForpart[i].second.y(),pointsForpart[i].second.z());
        points2Forpart.push_back(p_tmp);
        normalsForPart.push_back(n_tmp);
        std::cout<<"points and normal="<<pointsForpart[i].first.x()<<" "<<pointsForpart[i].first.y()<<" "<<pointsForpart[i].first.z()<<" "<<pointsForpart[i].second.x()<<" "<<pointsForpart[i].second.y()<<" "<<pointsForpart[i].second.z()<<std::endl;
    }
    createOFFFile("selectPointcloud0.off",points2Forpart,normalsForPart);
    
    Poisson_reconstruction_function functionForPart0=Poisson_reconstruction(points2Forpart,normalsForPart,meshPtr);
    functionSets[0]=functionForPart0;
    
    pointsSets[2].erase(pointsSets[2].begin(),pointsSets[2].end());
    std::string line1;
    std::ifstream in1("rotate1.txt");
    normalsForPart.erase(normalsForPart.begin(),normalsForPart.end());
    points2Forpart.erase(points2Forpart.begin(),points2Forpart.end());
    pointsForpart.erase(pointsForpart.begin(),pointsForpart.end());
    while(getline(in1, line1))
    {
        std::stringstream ss(line1);
        double x, y, z;
        ss >> x >> y >> z;
        pointsSets[2].push_back(Vector3(x,y,z));
        pointsForpart.push_back(std::make_pair(Point(x,y,z),Vector(0,0,0)));
        continue;
    }
    CGAL::pca_estimate_normals<Concurrency_tag>(pointsForpart.begin(), pointsForpart.end(), CGAL::First_of_pair_property_map<PointVectorPair>(), CGAL::Second_of_pair_property_map<PointVectorPair>(), nb_neighbors);
    std::vector<PointVectorPair>::iterator unoriented_points_begin4 =
    CGAL::mst_orient_normals(pointsForpart.begin(), pointsForpart.end(),
                             CGAL::First_of_pair_property_map<PointVectorPair>(),
                             CGAL::Second_of_pair_property_map<PointVectorPair>(),
                             nb_neighbors);
    pointsForpart.erase(unoriented_points_begin4, pointsForpart.end());
    for(unsigned i=0; i<pointsForpart.size(); i++){
        Vector3 p_tmp(pointsForpart[i].first.x(), pointsForpart[i].first.y(),
                      pointsForpart[i].first.z());
        Vector3 n_tmp(pointsForpart[i].second.x(), pointsForpart[i].second.y(),pointsForpart[i].second.z());
        points2Forpart.push_back(p_tmp);
        normalsForPart.push_back(n_tmp);
        std::cout<<"points and normal="<<pointsForpart[i].first.x()<<" "<<pointsForpart[i].first.y()<<" "<<pointsForpart[i].first.z()<<" "<<pointsForpart[i].second.x()<<" "<<pointsForpart[i].second.y()<<" "<<pointsForpart[i].second.z()<<std::endl;
    }
    createOFFFile("selectPointcloud1.off",points2Forpart,normalsForPart);
    
    Poisson_reconstruction_function functionForPart1=Poisson_reconstruction(points2Forpart,normalsForPart,meshPtr);
    functionSets[2]=functionForPart1;
}
    if(deformationSwitch==true){
    Poisson_reconstruction_function function1=Poisson_reconstruction(points2,normals2,meshPtr);
    functionSets[selectedSeg]=function1;
    }
    implicit_function_csg csgFunction(functionSets);
    implicit_function_min minFunction(functionSets);
    for(int i=0;i<points2.size();i++){
           // if(function1(Point(points2[i].x(),points2[i].y(),points2[i].z()))<=0){
                minPoints.push_back(points2[i]);
                minNormals.push_back(normals2[i]);
       // }
    }
    //Compute bounding box
    double max_x=minPoints[0].x(),min_x=minPoints[0].x(),max_y=minPoints[0].y(),min_y=minPoints[0].y(),max_z=minPoints[0].z(),min_z=minPoints[0].z();
    for(int i=0;i<minPoints.size();i++){
        if(max_x<minPoints[i].x())max_x=minPoints[i].x();
        if(min_x>minPoints[i].x())min_x=minPoints[i].x();
        if(max_y<minPoints[i].y())max_y=minPoints[i].y();
        if(min_y>minPoints[i].y())min_y=minPoints[i].y();
        if(max_z<minPoints[i].z())max_z=minPoints[i].z();
        if(min_z>minPoints[i].z())min_z=minPoints[i].z();
    }
    double e=epsilonForMarchingBounding;
    max_x=max_x+max_x*e;
    min_x=min_x+min_x*e;
    max_y=max_y+max_y*e;
    min_y=min_y+min_y*e;
    max_z=max_z+max_z*e;
    min_z=min_z+min_z*e;
    std::cout<<"max_x="<<max_x<<", min_x="<<min_x<<"max_y="<<max_y<<", min_y="<<min_y<<"max_z="<<max_z<<", min_z="<<min_z<<std::endl;
    initMCGrid(max_x,min_x,max_y,min_y,max_z,min_z);
    
    //MarchingCubesForEachFunction();

    Poisson_reconstruction_function function2=Poisson_reconstruction(minPoints,minNormals,meshPtr);

    double averagespacing = CGAL::compute_average_spacing<Concurrency_tag>(points3.begin(), points3.end(),CGAL::Identity_property_map<Point>() ,nb_neighbors2);
    SurfaceMesh output_mesh;
    

    if(mesher==0){
        normalize();
        if(replaceSwitch==1){
        OutputSelectedPart("selectPointcloud.txt",3);
        OutputSelectedPart("selectPointcloud_0.txt",0);
        OutputSelectedPart("selectPointcloud_1.txt",1);
        }
            const auto startTime = std::chrono::system_clock::now();
            std::cout<<"Marching start "<<unionValue<<std::endl;
        
            for (size_t i = 0; i < mcgrid.structuredGrid.size(); ++i) {
                if(unionValue==0){
                mcgrid.results[i]=csgFunction(Point(mcgrid.structuredGrid[i](0),mcgrid.structuredGrid[i](1),mcgrid.structuredGrid[i](2)));
                //    std::cout<<"Point="<<mcgrid.structuredGrid[i](0)<<" "<<mcgrid.structuredGrid[i](1)<<" "<<mcgrid.structuredGrid[i](2)<<" "<<"unionValue="<<mcgrid.results[i]<<std::endl;
                }
                else if(unionValue==1){
                    mcgrid.results[i]=minFunction(Point(mcgrid.structuredGrid[i](0),mcgrid.structuredGrid[i](1),mcgrid.structuredGrid[i](2)));
                }
                else if(unionValue==2){
                    mcgrid.results[i]=function2(Point(mcgrid.structuredGrid[i](0),mcgrid.structuredGrid[i](1),mcgrid.structuredGrid[i](2)));
                }
               //std::cout<<"marching "<<mcgrid.results[i]<<std::endl;
            }
            double *resultarray= new double[mcgrid.results.size()];
            for(unsigned int i=0;i<mcgrid.results.size();i++){
                resultarray[i] = mcgrid.results[i];
            }
            ciso->GenerateSurface(resultarray, 0,
                                  mcgrid.subx-1, mcgrid.suby-1, mcgrid.subz-1,
                                  mcgrid.dx, mcgrid.dy, mcgrid.dz);
            const auto endTime = std::chrono::system_clock::now();
             const auto timeSpan = endTime - startTime;
             std::cout << "Marching cube's time:" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
            delete[] resultarray;
            
            meshPtr->setData(ciso->m_ppt3dVertices, ciso->m_nVertices,
                             ciso->m_piTriangleIndices, ciso->m_nTriangles);
            //meshPtr->normalize();
        createVTKFile("MC_poisson_CSG.vtk",mcgrid.subx,mcgrid.suby,mcgrid.subz,mcgrid.structuredGrid,mcgrid.results);
            createOFFFile2("out.off");
        std::cout<<"drawed"<<std::endl;
            //createOBJFile_marching("out.obj");
        segmentedColor=false;

        
    }
    else if(mesher==1){
        float eps=0;
        Poisson_reconstruction_function fun = Poisson_reconstruction(meshPtr);
        std::cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;

        Point ip = fun.get_inner_point();
        Sphere bs = fun.bounding_sphere();
        FT r = std::sqrt(bs.squared_radius());
        r = 2.0*r;
        const auto startTime = std::chrono::system_clock::now();
        
        std::cout<<"Our delaunay start"<<std::endl;
        
        Delaunay dl;
        
        for (std::size_t i = 0; i < minPoints.size(); ++i) {
            dl.insert(Delaunay::Point(minPoints[i].x(), minPoints[i].y(),minPoints[i].z()));
        }
        dl.insert(Delaunay::Point(ip.x()+r, ip.y(), ip.z()));
        dl.insert(Delaunay::Point(ip.x()-r, ip.y(), ip.z()));
        dl.insert(Delaunay::Point(ip.x(), ip.y()+r, ip.z()));
        dl.insert(Delaunay::Point(ip.x(), ip.y()-r, ip.z()));
        dl.insert(Delaunay::Point(ip.x(), ip.y(), ip.z()+r));
        dl.insert(Delaunay::Point(ip.x(), ip.y(), ip.z()-r));
        
        Cell_inside<Delaunay, implicit_function_min> cellin(dl, minFunction,cellInsideValue);
       // Surface_builder<Delaunay, Cell_inside<Delaunay, implicit_function_csg>, SurfaceMesh> b(dl, cellin);
         Surface_builder_dual<Delaunay, implicit_function_min, Cell_inside<Delaunay, implicit_function_min>, SurfaceMesh> b(dl, minFunction, cellin);
        //output_mesh.delegate(b);
        output_mesh = b.get_polyhedron();

        if(resamplingSwitch==1){
            /*Delaunay dl2=resampling(output_mesh,meshPtr,averagespacing,dl);
            Cell_inside<Delaunay, implicit_function_csg> cellin2(dl2, csgFunction,cellInsideValue);
            Surface_builder<Delaunay, Cell_inside<Delaunay, implicit_function_csg>, SurfaceMesh> b2(dl2, cellin2);
            output_mesh.delegate(b2);*/
            std::vector<SurfaceMesh::Facet_handle>  new_facets;
            std::vector<polyVertex_handle> new_vertices;
            CGAL::Polygon_mesh_processing::refine(output_mesh,
                                                  faces(output_mesh),
                                                  std::back_inserter(new_facets),
                                                  std::back_inserter(new_vertices),
                                                  CGAL::Polygon_mesh_processing::parameters::density_control_factor(2.));

            typedef typename SurfaceMesh::Vertex_const_iterator VCI;
            std::vector<Vec3> vertices;
            Delaunay dl2;
            for (VCI vi = output_mesh.vertices_begin();vi != output_mesh.vertices_end();++vi){
                dl2.insert(Delaunay::Point(CGAL::to_double(vi->point().x()),
                       CGAL::to_double(vi->point().y()),
                       CGAL::to_double(vi->point().z())));
            }
            Cell_inside<Delaunay, implicit_function_min> cellin2(dl2, minFunction,cellInsideValue);
            Surface_builder<Delaunay, Cell_inside<Delaunay, implicit_function_min>, SurfaceMesh> b2(dl2, cellin2);
            output_mesh.delegate(b2);
            BOOST_FOREACH(SurfaceMesh::Halfedge_handle h, halfedges(output_mesh)){
                if(h->is_border())
                {
                    std::vector<SurfaceMesh::Facet_handle>  patch_facets;
                    std::vector<polyVertex_handle> patch_vertices;
                    bool success = CGAL::cpp11::get<0>(CGAL::Polygon_mesh_processing::triangulate_refine_and_fair_hole(output_mesh,h,std::back_inserter(patch_facets),std::back_inserter(patch_vertices),CGAL::Polygon_mesh_processing::parameters::vertex_point_map(get(CGAL::vertex_point, output_mesh)).geom_traits(Kernel())) );
        }
            }
        }
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "Our Delaunay's time:" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
        setMeshFromPolyhedron(output_mesh, meshPtr);
        segmentedColor=false;
        createOFFFileForPolyhedron("OurDelaunay.off",output_mesh);
    }
    else if(mesher==2){
        /*
    const auto startTime = std::chrono::system_clock::now();
    std::cout<<"CGAL's delaunay start"<<std::endl;
    Point inner_point = function2.get_inner_point();
    Sphere bsphere = function2.bounding_sphere();
    FT radius = std::sqrt(bsphere.squared_radius());
    FT sm_sphere_radius = 5.0 * radius;
    FT sm_dichotomy_error = sm_distance*averagespacing/1000.0; // Dichotomy error must be << sm_distance
    Surface_3 surface(function2,
                      Sphere(inner_point,sm_sphere_radius*sm_sphere_radius),
                      sm_dichotomy_error/sm_sphere_radius);
    
    CGAL::Surface_mesh_default_criteria_3<STr>
    criteria(sm_angle, sm_radius*averagespacing, sm_distance*averagespacing);
    
    STr tr;
    tr.insert(vertices.begin(), vertices.end());
    C2t3 c2t3(tr);
    CGAL::make_surface_mesh(c2t3,     // reconstructed mesh
                            surface,  // implicit surface
                            criteria, // meshing criteria
                            CGAL::Manifold_with_boundary_tag());  // require manifold mesh
    CGAL::output_surface_facets_to_polyhedron(c2t3, output_mesh);
    const auto endTime = std::chrono::system_clock::now();
    const auto timeSpan = endTime - startTime;
    std::cout << "Delaunay(CGAL)'s time:" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
        meshPtr->colors.erase(meshPtr->colors.begin(),meshPtr->colors.end());
    setMeshFromPolyhedron(output_mesh, meshPtr);

    std::cout<<"meshPtr->colors size="<<meshPtr->colors.size()<<std::endl;
     segmentedColor=false;
         */
        const auto startTime = std::chrono::system_clock::now();
        std::cout<<"CGAL's delaunay start"<<std::endl;
        PointList pt;
        for(std::size_t i=0;i<points2.size();i++){
            pt.push_back(Point(minPoints[i].x(), minPoints[i].y(),minPoints[i].z()));
        }
        Min_sphere  ms (pt.begin(), pt.end());
        FT sm_sphere_radius = 5.0 * 5.0;
        FT sm_dichotomy_error = sm_distance*averagespacing/1000.0; // Dichotomy error must be << sm_distance
        Surface_3_min surface(minFunction,Sphere(ms.center(),ms.squared_radius()*1.5));
        CGAL::Surface_mesh_default_criteria_3<STr>
        criteria(sm_angle, sm_radius*averagespacing, sm_distance*averagespacing);
        STr tr;
        tr.insert(vertices.begin(), vertices.end());
        C2t3 c2t3(tr);
        CGAL::make_surface_mesh(c2t3,surface,criteria,CGAL::Manifold_with_boundary_tag());
        CGAL::output_surface_facets_to_polyhedron(c2t3, output_mesh);
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "Delaunay(CGAL)'s time:" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
         setMeshFromPolyhedron(output_mesh, meshPtr);
        segmentedColor=false;
    }
}
/*
 //HRBF reconstruction
 if(reconstructionValue==0){
 Hrbf_function function=HRBF_reconstruction(points2,normals2,meshPtr);
 if(mesher==1){
 const auto startTime = std::chrono::system_clock::now();
 std::cout<<"Our delaunay start"<<std::endl;
 Delaunay dl;
 for (std::size_t i = 0; i < points2.size(); ++i) {
 dl.insert(Delaunay::Point(points2[i][0], points2[i][1],points2[i][2]));
 }
 Cell_inside<Delaunay, Hrbf_function> cellin(dl, function,cellInsideValue);
 Surface_builder<Delaunay, Cell_inside<Delaunay, Hrbf_function>, SurfaceMesh> b(dl, cellin);
 output_mesh.delegate(b);
 const auto endTime = std::chrono::system_clock::now();
 const auto timeSpan = endTime - startTime;
 std::cout << "Delaunay's time:" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
 if(resamplingSwitch==1){
 Delaunay dl2=resampling(output_mesh,meshPtr,averagespacing,dl);
 Cell_inside<Delaunay, Hrbf_function> cellin2(dl2, function,cellInsideValue);
 Surface_builder<Delaunay, Cell_inside<Delaunay, Hrbf_function>, SurfaceMesh> b2(dl2, cellin2);
 output_mesh.delegate(b2);
 }
 
 }
 if(mesher==2){
 const auto startTime = std::chrono::system_clock::now();
 std::cout<<"CGAL's delaunay start"<<std::endl;
 PointList pt;
 for(std::size_t i=0;i<points2.size();i++){
 pt.push_back(Point(points2[i][0], points2[i][1], points2[i][2]));
 }
 Min_sphere  ms (pt.begin(), pt.end());
 FT sm_sphere_radius = 5.0 * 5;
 FT sm_dichotomy_error = sm_distance*averagespacing/1000.0; // Dichotomy error must be << sm_distance
 Surface_3_hrbf surface(function,Sphere(ms.center(),ms.squared_radius()*1.5));
 CGAL::Surface_mesh_default_criteria_3<STr>
 criteria(sm_angle, sm_radius*averagespacing, sm_distance*averagespacing);
 STr tr;
 tr.insert(vertices.begin(), vertices.end());
 C2t3 c2t3(tr);
 CGAL::make_surface_mesh(c2t3,surface,criteria,CGAL::Manifold_with_boundary_tag());
 CGAL::output_surface_facets_to_polyhedron(c2t3, output_mesh);
 const auto endTime = std::chrono::system_clock::now();
 const auto timeSpan = endTime - startTime;
 std::cout << "Delaunay(CGAL)'s time:" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
 }
 }
 //HRBF_closed
 else if(reconstructionValue==1){
 Crbf_function function=HRBF_closed_reconstruction(points,points2,normals2,meshPtr);
 if(mesher==1){
 const auto startTime = std::chrono::system_clock::now();
 std::cout<<"Our delaunay start"<<std::endl;
 Delaunay dl;
 for (std::size_t i = 0; i < points2.size(); ++i) {
 dl.insert(Delaunay::Point(points2[i][0], points2[i][1],points2[i][2]));
 }
 Cell_inside<Delaunay, Crbf_function> cellin(dl, function,cellInsideValue);
 Surface_builder<Delaunay, Cell_inside<Delaunay, Crbf_function>, SurfaceMesh> b(dl, cellin);
 output_mesh.delegate(b);
 const auto endTime = std::chrono::system_clock::now();
 const auto timeSpan = endTime - startTime;
 std::cout << "Delaunay's time:" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
 if(resamplingSwitch==1){
 Delaunay dl2=resampling(output_mesh,meshPtr,averagespacing,dl);
 Cell_inside<Delaunay, Crbf_function> cellin2(dl2, function,cellInsideValue);
 Surface_builder<Delaunay, Cell_inside<Delaunay, Crbf_function>, SurfaceMesh> b2(dl2, cellin2);
 output_mesh.delegate(b2);
 }
 
 }
 if(mesher==2){
 const auto startTime = std::chrono::system_clock::now();
 std::cout<<"CGAL's delaunay start"<<std::endl;
 PointList pt;
 for(std::size_t i=0;i<points2.size();i++){
 pt.push_back(Point(points2[i][0], points2[i][1], points2[i][2]));
 }
 Min_sphere  ms (pt.begin(), pt.end());
 FT sm_sphere_radius = 5.0 * 5.0;
 FT sm_dichotomy_error = sm_distance*averagespacing/1000.0; // Dichotomy error must be << sm_distance
 Surface_3_crbf surface(function,Sphere(ms.center(),ms.squared_radius()*1.5));
 CGAL::Surface_mesh_default_criteria_3<STr>
 criteria(sm_angle, sm_radius*averagespacing, sm_distance*averagespacing);
 STr tr;
 tr.insert(vertices.begin(), vertices.end());
 C2t3 c2t3(tr);
 CGAL::make_surface_mesh(c2t3,surface,criteria,CGAL::Manifold_with_boundary_tag());
 CGAL::output_surface_facets_to_polyhedron(c2t3, output_mesh);
 const auto endTime = std::chrono::system_clock::now();
 const auto timeSpan = endTime - startTime;
 std::cout << "Delaunay(CGAL)'s time:" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
 }
 }
 
 // Poisson reconstruction
 else if(reconstructionValue==2){
 
 Poisson_reconstruction_function function=Poisson_reconstruction(points2,normals2,meshPtr);
 
 if(mesher==1){
 const auto startTime = std::chrono::system_clock::now();
 
 std::cout<<"Our delaunay start"<<std::endl;
 
 Delaunay dl;
 
 for (std::size_t i = 0; i < points2.size(); ++i) {
 dl.insert(Delaunay::Point(points2[i][0], points2[i][1],points2[i][2]));
 }
 
 
 
 Cell_inside<Delaunay, Poisson_reconstruction_function> cellin(dl, function,cellInsideValue);
 Surface_builder<Delaunay, Cell_inside<Delaunay, Poisson_reconstruction_function>, SurfaceMesh> b(dl, cellin);
 output_mesh.delegate(b);
 const auto endTime = std::chrono::system_clock::now();
 const auto timeSpan = endTime - startTime;
 std::cout << "Delaunay's time:" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
 if(resamplingSwitch==1){
 Delaunay dl2=resampling(output_mesh,meshPtr,averagespacing,dl);
 Cell_inside<Delaunay, Poisson_reconstruction_function> cellin2(dl2, function,cellInsideValue);
 Surface_builder<Delaunay, Cell_inside<Delaunay, Poisson_reconstruction_function>, SurfaceMesh> b2(dl2, cellin2);
 output_mesh.delegate(b2);
 }
 
 }
 if(mesher==2){
 const auto startTime = std::chrono::system_clock::now();
 std::cout<<"CGAL's delaunay start"<<std::endl;
 Point inner_point = function.get_inner_point();
 Sphere bsphere = function.bounding_sphere();
 FT radius = std::sqrt(bsphere.squared_radius());
 
 FT sm_sphere_radius = 5.0 * radius;
 FT sm_dichotomy_error = sm_distance*averagespacing/1000.0; // Dichotomy error must be << sm_distance
 Surface_3 surface(function,
 Sphere(inner_point,sm_sphere_radius*sm_sphere_radius),
 sm_dichotomy_error/sm_sphere_radius);
 
 CGAL::Surface_mesh_default_criteria_3<STr>
 criteria(sm_angle, sm_radius*averagespacing, sm_distance*averagespacing);
 
 STr tr;
 tr.insert(vertices.begin(), vertices.end());
 C2t3 c2t3(tr);
 CGAL::make_surface_mesh(c2t3,     // reconstructed mesh
 surface,  // implicit surface
 criteria, // meshing criteria
 CGAL::Manifold_with_boundary_tag());  // require manifold mesh
 CGAL::output_surface_facets_to_polyhedron(c2t3, output_mesh);
 const auto endTime = std::chrono::system_clock::now();
 const auto timeSpan = endTime - startTime;
 std::cout << "Delaunay(CGAL)'s time:" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
 }
 }
 if(mesher==1||mesher==2){
 createOBJFile("out.obj",output_mesh);
 createOFFFileForPolyhedron("out.off",output_mesh);
 Polyhedron2 outpoly;
 outpoly=takeUnitPolyhedron();
 setMeshFromPolyhedron(outpoly, meshPtr);
 }
 
 
 std::cout<<"drawed"<<std::endl;
 segmentedColor=false;
 
 }
 */

