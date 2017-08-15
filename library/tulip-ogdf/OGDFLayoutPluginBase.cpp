/**
 *
 * This file is part of Tulip (http://tulip.labri.fr)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux
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
#include "tulip2ogdf/OGDFLayoutPluginBase.h"

#include <vector>
#include <tulip/DrawingTools.h>
#include <tulip/ForEach.h>

using namespace std;

OGDFLayoutPluginBase::OGDFLayoutPluginBase(const tlp::PluginContext* context, ogdf::LayoutModule *ogdfLayoutAlgo) :
  tlp::LayoutAlgorithm(context), tlpToOGDF(NULL), ogdfLayoutAlgo(ogdfLayoutAlgo) {
  // convert Tulip Graph to OGDF Graph including attributes
  if (graph)
    tlpToOGDF = new TulipToOGDF(graph, false);
}

OGDFLayoutPluginBase::~OGDFLayoutPluginBase() {
  delete tlpToOGDF;
  delete ogdfLayoutAlgo;
}

bool OGDFLayoutPluginBase::run() {

  ogdf::GraphAttributes &gAttributes = tlpToOGDF->getOGDFGraphAttr();

  beforeCall();

  try {
    // run the algorithm on the OGDF Graph with attributes
    callOGDFLayoutAlgorithm(gAttributes);
  }
  catch (PreconditionViolatedException& pvce) {
    std::string msg;

    switch(pvce.exceptionCode()) {
    case pvcSelfLoop:
      msg = "graph contains a self-loop";
      break;

    case pvcTreeHierarchies:
      msg = "hierarchies are not only trees";
      break;

    case pvcAcyclicHierarchies:
      msg = "hierarchies are not acyclic";
      break;

    case pvcSingleSource:
      msg = "graph has not a single source";
      break;

    case pvcUpwardPlanar:
      msg = "graph is not upward planar";
      break;

    case pvcTree:
      msg = "graph is not a rooted tree";
      break;

    case pvcForest:
      msg = "graph is not a rooted forest";
      break;

    case pvcOrthogonal:
      msg = "layout is not orthogonal";
      break;

    case pvcPlanar:
      msg = "graph is not planar";
      break;

    case pvcClusterPlanar:
      msg = "graph is not c-planar";
      break;

    case pvcNoCopy:
      msg = "graph is not a copy of the corresponding graph";
      break;

    case pvcConnected:
      msg = "graph is not connected";
      break;

    case pvcBiconnected:
      msg = "graph is not twoconnected";
      break;

    default:
      msg = "unknown error";
    }

    pluginProgress->setError(std::string("Error\n") + msg);
    return false;
  }
  catch (AlgorithmFailureException& afce) {
    std::string msg;

    switch(afce.exceptionCode()) {
    case afcIllegalParameter:
      msg = "function parameter is illegal";
      break;

    case afcNoFlow:
      msg = "min-cost flow could not find a legal flow";
      break;

    case afcSort:
      msg = "sequence not sorted";
      break;

    case afcLabel:
      msg = "labelling failed";
      break;

    case afcExternalFace:
      msg = "external face not correct";
      break;

    case afcForbiddenCrossing:
      msg = "crossing forbidden but necessary";
      break;

    case afcTimelimitExceeded:
      msg = "it took too long";
      break;

    case afcNoSolutionFound:
      msg = "couldn't solve the problem";
      break;

    default:
      msg = "unknown error";
    }

    pluginProgress->setError(std::string("Error\n") + msg);
    return false;
  }

  // retrieve nodes coordinates computed by the OGDF Layout Algorithm
  // and store them in the Tulip Layout Property
  const std::vector<tlp::node>& nodes = graph->nodes();
  unsigned int nbElts = nodes.size();

  for (unsigned int i = 0; i < nbElts; ++i) {
    tlp::Coord nodeCoord = tlpToOGDF->getNodeCoordFromOGDFGraphAttr(i);
    result->setNodeValue(nodes[i], nodeCoord);
  }

  // same operation as above but with edges
  const std::vector<tlp::edge>& edges = graph->edges();
  nbElts = edges.size();

  for (unsigned int i = 0; i < nbElts; ++i) {
    vector<tlp::Coord> edgeCoord =
      tlpToOGDF->getEdgeCoordFromOGDFGraphAttr(i);
    result->setEdgeValue(edges[i], edgeCoord);
  }

  afterCall();

  return true;
}

void OGDFLayoutPluginBase::callOGDFLayoutAlgorithm(ogdf::GraphAttributes &gAttributes) {
  ogdfLayoutAlgo->call(gAttributes);
}

void OGDFLayoutPluginBase::transposeLayoutVertically() {
  const std::vector<tlp::node>& nodes = graph->nodes();
  const std::vector<tlp::edge>& edges = graph->edges();
  tlp::BoundingBox graphBB =
    tlp::computeBoundingBox(nodes, edges, result,
                            graph->getProperty<SizeProperty>("viewSize"),
                            graph->getProperty<DoubleProperty>("viewRotation"));
  float midY = (graphBB[0][1] + graphBB[1][1]) / 2.f;
  unsigned int nbElts = nodes.size();

  for (unsigned int i = 0; i < nbElts; ++i) {
    tlp::Coord nodeCoord = result->getNodeValue(nodes[i]);
    nodeCoord[1] = midY - (nodeCoord[1] - midY);
    result->setNodeValue(nodes[i], nodeCoord);
  }

  nbElts = edges.size();

  for (unsigned int i = 0; i < nbElts; ++i) {
    std::vector<tlp::Coord> bends = result->getEdgeValue(edges[i]);

    if (bends.size()) {
      for (size_t i = 0 ; i < bends.size() ; ++i) {
        bends[i][1] = midY - (bends[i][1] - midY);
      }

      result->setEdgeValue(edges[i], bends);
    }
  }
}
