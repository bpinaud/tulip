/**
 *
 * This file is part of Tulip (www.tulip-software.org)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux 1 and Inria Bordeaux - Sud Ouest
 *
 * Tulip is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Tulip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 */
#ifndef PROPERTY_INTERFACE_H
#define PROPERTY_INTERFACE_H

#include <string>
#include "tulip/tulipconf.h"
#include "tulip/Observable.h"
#include "tulip/ObservableProperty.h"
#include "tulip/Iterator.h"
#include "tulip/Node.h"
#include "tulip/Edge.h"
#include "tulip/Reflect.h"

namespace tlp {

/**
 * \defgroup properties
 */ 
/*@{*/
//=============================================================
class TLP_SCOPE PropertyInterface: public Observable, public ObservableProperty {
  friend class PropertyManager;
protected:
  // name of the property when registered as a property of a graph
  std::string name;
  // the graph for whom the property is registered
  Graph *graph;

public:
  virtual ~PropertyInterface();
  /**
   * Remove the value stored for the node given in parameter.
   */
  virtual void erase(const node) = 0;
  /**
   * Remove the value stored for the edge given in parameter.
   */
  virtual void erase(const edge) = 0;
  /**
   * Set the value of a node (first argument) in the current property (this)
   * with the value of the node (second argument) defined in prop (third argument)
   * If the fourth argument is set to true, the value will be copied only if
   * it is not the default value.
   */
  virtual void copy(const node src, const node dst, PropertyInterface *prop,
		    bool ifNotDefault = false) =0;
  /**
   * Set the value of an edge (first argument) in the current property (this)
   * with the value of the edge (second argument) defined in prop (third argument)
   * defined in this property (this).
   * If the fourth argument is set to true, the value will be copied only if
   * it is not the default value.
   */
  virtual void copy(const edge src, const edge dst , PropertyInterface *prop,
		    bool ifNotDefault = false) =0;
  /**
    * Create an object of the same real type of the current property, in the
    * the graph (first parameter) with the name (second parameter).
    * Values are not copied.
    */
  virtual PropertyInterface* clonePrototype(Graph *, const std::string&) =0;
  /**
    * Returns a string describing the type of the property.
    * i.e. "graph", "double", "layout", "string", "integer", "color", "size", ...
    */
  virtual std::string getTypename() const = 0;
  /**
    * Returns a string describing the type a property (first parameter).
    * i.e. "graph", "double", "layout", "string", "integer", "color", "size", ...
    */
  static  std::string getTypename(const PropertyInterface *);

  /**
    * Return the name of the property
    */
  const std::string& getName() const {
    return name;
  }

  /**
  * Returns the graph on which the property has been defined.
  * WARNING : If the property is inherited the graph could be different
  * that the one used to get that property.
  */
  tlp::Graph * getGraph() {
      return graph;
  }

