/**********************************************************************
  MeshGenerator - Class to generate meshes from volumetric data

  Copyright (C) 2008 by Marcus D. Hanwell

  This file is part of the Avogadro molecular editor project.
  For more information, see <http://avogadro.sourceforge.net/>

  Avogadro is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  Avogadro is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
 **********************************************************************/

 #ifndef MESHGENERATOR_H
 #define MESHGENERATOR_H

#include <avogadro/global.h>

#include <Eigen/Core>

#include <QThread>

#include <vector>

 namespace Avogadro {

  class Cube;
  class Mesh;

  /**
   * @class MeshGenerator meshgenerator.h <avogadro/meshgenerator.h>
   * @brief Class that can generate Mesh objects from Cube objects.
   * @author Marcus D. Hanwell
   *
   * This class implements a method of generating an isosurface Mesh from
   * volumetric data. In the case of the MeshGenerator class it expects a
   * Cube as an input, an isosurface value and optionally a bounding box.
   * If no bounding box is supplied the mesh will be generated over the entire
   * Cube. The tables and the basic code is taken from the public domain code
   * written by Cory Bloyd (marchingsource.cpp) and available at,
   * http://local.wasp.uwa.edu.au/~pbourke/geometry/polygonise/
   *
   * You must first initialize the class and then call run() to actually
   * polygonize the isosurface. Connect to the classes finished() signal to
   * do something once the polygonization is complete.
   */

  class A_EXPORT MeshGenerator : public QThread
  {
  public:
    /**
     * Constructor.
     */
    MeshGenerator(QObject *parent = 0);

    /**
     * Constructor. Can be used to initialize the MeshGenerator.
     * @param cube The source Cube with the volumetric data.
     * @param mesh The Mesh that will hold the isosurface.
     * @param iso The iso value of the surface.
     * @return True if the MeshGenerator was successfully initialized.
     */
    explicit MeshGenerator(const Cube *cube, Mesh *mesh, float iso,
                           QObject *parent = 0);

    /**
     * Destructor.
     */
    virtual ~MeshGenerator();

    /**
     * Initialization function, set up the MeshGenerator ready to find an
     * isosurface of the supplied Cube.
     * @param cube The source Cube with the volumetric data.
     * @param mesh The Mesh that will hold the isosurface.
     * @param iso The iso value of the surface.
     * @return True if the MeshGenerator was successfully initialized.
     */
    bool initialize(const Cube *cube, Mesh *mesh, float iso);

    /**
     * Use this function to begin Mesh generation. Uses an asynchronous thread,
     * and so avoids locking the user interface while the isosurface is found.
     */
    void run();

    /**
     * @return The Cube being used by the class.
     */
    const Cube * cube() const { return m_cube; }

    /**
     * @return The Mesh being generated by the class.
     */
    Mesh * mesh() const { return m_mesh; }

  protected:
    /**
     * Get the normal to the supplied point. This operation is quite expensive
     * and so should be avoided wherever possible.
     * @param pos The position of the vertex whose normal is needed.
     * @return The normal vector for the supplied point.
     */
    Eigen::Vector3f normal(const Eigen::Vector3f &pos);

    /**
     * Get the offset, i.e. the approximate point of intersection of the surface
     * between two points.
     * @param val1 The position of the vertex whose normal is needed.
     * @return The normal vector for the supplied point.
     */
    float offset(float val1, float val2);

    /**
     * Perform a marching cubes step on a single cube.
     */
    bool marchingCube(const Eigen::Vector3i &pos);

    float m_iso;           /** The value of the isosurface. */
    const Cube *m_cube;    /** The cube that we are generating a Mesh from. */
    Mesh *m_mesh;          /** The mesh that is being generated. */
    float m_stepSize;      /** The step size of the cube. */
    Eigen::Vector3f m_min; /** The minimum point in the cube. */
    Eigen::Vector3i m_dim; /** The dimensions of the cube. */
    std::vector<Eigen::Vector3f> m_vertices, m_normals;

    /**
     * These are the tables of constants for the marching cubes and tetrahedra
     * algorithms. They are taken from the public domain source at
     * http://local.wasp.uwa.edu.au/~pbourke/geometry/polygonise/
     */
    static const float a2fVertexOffset[8][3];
    static const int   a2iVertexOffset[8][3];
    static const int   a2iEdgeConnection[12][2];
    static const float a2fEdgeDirection[12][3];
    static const int   a2iTetrahedronEdgeConnection[6][2];
    static const int   a2iTetrahedronsInACube[6][4];
    static const long  aiTetrahedronEdgeFlags[16];
    static const int   a2iTetrahedronTriangles[16][7];
    static const long  aiCubeEdgeFlags[256];
    static const int   a2iTriangleConnectionTable[256][16];
  };

  inline float MeshGenerator::offset(float val1, float val2)
  {
    if (val2 - val1 == 0.0f)
      return 0.5;
    return (m_iso - val1) / (val2 - val1);
  }

 } // End namespace Avogadro

 #endif
 
