#ifndef GL_CONVEX_HULL_H
#define GL_CONVEX_HULL_H

#include <vector>
#include <tulip/BoundingBox.h>
#include <tulip/Graph.h>
#include <tulip/Color.h>
#include <tulip/Coord.h>

#include "tulip/GlSimpleEntity.h"

namespace tlp {

  /** \brief Class used to represent ConvexHull
   *
   * Class used to represent ConvexHull. GlHierarchyConvexHull class use this to create all convexHull of the graph
   */
  class TLP_GL_SCOPE GlConvexHull: public GlSimpleEntity {
  public:
    /**
     * Build a Polygon with the convex hull of points
     */
    GlConvexHull(const std::vector<Coord> &points, 
		 const std::vector<Color> &fillColors,
		 const std::vector<Color> &outlineColors,
		 const bool filled,
		 const bool outlined,
		 const std::string &name,
		 bool computeHull=true);
    
    virtual ~GlConvexHull() {}

    /**
     * Retrun the name of this convex hull
     */
    std::string getName() {return _name;}
    
    /**
     * Draw the convexHull
     */
    virtual void draw(float lod,Camera *camera);
    
    /**
     * Static function who build a hierarchy of convexHull with the given graph
     */
    static std::vector<GlConvexHull *> buildConvexHullsFromHierarchy(Graph *graph,
								     std::vector<Color> fColors,
								     std::vector<Color> oColors,
								     bool deduceFromChilds = true,
								     Graph *root = 0,
								     unsigned int depth = 0);

    /**
     * Translate entity
     */
    virtual void translate(const Coord& mouvement);
    
    /**
     * Get the data in XML form
     */
    void getXML(xmlNodePtr rootNode);

    /**
     * Set the data with XML
     */
    void setWithXML(xmlNodePtr rootNode);
    
  protected:
    std::vector<Coord> _points;
    std::vector<Color> _fillColors;
    std::vector<Color> _outlineColors;
    bool _filled;
    bool _outlined;
    std::string _name;
  };
}
#endif