  /**
  * Returns a string representation of the node default value
  */
  virtual std::string getNodeDefaultStringValue() const = 0;
  /**
   * Clear all nodes registered values, and set a new node default value
   * in converting the given string representation of a value
   */
  virtual bool setAllNodeStringValue( const std::string & v ) = 0;
  /**
  * Returns a string representation of the edge default value
  */
  virtual std::string getEdgeDefaultStringValue() const = 0;
  /**
   * Clear all edges registered values, and set a new edge default value
   * in converting the given string representation of a value
   */
  virtual bool setAllEdgeStringValue( const std::string & v ) = 0;
  /**
   * Returns a string conversion of the value registered for the given node
   */
  virtual std::string getNodeStringValue( const node n ) const = 0;
  /**
   * Register a new value for the given node in converting
   * the given string representation
   */
  virtual bool setNodeStringValue( const node n, const std::string & v ) = 0;
  /**
   * Returns a string conversion of the value registered for the given edge
   */
  virtual std::string getEdgeStringValue( const edge e ) const = 0;
  /**
   * Register a new value for the given edge in converting
   * the given string representation
   */
  virtual bool setEdgeStringValue( const edge e, const std::string & v ) = 0;
  /**
   * Returns a pointer to a DataMem structure embedding the node default value
   * WARNING: it is of the caller responsability to delete the returned structure
  */
  virtual DataMem* getNodeDefaultDataMemValue() const = 0;
  /**
   * Clear all nodes registered values, and set a new node default value
   * using the value embedded in the pointed DataMem structure
   */
  virtual void setAllNodeDataMemValue(const DataMem* v ) = 0;
  /**
   * Returns a pointer to a DataMem structure embedding the edge default value
   * WARNING: the caller have the responsability to delete the returned structure
  */
  virtual DataMem* getEdgeDefaultDataMemValue() const = 0;
  /**
   * Clear all edges registered values, and set a new edge default value
   * using the value embedded in the pointed DataMem structure
   */
  virtual void setAllEdgeDataMemValue(const DataMem* v ) = 0;
  /**
  * Returns a pointer to a DataMem structure embedding the node registered value
  * WARNING: it is of the caller responsability to delete the returned structure
  */
  virtual DataMem* getNodeDataMemValue( const node n ) const = 0;
  /**
   * Returns a NULL pointer if the given node registered value is the default
   * else returns a pointer to a DataMem structure embedding the value
   * WARNING: it is of the caller responsability to delete the returned structure
  */
  virtual DataMem* getNonDefaultDataMemValue( const node n ) const = 0;
  /**
   * Register a new value for the given node
   * using the value embedded in the pointed DataMem structure
   */
  virtual void setNodeDataMemValue( const node n, const DataMem* v) = 0;
  /**
  * Returns a pointer to a DataMem structure embedding the edge registered value
  * WARNING: it is of the caller responsability to delete the returned structure
  */
  virtual DataMem* getEdgeDataMemValue( const edge e ) const = 0;
  /**
   * Returns a NULL pointer if the given edge registered value is the default
   * else returns a pointer to a DataMem structure embedding the value.
   * WARNING: it is of the caller responsability to delete the returned structure
  */
  virtual DataMem* getNonDefaultDataMemValue( const edge e ) const = 0;
  /**
   * Register a new value for the given node
   * using the value embedded in the pointed DataMem structure
   */
  virtual void setEdgeDataMemValue( const edge e, const DataMem* v) = 0;
  /**
   * Returns an iterator on all nodes whose value is different
   * from the default value. When the pointer to the graph is not NULL
   * only the edges owned by this graph are returned by the iterator.
   * WARNING: it is of the caller responsability to delete the returned iterator
   */
  virtual tlp::Iterator<node>* getNonDefaultValuatedNodes(const Graph* = NULL) const = 0;
  /**
   * Returns an iterator on all edges whose value is different
   * from the default value. When the pointer to the graph is not NULL
   * only the edges owned by this graph are returned by the iterator.
   * WARNING: it is of the caller responsability to delete the returned iterator
   */
  virtual tlp::Iterator<edge>* getNonDefaultValuatedEdges(const Graph* = NULL) const = 0;
  /**
   * Set a computed value for the meta node mN pointing to the sub-graph sg
   * mg is the graph owning the meta node
   */
  virtual void computeMetaValue(node mN, Graph* sg, Graph* mg)=0;
  /**
   * Set a computed value for the meta edge mE representing the edges
   * from the iterator itE.
   * mg is the graph owning the meta edge
   */
  virtual void computeMetaValue(edge mE, tlp::Iterator<edge>* itE, Graph* mg)=0;

  // superclass of further MetaValueCalculator
  class MetaValueCalculator {
  public:
    virtual ~MetaValueCalculator() {}
  };


  /**
   * Returns the meta value calculator associated to this property
   */
  MetaValueCalculator* getMetaValueCalculator() {
    return metaValueCalculator;
  }

  /**
   * Sets the meta value calculator.
   * Be careful that its destruction is not managed by the property
   */
  virtual void setMetaValueCalculator(MetaValueCalculator* mvCalc) {
    metaValueCalculator = mvCalc;
  }


 protected:
  MetaValueCalculator* metaValueCalculator;
  // redefinitions of ObservableProperty methods
  void notifyAfterSetNodeValue(PropertyInterface*,const node n);
  void notifyAfterSetEdgeValue(PropertyInterface*,const edge e);
  void notifyAfterSetAllNodeValue(PropertyInterface*);
  void notifyAfterSetAllEdgeValue(PropertyInterface*);
  void notifyDestroy(PropertyInterface*);
};

}

#endif // PROPERTY_INTERFACE_H
