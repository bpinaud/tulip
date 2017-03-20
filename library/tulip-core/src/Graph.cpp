/**
 *
 * This file is part of Tulip (www.tulip-software.org)
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

#include <iomanip>
#include <fstream>
#include <stack>

#include <tulip/ForEach.h>
#include <tulip/StlIterator.h>
#include <tulip/TlpTools.h>
#include <tulip/Graph.h>
#include <tulip/GraphImpl.h>
#include <tulip/BooleanProperty.h>
#include <tulip/ColorProperty.h>
#include <tulip/DoubleProperty.h>
#include <tulip/IntegerProperty.h>
#include <tulip/GraphProperty.h>
#include <tulip/LayoutProperty.h>
#include <tulip/SizeProperty.h>
#include <tulip/StringProperty.h>
#include <tulip/DataSet.h>
#include <tulip/ExportModule.h>
#include <tulip/Algorithm.h>
#include <tulip/ImportModule.h>
#include <tulip/SimplePluginProgress.h>
#include <tulip/BoundingBox.h>
#include <tulip/DrawingTools.h>
#include <tulip/TulipViewSettings.h>
#include <tulip/TulipFontAwesome.h>
#include <tulip/TulipMaterialDesignIcons.h>

using namespace std;
using namespace tlp;

ostream &operator<<(ostream &os, const Graph *sp) {
  os << ";(nodes <node_id> <node_id> ...)" << endl;
  os << "(nodes ";
  const std::vector<node> &nodes = sp->nodes();
  unsigned int nbElts = nodes.size();

  if (nbElts) {
    node beginNode, previousNode;
    beginNode = previousNode = nodes[0];
    os << beginNode.id;
    unsigned int i = 0;
    for (unsigned int j = 1; j < nbElts; ++j) {
      node current = nodes[j];
      if (current.id == previousNode.id + 1) {
        previousNode = current;

        if (++i == sp->numberOfNodes())
          os << ".." << current.id;
      } else {
        if (previousNode != beginNode) {
          os << ".." << previousNode.id;
        }

        os << " " << current.id;
        beginNode = previousNode = current;
      }
    }
  }

  os << ")" << endl;
  os << ";(edge <edge_id> <source_id> <target_id>)" << endl;

  const std::vector<edge> &edges = sp->edges();
  nbElts = edges.size();
  for (unsigned int i = 0; i < nbElts; ++i) {
    edge e = edges[i];
    std::pair<node, node> ends = sp->ends(e);
    os << "(edge " << e.id << " " << ends.first.id << " " << ends.second.id << ")" << endl;
  }

  return os;
}

static void setViewPropertiesDefaults(Graph *g) {

  const std::string shapes = "viewShape", colors = "viewColor", sizes = "viewSize", metrics = "viewMetric", fonts = "viewFont",
                    fontAwesomeIcon = "viewFontAwesomeIcon", fontSizes = "viewFontSize", borderWidth = "viewBorderWidth",
                    borderColor = "viewBorderColor", tgtShape = "viewTgtAnchorShape", srcShape = "viewSrcAnchorShape", labelColor = "viewLabelColor",
                    labelBorderColor = "viewLabelBorderColor", labelBorderWidth = "viewLabelBorderWidth", labelPosition = "viewLabelPosition",
                    label = "viewLabel", layout = "viewLayout", materialDesignIcon = "viewMaterialDesignIcon", rotation = "viewRotation",
                    srcAnchorSize = "viewSrcAnchorSize", selection = "viewSelection", texture = "viewTexture", tgtAnchorSize = "viewTgtAnchorSize";

  if (!g->existProperty(shapes)) {
    g->getProperty<IntegerProperty>(shapes)->setAllNodeValue(TulipViewSettings::instance().defaultShape(NODE));
    g->getProperty<IntegerProperty>(shapes)->setAllEdgeValue(TulipViewSettings::instance().defaultShape(EDGE));
  }

  if (!g->existProperty(colors)) {
    g->getProperty<ColorProperty>(colors)->setAllNodeValue(TulipViewSettings::instance().defaultColor(NODE));
    g->getProperty<ColorProperty>(colors)->setAllEdgeValue(TulipViewSettings::instance().defaultColor(EDGE));
  }

  if (!g->existProperty(sizes)) {
    g->getProperty<SizeProperty>(sizes)->setAllNodeValue(TulipViewSettings::instance().defaultSize(NODE));
    g->getProperty<SizeProperty>(sizes)->setAllEdgeValue(TulipViewSettings::instance().defaultSize(EDGE));
  }

  if (!g->existProperty(metrics)) {
    g->getProperty<DoubleProperty>(metrics)->setAllNodeValue(0);
    g->getProperty<DoubleProperty>(metrics)->setAllEdgeValue(0);
  }

  if (!g->existProperty(fonts)) {
    g->getProperty<StringProperty>(fonts)->setAllNodeValue(TulipViewSettings::instance().defaultFontFile());
    g->getProperty<StringProperty>(fonts)->setAllEdgeValue(TulipViewSettings::instance().defaultFontFile());
  }

  if (!g->existProperty(fontSizes)) {
    g->getProperty<IntegerProperty>(fontSizes)->setAllNodeValue(TulipViewSettings::instance().defaultFontSize());
    g->getProperty<IntegerProperty>(fontSizes)->setAllEdgeValue(TulipViewSettings::instance().defaultFontSize());
  }

  if (!g->existProperty(borderWidth)) {
    g->getProperty<DoubleProperty>(borderWidth)->setAllNodeValue(TulipViewSettings::instance().defaultBorderWidth(NODE));
    g->getProperty<DoubleProperty>(borderWidth)->setAllEdgeValue(TulipViewSettings::instance().defaultBorderWidth(EDGE));
  }

  if (!g->existProperty(borderColor)) {
    g->getProperty<ColorProperty>(borderColor)->setAllNodeValue(TulipViewSettings::instance().defaultBorderColor(NODE));
    g->getProperty<ColorProperty>(borderColor)->setAllEdgeValue(TulipViewSettings::instance().defaultBorderColor(EDGE));
  }

  if (!g->existProperty(tgtShape)) {
    g->getProperty<IntegerProperty>(tgtShape)->setAllEdgeValue(TulipViewSettings::instance().defaultEdgeExtremityTgtShape());
  }

  if (!g->existProperty(srcShape)) {
    g->getProperty<IntegerProperty>(srcShape)->setAllEdgeValue(TulipViewSettings::instance().defaultEdgeExtremitySrcShape());
  }

  if (!g->existProperty(labelColor)) {
    g->getProperty<ColorProperty>(labelColor)->setAllNodeValue(TulipViewSettings::instance().defaultLabelColor());
    g->getProperty<ColorProperty>(labelColor)->setAllEdgeValue(TulipViewSettings::instance().defaultLabelColor());
  }

  if (!g->existProperty(labelBorderColor)) {
    g->getProperty<ColorProperty>(labelBorderColor)->setAllNodeValue(TulipViewSettings::instance().defaultLabelBorderColor());
    g->getProperty<ColorProperty>(labelBorderColor)->setAllEdgeValue(TulipViewSettings::instance().defaultLabelBorderColor());
  }

  if (!g->existProperty(labelBorderWidth)) {
    g->getProperty<DoubleProperty>(labelBorderWidth)->setAllNodeValue(TulipViewSettings::instance().defaultLabelBorderWidth());
    g->getProperty<DoubleProperty>(labelBorderWidth)->setAllEdgeValue(TulipViewSettings::instance().defaultLabelBorderWidth());
  }

  if (!g->existProperty(labelPosition)) {
    g->getProperty<IntegerProperty>(labelPosition)->setAllNodeValue(TulipViewSettings::instance().defaultLabelPosition());
    g->getProperty<IntegerProperty>(labelPosition)->setAllEdgeValue(TulipViewSettings::instance().defaultLabelPosition());
  }

  if (!g->existProperty(layout)) {
    g->getProperty<LayoutProperty>(layout)->setAllNodeValue(Coord(0, 0, 0));
    g->getProperty<LayoutProperty>(layout)->setAllEdgeValue(std::vector<Coord>());
  }

  if (!g->existProperty(rotation)) {
    g->getProperty<DoubleProperty>(rotation)->setAllNodeValue(0);
    g->getProperty<DoubleProperty>(rotation)->setAllEdgeValue(0);
  }

  if (!g->existProperty(srcAnchorSize)) {
    g->getProperty<SizeProperty>(srcAnchorSize)->setAllEdgeValue(TulipViewSettings::instance().defaultEdgeExtremitySrcSize());
  }

  if (!g->existProperty(tgtAnchorSize)) {
    g->getProperty<SizeProperty>(tgtAnchorSize)->setAllEdgeValue(TulipViewSettings::instance().defaultEdgeExtremityTgtSize());
  }

  if (!g->existProperty(texture)) {
    g->getProperty<StringProperty>(texture)->setAllNodeValue("");
    g->getProperty<StringProperty>(texture)->setAllEdgeValue("");
  }

  if (!g->existProperty(label)) {
    g->getProperty<StringProperty>(label)->setAllNodeValue("");
    g->getProperty<StringProperty>(label)->setAllEdgeValue("");
  }

  if (!g->existProperty(selection)) {
    g->getProperty<BooleanProperty>(selection)->setAllNodeValue(false);
    g->getProperty<BooleanProperty>(selection)->setAllEdgeValue(false);
  }

  if (!g->existProperty(fontAwesomeIcon)) {
    g->getProperty<StringProperty>(fontAwesomeIcon)->setAllNodeValue(TulipFontAwesome::QuestionCircle);
    g->getProperty<StringProperty>(fontAwesomeIcon)->setAllEdgeValue(TulipFontAwesome::QuestionCircle);
  }

  if (!g->existProperty(materialDesignIcon)) {
    g->getProperty<StringProperty>(materialDesignIcon)->setAllNodeValue(TulipMaterialDesignIcons::HelpCircle);
    g->getProperty<StringProperty>(materialDesignIcon)->setAllEdgeValue(TulipMaterialDesignIcons::HelpCircle);
  }
}

//=========================================================
Graph *tlp::newGraph() {
  Graph *g = new GraphImpl();
  setViewPropertiesDefaults(g);
  return g;
}
//=========================================================
Graph *tlp::loadGraph(const std::string &filename, PluginProgress *progress) {
  DataSet dataSet;
  std::string importPluginName = "TLP Import";

  bool gzip = false;

  if (filename.rfind(".gz") == (filename.length() - 3)) {
    gzip = true;
  }

  list<string> importPlugins = PluginLister::instance()->availablePlugins<ImportModule>();

  for (list<string>::iterator it = importPlugins.begin(); it != importPlugins.end(); ++it) {
    const ImportModule &importPlugin = static_cast<const ImportModule &>(PluginLister::instance()->pluginInformation(*it));
    list<string> extensions = importPlugin.fileExtensions();

    for (list<string>::iterator itE = extensions.begin(); itE != extensions.end(); ++itE)
      if (filename.rfind(*itE) == (filename.size() - (*itE).size())) {
        importPluginName = importPlugin.name();
        break;
      }
  }

  if (gzip && importPluginName != "TLP Import" && importPluginName != "TLPB Import") {
    tlp::error() << "GZip compression is only supported for TLP and TLPB formats." << endl;
    return nullptr;
  }

  dataSet.set("file::filename", filename);
  Graph *graph = tlp::importGraph(importPluginName, dataSet, progress);
  return graph;
}
//=========================================================
bool tlp::saveGraph(Graph *graph, const std::string &filename, PluginProgress *progress) {
  ostream *os;

  string filenameCp = filename;
  bool gzip = false;

  if (filename.rfind(".gz") == (filename.length() - 3)) {
    gzip = true;
    filenameCp = filenameCp.substr(0, filenameCp.size() - 3);
  }

  string exportPluginName = "TLP Export";
  list<string> exportPlugins = PluginLister::instance()->availablePlugins<ExportModule>();

  for (list<string>::iterator it = exportPlugins.begin(); it != exportPlugins.end(); ++it) {
    const ExportModule &exportPlugin = static_cast<const ExportModule &>(PluginLister::instance()->pluginInformation(*it));
    string ext = exportPlugin.fileExtension();

    if (filenameCp.rfind(ext) == (filenameCp.size() - ext.size())) {
      exportPluginName = exportPlugin.name();
      break;
    }
  }

  if (gzip && exportPluginName != "TLP Export" && exportPluginName != "TLPB Export") {
    tlp::error() << "GZip compression is only supported for TLP and TLPB formats." << endl;
    return false;
  }

  if (gzip) {
    os = tlp::getOgzstream(filename);
  } else {
    std::ios_base::openmode openMode = ios::out;

    if (exportPluginName == "TLPB Export") {
      openMode |= ios::binary;
    }

    os = tlp::getOutputFileStream(filename, openMode);
  }

  bool result;
  DataSet data;
  data.set("file", filename);
  result = tlp::exportGraph(graph, *os, exportPluginName, data, progress);
  delete os;
  return result;
}
//=========================================================
Graph *tlp::importGraph(const std::string &format, DataSet &dataSet, PluginProgress *progress, Graph *graph) {

  if (!PluginLister::pluginExists(format)) {
    tlp::warning() << "libtulip: " << __FUNCTION__ << ": import plugin \"" << format << "\" does not exist (or is not loaded)" << endl;
    return nullptr;
  }

  bool newGraphP = false;

  if (graph == nullptr) {
    graph = tlp::newGraph();
    newGraphP = true;
  }

  PluginProgress *tmpProgress;
  bool deletePluginProgress = false;

  if (progress == nullptr) {
    tmpProgress = new SimplePluginProgress();
    deletePluginProgress = true;
  } else
    tmpProgress = progress;

  AlgorithmContext *tmp = new AlgorithmContext(graph, &dataSet, tmpProgress);
  ImportModule *newImportModule = PluginLister::instance()->getPluginObject<ImportModule>(format, tmp);
  assert(newImportModule != nullptr);

  // ensure that the parsing of float or double does not depend on locale
  setlocale(LC_NUMERIC, "C");

  // If the import failed and we created the graph then delete the graph
  if (!newImportModule->importGraph()) {
    graph = nullptr;

    if (newGraphP)
      delete graph;
  } else {
    std::string filename;

    if (dataSet.get("file::filename", filename)) {
      graph->setAttribute("file", filename);
    }
  }

  if (deletePluginProgress)
    delete tmpProgress;

  delete newImportModule;
  dataSet = *tmp->dataSet;

  return graph;
}
//=========================================================
bool tlp::exportGraph(Graph *graph, std::ostream &outputStream, const std::string &format, DataSet &dataSet, PluginProgress *progress) {
  if (!PluginLister::pluginExists(format)) {
    tlp::warning() << "libtulip: " << __FUNCTION__ << ": export plugin \"" << format << "\" does not exist (or is not loaded)" << endl;
    return false;
  }

  bool result;
  bool deletePluginProgress = false;
  PluginProgress *tmpProgress = nullptr;

  if (progress == nullptr) {
    tmpProgress = new SimplePluginProgress();
    deletePluginProgress = true;
  } else
    tmpProgress = progress;

  AlgorithmContext *context = new AlgorithmContext(graph, &dataSet, tmpProgress);
  ExportModule *newExportModule = PluginLister::instance()->getPluginObject<ExportModule>(format, context);
  assert(newExportModule != nullptr);
  std::string filename;

  if (dataSet.get("file", filename)) {
    graph->setAttribute("file", filename);
  }

  result = newExportModule->exportGraph(outputStream);

  if (deletePluginProgress)
    delete tmpProgress;

  delete newExportModule;
  return result;
}
//=========================================================
static void removeFromGraph(Graph *g, const vector<node> &nodes, const std::vector<edge> &edges) {
  unsigned int nbNodes = nodes.size();
  unsigned int nbEdges = edges.size();

  // Clean properties
  std::string prop;
  forEach(prop, g->getProperties()) {
    PropertyInterface *p = g->getProperty(prop);

    for (unsigned int i = 0; i < nbNodes; i++)
      p->erase(nodes[i]);

    for (unsigned int i = 0; i < nbEdges; i++)
      p->erase(edges[i]);
  }

  // Remove edges
  for (unsigned int i = 0; i < nbEdges; i++)
    g->delEdge(edges[i]);

  // Remove nodes
  for (unsigned int i = 0; i < nbNodes; i++)
    g->delNode(nodes[i]);
}

void tlp::removeFromGraph(Graph *ioG, BooleanProperty *inSel) {
  if (!ioG)
    return;

  if (!inSel) {
    ::removeFromGraph(ioG, ioG->nodes(), ioG->edges());
    return;
  }

  vector<edge> edgeA;
  // get edges
  const std::vector<edge> &edges = ioG->edges();
  unsigned int nbElts = edges.size();
  for (unsigned int i = 0; i < nbElts; ++i) {
    edge e = edges[i];
    if (inSel->getEdgeValue(e)) {
      // selected edge -> remove it !
      edgeA.push_back(e);
    } else {
      // unselected edge -> don't remove node ends !
      std::pair<node, node> ends = ioG->ends(e);
      inSel->setNodeValue(ends.first, false);
      inSel->setNodeValue(ends.second, false);
    }
  }

  vector<node> nodeA;
  // get nodes
  const std::vector<node> &nodes = ioG->nodes();
  nbElts = nodes.size();
  for (unsigned int i = 0; i < nbElts; ++i) {
    node n = nodes[i];
    if (inSel->getNodeValue(n)) {
      // selected node -> remove it !
      nodeA.push_back(n);
    }
  }
  ::removeFromGraph(ioG, nodeA, edgeA);
}

void tlp::copyToGraph(Graph *outG, const Graph *inG, BooleanProperty *inSel, BooleanProperty *outSel) {
  if (outSel) {
    outSel->setAllNodeValue(false);
    outSel->setAllEdgeValue(false);
  }

  if (!outG || !inG)
    return;

  // extend the selection to edge ends
  if (inSel) {
    edge e;
    forEach(e, inSel->getNonDefaultValuatedEdges(inG)) {
      const pair<node, node> eEnds = inG->ends(e);
      inSel->setNodeValue(eEnds.first, true);
      inSel->setNodeValue(eEnds.second, true);
    }
  }

  MutableContainer<node> nodeTrl;
  nodeTrl.setAll(node());
  // loop on selected nodes
  Iterator<node> *nodeIt = inSel ? inSel->getNonDefaultValuatedNodes(inG) : inG->getNodes();

  node nIn;
  forEach(nIn, nodeIt) {
    // add outG corresponding node
    node nOut = outG->addNode();

    // select added node
    if (outSel)
      outSel->setNodeValue(nOut, true);

    // add to translation tab
    nodeTrl.set(nIn.id, nOut);

    // copy node properties
    PropertyInterface *src = nullptr;
    forEach(src, inG->getObjectProperties()) {
      if (dynamic_cast<GraphProperty *>(src) == nullptr) {
        const std::string &pName = src->getName();
        PropertyInterface *dst = outG->existProperty(pName) ? outG->getProperty(pName) : src->clonePrototype(outG, pName);
        dst->copy(nOut, nIn, src);
      }
    }
  }

  // loop on selected edges
  Iterator<edge> *edgeIt = inSel ? inSel->getNonDefaultValuatedEdges(inG) : inG->getEdges();

  edge eIn;
  forEach(eIn, edgeIt) {
    const pair<node, node> &eEnds = inG->ends(eIn);
    // add outG correponding edge
    edge eOut = outG->addEdge(nodeTrl.get(eEnds.first.id), nodeTrl.get(eEnds.second.id));

    // select added edge
    if (outSel)
      outSel->setEdgeValue(eOut, true);

    // copy edge properties
    PropertyInterface *src = nullptr;
    forEach(src, inG->getObjectProperties()) {
      if (dynamic_cast<GraphProperty *>(src) == nullptr) {
        const std::string &pName = src->getName();
        PropertyInterface *dst = outG->existProperty(pName) ? outG->getProperty(pName) : src->clonePrototype(outG, pName);
        dst->copy(eOut, eIn, src);
      }
    }
  }
}

// define a class of Iterator to iterate the root graphs
class RootGraphsIterator : public tlp::Iterator<tlp::Graph *> {
  std::vector<Graph *> roots;
  Iterator<Graph *> *rootsIterator;

public:
  RootGraphsIterator() {
    const VectorGraph &ovg = tlp::Observable::getObservableGraph();

    // we iterate the observable graph nodes
    for (node n : ovg.getNodes()) {
      Graph *g;

      if (tlp::Observable::isAlive(n) && (g = dynamic_cast<Graph *>(tlp::Observable::getObject(n)))) {
        // check if g is a root graph
        if (g->getRoot() == g)
          roots.push_back(g);
      }
    }

    rootsIterator = new StlIterator<Graph *, std::vector<tlp::Graph *>::iterator>(roots.begin(), roots.end());
  }

  ~RootGraphsIterator() {
    delete rootsIterator;
  }

  Graph *next() {
    return rootsIterator->next();
  }

  bool hasNext() {
    return rootsIterator->hasNext();
  }
};

Iterator<Graph *> *tlp::getRootGraphs() {
  return new RootGraphsIterator();
}

bool Graph::applyAlgorithm(const std::string &algorithm, std::string &errorMessage, DataSet *dataSet, PluginProgress *progress) {
  if (!PluginLister::pluginExists(algorithm)) {
    tlp::warning() << "libtulip: " << __FUNCTION__ << ": algorithm plugin \"" << algorithm << "\" does not exist (or is not loaded)" << endl;
    return false;
  }

  bool result;
  bool deletePluginProgress = false;
  PluginProgress *tmpProgress;

  if (progress == nullptr) {
    tmpProgress = new SimplePluginProgress();
    deletePluginProgress = true;
  } else
    tmpProgress = progress;

  AlgorithmContext *context = new AlgorithmContext(this, dataSet, tmpProgress);
  Algorithm *newAlgo = PluginLister::instance()->getPluginObject<Algorithm>(algorithm, context);

  if ((result = newAlgo->check(errorMessage))) {
    result = newAlgo->run();

    if (!result)
      errorMessage = tmpProgress->getError();
  }

  delete newAlgo;
  delete context;

  if (deletePluginProgress)
    delete tmpProgress;

  return result;
}

//=========================================================
bool tlp::Graph::applyPropertyAlgorithm(const std::string &algorithm, PropertyInterface *prop, std::string &errorMessage,
                                        tlp::PluginProgress *progress, tlp::DataSet *parameters) {
  bool result;
  tlp::AlgorithmContext context;

  // check if this is a subgraph of prop->graph
  if (getRoot() != prop->getGraph()) {
    tlp::Graph *currentGraph = this;

    while (currentGraph->getSuperGraph() != currentGraph) {
      if (currentGraph == prop->getGraph())
        break;

      currentGraph = currentGraph->getSuperGraph();
    }

    if (currentGraph != prop->getGraph()) {
      errorMessage = "The property parameter does not belong to the graph";
#ifndef NDEBUG
      tlp::error() << __PRETTY_FUNCTION__ << ": " << errorMessage << std::endl;
#endif
      return false;
    }
  }

  TLP_HASH_MAP<std::string, PropertyInterface *>::const_iterator it = circularCalls.find(algorithm);

  if (it != circularCalls.end() && (*it).second == prop) {
    errorMessage = std::string("Circular call of ") + __PRETTY_FUNCTION__;
#ifndef NDEBUG
    tlp::error() << errorMessage << std::endl;
#endif
    return false;
  }

  // nothing to do if the graph is empty
  if (numberOfNodes() == 0) {
    errorMessage = "The graph is empty";
    return false;
  }

  tlp::PluginProgress *tmpProgress;

  if (progress == nullptr)
    tmpProgress = new tlp::SimplePluginProgress();
  else
    tmpProgress = progress;

  bool hasData = parameters != nullptr;

  if (!hasData)
    parameters = new tlp::DataSet();

  // add prop as result in dataset
  parameters->set("result", prop);

  context.pluginProgress = tmpProgress;
  context.graph = this;
  context.dataSet = parameters;

  tlp::Observable::holdObservers();
  circularCalls[algorithm] = prop;
  Algorithm *tmpAlgo = tlp::PluginLister::instance()->getPluginObject<PropertyAlgorithm>(algorithm, &context);

  if (tmpAlgo != nullptr) {
    result = tmpAlgo->check(errorMessage);

    if (result) {
      result = tmpAlgo->run();

      if (!result)
        errorMessage = tmpProgress->getError();
    }

    delete tmpAlgo;
  } else {
    errorMessage = algorithm + " - No algorithm available with this name";
#ifndef NDEBUG
    tlp::error() << __PRETTY_FUNCTION__ << ": " << errorMessage << std::endl;
#endif
    result = false;
  }

  circularCalls.erase(algorithm);
  tlp::Observable::unholdObservers();

  if (progress == nullptr)
    delete tmpProgress;

  if (hasData)
    // remove result from dataset
    parameters->remove("result");
  else
    delete parameters;

  return result;
}

tlp::node Graph::getSource() const {
  const std::vector<node> &nodes = this->nodes();
  unsigned int nbNodes = nodes.size();
  for (unsigned int i = 0; i < nbNodes; ++i) {
    node source = nodes[i];
    if (indeg(source) == 0)
      return source;
  }
  return node();
}

DataType *Graph::getAttribute(const std::string &name) const {
  return getAttributes().getData(name);
}

void Graph::setAttribute(const std::string &name, const DataType *value) {
  notifyBeforeSetAttribute(name);
  getNonConstAttributes().setData(name, value);
  notifyAfterSetAttribute(name);
}

void Graph::notifyAddNode(const node n) {
  if (hasOnlookers())
    sendEvent(GraphEvent(*this, GraphEvent::TLP_ADD_NODE, n));
}

void Graph::notifyDelNode(const node n) {
  if (hasOnlookers())
    sendEvent(GraphEvent(*this, GraphEvent::TLP_DEL_NODE, n));
}

void Graph::notifyAddEdge(const edge e) {
  if (hasOnlookers())
    sendEvent(GraphEvent(*this, GraphEvent::TLP_ADD_EDGE, e));
}

void Graph::notifyDelEdge(const edge e) {
  if (hasOnlookers())
    sendEvent(GraphEvent(*this, GraphEvent::TLP_DEL_EDGE, e));
}

void Graph::notifyReverseEdge(const edge e) {
  if (hasOnlookers())
    sendEvent(GraphEvent(*this, GraphEvent::TLP_REVERSE_EDGE, e));
}

void Graph::notifyBeforeSetEnds(const edge e) {
  if (hasOnlookers())
    sendEvent(GraphEvent(*this, GraphEvent::TLP_BEFORE_SET_ENDS, e, Event::TLP_INFORMATION));
}

void Graph::notifyAfterSetEnds(const edge e) {
  if (hasOnlookers())
    sendEvent(GraphEvent(*this, GraphEvent::TLP_AFTER_SET_ENDS, e));
}

void Graph::notifyBeforeAddSubGraph(const Graph *graph) {
  if (hasOnlookers())
    sendEvent(GraphEvent(*this, GraphEvent::TLP_BEFORE_ADD_SUBGRAPH, graph));

  Graph *g = this;

  while (g != getRoot()) {
    g->notifyBeforeAddDescendantGraph(graph);
    g = g->getSuperGraph();
  }

  getRoot()->notifyBeforeAddDescendantGraph(graph);
}

void Graph::notifyAfterAddSubGraph(const Graph *graph) {
  if (hasOnlookers())
    sendEvent(GraphEvent(*this, GraphEvent::TLP_AFTER_ADD_SUBGRAPH, graph));

  Graph *g = this;

  while (g != getRoot()) {
    g->notifyAfterAddDescendantGraph(graph);
    g = g->getSuperGraph();
  }

  getRoot()->notifyAfterAddDescendantGraph(graph);
}

void Graph::notifyBeforeDelSubGraph(const Graph *graph) {
  if (hasOnlookers())
    sendEvent(GraphEvent(*this, GraphEvent::TLP_BEFORE_DEL_SUBGRAPH, graph));

  Graph *g = this;

  while (g != getRoot()) {
    g->notifyBeforeDelDescendantGraph(graph);
    g = g->getSuperGraph();
  }

  getRoot()->notifyBeforeDelDescendantGraph(graph);
}
void Graph::notifyAfterDelSubGraph(const Graph *graph) {
  if (hasOnlookers())
    sendEvent(GraphEvent(*this, GraphEvent::TLP_AFTER_DEL_SUBGRAPH, graph));

  Graph *g = this;

  while (g != getRoot()) {
    g->notifyAfterDelDescendantGraph(graph);
    g = g->getSuperGraph();
  }

  getRoot()->notifyAfterDelDescendantGraph(graph);
}

void Graph::notifyBeforeAddDescendantGraph(const Graph *graph) {
  if (hasOnlookers())
    sendEvent(GraphEvent(*this, GraphEvent::TLP_BEFORE_ADD_DESCENDANTGRAPH, graph));
}
void Graph::notifyAfterAddDescendantGraph(const Graph *graph) {
  if (hasOnlookers())
    sendEvent(GraphEvent(*this, GraphEvent::TLP_AFTER_ADD_DESCENDANTGRAPH, graph));
}

void Graph::notifyBeforeDelDescendantGraph(const Graph *graph) {
  if (hasOnlookers())
    sendEvent(GraphEvent(*this, GraphEvent::TLP_BEFORE_DEL_DESCENDANTGRAPH, graph));
}
void Graph::notifyAfterDelDescendantGraph(const Graph *graph) {
  if (hasOnlookers())
    sendEvent(GraphEvent(*this, GraphEvent::TLP_AFTER_DEL_DESCENDANTGRAPH, graph));
}

void Graph::notifyBeforeAddLocalProperty(const std::string &propName) {
  if (hasOnlookers())
    sendEvent(GraphEvent(*this, GraphEvent::TLP_BEFORE_ADD_LOCAL_PROPERTY, propName));
}
void Graph::notifyAddLocalProperty(const std::string &propName) {
  if (hasOnlookers())
    sendEvent(GraphEvent(*this, GraphEvent::TLP_ADD_LOCAL_PROPERTY, propName));
}

void Graph::notifyBeforeDelLocalProperty(const std::string &propName) {
  if (hasOnlookers())
    sendEvent(GraphEvent(*this, GraphEvent::TLP_BEFORE_DEL_LOCAL_PROPERTY, propName, Event::TLP_INFORMATION));
}

void Graph::notifyAfterDelLocalProperty(const std::string &propName) {
  if (hasOnlookers())
    sendEvent(GraphEvent(*this, GraphEvent::TLP_AFTER_DEL_LOCAL_PROPERTY, propName));
}

void Graph::notifyBeforeSetAttribute(const std::string &attName) {
  if (hasOnlookers())
    sendEvent(GraphEvent(*this, GraphEvent::TLP_BEFORE_SET_ATTRIBUTE, attName, Event::TLP_INFORMATION));
}

void Graph::notifyAfterSetAttribute(const std::string &attName) {
  if (hasOnlookers())
    sendEvent(GraphEvent(*this, GraphEvent::TLP_AFTER_SET_ATTRIBUTE, attName));
}

void Graph::notifyRemoveAttribute(const std::string &attName) {
  if (hasOnlookers())
    sendEvent(GraphEvent(*this, GraphEvent::TLP_REMOVE_ATTRIBUTE, attName, Event::TLP_INFORMATION));
}

void Graph::notifyDestroy() {
  if (hasOnlookers()) {
    // the undo/redo mechanism has to simulate graph destruction
    Event evt(*this, Event::TLP_MODIFICATION);
    evt._type = Event::TLP_DELETE;
    sendEvent(evt);
  }
}
PropertyInterface *Graph::getLocalProperty(const std::string &propertyName, const std::string &propertyType) {
  if (propertyType.compare(DoubleProperty::propertyTypename) == 0) {
    return getLocalProperty<DoubleProperty>(propertyName);
  } else if (propertyType.compare(LayoutProperty::propertyTypename) == 0) {
    return getLocalProperty<LayoutProperty>(propertyName);
  } else if (propertyType.compare(StringProperty::propertyTypename) == 0) {
    return getLocalProperty<StringProperty>(propertyName);
  } else if (propertyType.compare(IntegerProperty::propertyTypename) == 0) {
    return getLocalProperty<IntegerProperty>(propertyName);
  } else if (propertyType.compare(ColorProperty::propertyTypename) == 0) {
    return getLocalProperty<ColorProperty>(propertyName);
  } else if (propertyType.compare(SizeProperty::propertyTypename) == 0) {
    return getLocalProperty<SizeProperty>(propertyName);
  } else if (propertyType.compare(BooleanProperty::propertyTypename) == 0) {
    return getLocalProperty<BooleanProperty>(propertyName);
  } else if (propertyType.compare(DoubleVectorProperty::propertyTypename) == 0) {
    return getLocalProperty<DoubleVectorProperty>(propertyName);
  } else if (propertyType.compare(StringVectorProperty::propertyTypename) == 0) {
    return getLocalProperty<StringVectorProperty>(propertyName);
  } else if (propertyType.compare(IntegerVectorProperty::propertyTypename) == 0) {
    return getLocalProperty<IntegerVectorProperty>(propertyName);
  } else if (propertyType.compare(CoordVectorProperty::propertyTypename) == 0) {
    return getLocalProperty<CoordVectorProperty>(propertyName);
  } else if (propertyType.compare(ColorVectorProperty::propertyTypename) == 0) {
    return getLocalProperty<ColorVectorProperty>(propertyName);
  } else if (propertyType.compare(BooleanVectorProperty::propertyTypename) == 0) {
    return getLocalProperty<BooleanVectorProperty>(propertyName);
  } else if (propertyType.compare(SizeVectorProperty::propertyTypename) == 0) {
    return getLocalProperty<SizeVectorProperty>(propertyName);
  } else if (propertyType.compare(GraphProperty::propertyTypename) == 0) {
    return getLocalProperty<GraphProperty>(propertyName);
  } else {
    return nullptr;
  }
}

PropertyInterface *Graph::getProperty(const std::string &propertyName, const std::string &propertyType) {
  if (propertyType.compare(DoubleProperty::propertyTypename) == 0) {
    return getProperty<DoubleProperty>(propertyName);
  } else if (propertyType.compare(LayoutProperty::propertyTypename) == 0) {
    return getProperty<LayoutProperty>(propertyName);
  } else if (propertyType.compare(StringProperty::propertyTypename) == 0) {
    return getProperty<StringProperty>(propertyName);
  } else if (propertyType.compare(IntegerProperty::propertyTypename) == 0) {
    return getProperty<IntegerProperty>(propertyName);
  } else if (propertyType.compare(ColorProperty::propertyTypename) == 0) {
    return getProperty<ColorProperty>(propertyName);
  } else if (propertyType.compare(SizeProperty::propertyTypename) == 0) {
    return getProperty<SizeProperty>(propertyName);
  } else if (propertyType.compare(BooleanProperty::propertyTypename) == 0) {
    return getProperty<BooleanProperty>(propertyName);
  } else if (propertyType.compare(DoubleVectorProperty::propertyTypename) == 0) {
    return getProperty<DoubleVectorProperty>(propertyName);
  } else if (propertyType.compare(StringVectorProperty::propertyTypename) == 0) {
    return getProperty<StringVectorProperty>(propertyName);
  } else if (propertyType.compare(IntegerVectorProperty::propertyTypename) == 0) {
    return getProperty<IntegerVectorProperty>(propertyName);
  } else if (propertyType.compare(CoordVectorProperty::propertyTypename) == 0) {
    return getProperty<CoordVectorProperty>(propertyName);
  } else if (propertyType.compare(ColorVectorProperty::propertyTypename) == 0) {
    return getProperty<ColorVectorProperty>(propertyName);
  } else if (propertyType.compare(BooleanVectorProperty::propertyTypename) == 0) {
    return getProperty<BooleanVectorProperty>(propertyName);
  } else if (propertyType.compare(SizeVectorProperty::propertyTypename) == 0) {
    return getProperty<SizeVectorProperty>(propertyName);
  } else if (propertyType.compare(GraphProperty::propertyTypename) == 0) {
    return getProperty<GraphProperty>(propertyName);
  } else {
    return nullptr;
  }
}

static const string layoutProperty = "viewLayout";
static const string sizeProperty = "viewSize";
static const string rotationProperty = "viewRotation";
static const string colorProperty = "viewColor";

static void buildMapping(Iterator<node> *it, MutableContainer<node> &mapping, GraphProperty *metaInfo, const node from = node()) {
  while (it->hasNext()) {
    node n = it->next();

    if (!from.isValid())
      mapping.set(n.id, n);
    else
      mapping.set(n.id, from);

    Graph *meta = metaInfo->getNodeValue(n);

    if (meta != nullptr)
      buildMapping(meta->getNodes(), mapping, metaInfo, mapping.get(n.id));
  }

  delete it;
}
//====================================================================================
void updatePropertiesUngroup(Graph *graph, node metanode, GraphProperty *clusterInfo) {
  if (clusterInfo->getNodeValue(metanode) == nullptr)
    return; // The metanode is not a metanode.

  LayoutProperty *graphLayout = graph->getProperty<LayoutProperty>(layoutProperty);
  SizeProperty *graphSize = graph->getProperty<SizeProperty>(sizeProperty);
  DoubleProperty *graphRot = graph->getProperty<DoubleProperty>(rotationProperty);
  const Size &size = graphSize->getNodeValue(metanode);
  const Coord &pos = graphLayout->getNodeValue(metanode);
  const double &rot = graphRot->getNodeValue(metanode);
  Graph *cluster = clusterInfo->getNodeValue(metanode);
  LayoutProperty *clusterLayout = cluster->getProperty<LayoutProperty>(layoutProperty);
  SizeProperty *clusterSize = cluster->getProperty<SizeProperty>(sizeProperty);
  DoubleProperty *clusterRot = cluster->getProperty<DoubleProperty>(rotationProperty);
  BoundingBox box = tlp::computeBoundingBox(cluster, clusterLayout, clusterSize, clusterRot);
  double width = box.width();
  double height = box.height();
  double depth = box.depth();

  if (width < 0.0001)
    width = 1.0;

  if (height < 0.0001)
    height = 1.0;

  if (depth < 0.0001)
    depth = 1.0;

  // keep aspect ratio of content layout when opening a meta-node
  double divW = size[0] / width;
  double divH = size[1] / height;
  double scale = 1.0;

  if (divH * width > size[0]) {
    scale = divW;
  } else {
    scale = divH;
  }

  clusterLayout->translate(-box.center(), cluster);
  clusterLayout->rotateZ(graphRot->getNodeValue(metanode), cluster);
  clusterLayout->scale(Coord(scale, scale, size[2] / depth), cluster);
  clusterLayout->translate(pos, cluster);
  clusterSize->scale(Size(scale, scale, size[2] / depth), cluster);

  const std::vector<node> &nodes = cluster->nodes();
  unsigned int nbNodes = nodes.size();
  for (unsigned int i = 0; i < nbNodes; ++i) {
    node n = nodes[i];
    graphLayout->setNodeValue(n, clusterLayout->getNodeValue(n));
    graphSize->setNodeValue(n, clusterSize->getNodeValue(n));
    graphRot->setNodeValue(n, clusterRot->getNodeValue(n) + rot);
  }

  const std::vector<edge> &edges = cluster->edges();
  unsigned int nbEdges = edges.size();
  for (unsigned int i = 0; i < nbEdges; ++i) {
    edge e = edges[i];
    graphLayout->setEdgeValue(e, clusterLayout->getEdgeValue(e));
    graphSize->setEdgeValue(e, clusterSize->getEdgeValue(e));
  }

  // propagate all cluster local properties
  for (PropertyInterface *property : cluster->getLocalObjectProperties()) {
    if (property == graphLayout || property == graphSize || property == graphRot)
      continue;

    PropertyInterface *graphProp = nullptr;

    if (graph->existProperty(property->getName()) && graph->getProperty(property->getName())->getTypename() == property->getTypename())
      graphProp = graph->getProperty(property->getName());
    else
      graphProp = property->clonePrototype(graph, property->getName());

    for (unsigned int i = 0; i < nbNodes; ++i) {
      node n = nodes[i];
      graphProp->setNodeStringValue(n, property->getNodeStringValue(n));
    }

    for (unsigned int i = 0; i < nbEdges; ++i) {
      edge e = edges[i];
      graphProp->setEdgeStringValue(e, property->getEdgeStringValue(e));
    }
  }
}
//=========================================================
Graph *Graph::addSubGraph(const std::string &name) {
  Graph *g = addSubGraph(nullptr, name);
  return g;
}
//=========================================================
Graph *Graph::addCloneSubGraph(const std::string &name, bool addSibling, bool addSiblingProperties) {
  BooleanProperty selection(this);
  selection.setAllNodeValue(true);
  selection.setAllEdgeValue(true);
  Graph *parentSubGraph = this;

  if (addSibling) {
    parentSubGraph = getSuperGraph();

    if (this == parentSubGraph)
      // cannot add sibling of root graph
      return nullptr;
  }

  Graph *clone = parentSubGraph->addSubGraph(&selection, name);
  if (addSibling && addSiblingProperties) {
    PropertyInterface *prop = nullptr;
    forEach(prop, getLocalObjectProperties()) {
      PropertyInterface *cloneProp = prop->clonePrototype(clone, prop->getName());
      tlp::debug() << "clone property " << prop->getName().c_str() << std::endl;
      cloneProp->copy(prop);
    }
  }
  return clone;
}
//=========================================================
Graph *Graph::inducedSubGraph(const std::set<node> &nodes, Graph *parentSubGraph, const string &name) {
  if (parentSubGraph == NULL)
    parentSubGraph = this;

  // create subgraph and add nodes
  Graph *result = parentSubGraph->addSubGraph(name);
  StlIterator<node, std::set<node>::const_iterator> it(nodes.begin(), nodes.end());
  result->addNodes(&it);

  node itn;
  forEach(itn, result->getNodes()) {
    edge ite;
    forEach(ite, getOutEdges(itn)) {
      if (result->isElement(target(ite)))
        result->addEdge(ite);
    }
  }

  return result;
}
//=========================================================
Graph *Graph::inducedSubGraph(BooleanProperty *selection, Graph *parentSubGraph, const string &name) {
  set<node> nodesSet;
  node n;
  forEach(n, selection->getNodesEqualTo(true, parentSubGraph)) {
    nodesSet.insert(n);
  }
  edge e;
  forEach(e, selection->getEdgesEqualTo(true, parentSubGraph)) {
    const pair<node, node> &ext = ends(e);
    nodesSet.insert(ext.first);
    nodesSet.insert(ext.second);
  }
  return inducedSubGraph(nodesSet, parentSubGraph, name);
}
//====================================================================================
node Graph::createMetaNode(const std::set<node> &nodeSet, bool multiEdges, bool delAllEdge) {
  if (getRoot() == this) {
    tlp::warning() << __PRETTY_FUNCTION__ << std::endl;
    tlp::warning() << "\t Error: Could not group a set of nodes in the root graph" << std::endl;
    return node();
  }

  if (nodeSet.empty()) {
    tlp::warning() << __PRETTY_FUNCTION__ << std::endl;
    tlp::warning() << '\t' << "Warning: Creation of an empty metagraph" << std::endl;
  }

  // create an induced brother sub graph
  Graph *subGraph = inducedSubGraph(nodeSet, getSuperGraph());
  // all local properties
  // must be cloned in subgraph
  for (PropertyInterface *prop : getLocalObjectProperties()) {
    PropertyInterface *sgProp = prop->clonePrototype(subGraph, prop->getName());
    set<node>::const_iterator itNodeSet = nodeSet.begin();

    for (; itNodeSet != nodeSet.end(); ++itNodeSet) {
      node n = *itNodeSet;
      DataMem *val = prop->getNodeDataMemValue(n);
      sgProp->setNodeDataMemValue(n, val);
      delete val;
    }
  }

  stringstream st;
  st << "grp_" << setfill('0') << setw(5) << subGraph->getId();
  subGraph->setAttribute("name", st.str());
  return createMetaNode(subGraph, multiEdges, delAllEdge);
}

//====================================================================================
node Graph::createMetaNode(Graph *subGraph, bool multiEdges, bool edgeDelAll) {
  if (getRoot() == this) {
    tlp::warning() << __PRETTY_FUNCTION__ << std::endl;
    tlp::warning() << "\t Error: Could not create a meta node in the root graph" << std::endl;
    return node();
  }

  GraphProperty *metaInfo = ((GraphAbstract *)getRoot())->getMetaGraphProperty();
  node metaNode = addNode();
  metaInfo->setNodeValue(metaNode, subGraph);
  Observable::holdObservers();
  // updateGroupLayout(this, subGraph, metaNode);
  // compute meta node values
  for (PropertyInterface *property : getObjectProperties()) {
    property->computeMetaValue(metaNode, subGraph, this);
  }

  // keep track of graph existing edges
  MutableContainer<bool> graphEdges;
  graphEdges.setAll(false);

  edge e;
  forEach(e, getEdges()) graphEdges.set(e.id, true);

  // we can now Remove nodes from graph
  StableIterator<node> itN(subGraph->getNodes());
  delNodes(&itN);

  // create new meta edges from nodes to metanode
  Graph *super = getSuperGraph();
  // colors = super->getProperty<ColorProperty> (colorProperty);
  TLP_HASH_MAP<node, TLP_HASH_SET<node>> edges;
  TLP_HASH_MAP<node, edge> metaEdges;
  TLP_HASH_MAP<edge, set<edge>> subEdges;

  node n;
  forEach(n, subGraph->getNodes()) {
    edge e;
    stableForEach(e, getSuperGraph()->getInOutEdges(n)) {
      pair<node, node> eEnds = ends(e);
      node src = eEnds.first;
      node tgt = eEnds.second;
      bool toDelete = ((metaInfo->getNodeValue(src) != nullptr) || (metaInfo->getNodeValue(tgt) != nullptr)) && isElement(src) && isElement(tgt) &&
                      existEdge(src, tgt).isValid();

      if (isElement(src) && subGraph->isElement(tgt)) {
        if (multiEdges || edges[src].empty()) {
          // add new meta edge
          edge metaEdge = addEdge(src, metaNode);

          if (!graphEdges.get(e.id))
            delEdge(metaEdge);

          // e is a sub-edge of metaEdge
          subEdges[metaEdge].insert(e);

          if (!multiEdges)
            // record metaEdge
            metaEdges[src] = metaEdge;

          if (!super->isElement(metaEdge))
            super->addEdge(metaEdge);
        } else if (!multiEdges)
          // e is a sub-edge of an already created meta edge
          subEdges[metaEdges[src]].insert(e);

        edges[src].insert(tgt);

        if (toDelete) {
          delEdge(e, edgeDelAll);
        }
      }

      if (isElement(tgt) && subGraph->isElement(src)) {
        if (multiEdges || edges[tgt].empty()) {
          // add new meta edge
          edge metaEdge = addEdge(metaNode, tgt);

          if (!graphEdges.get(e.id))
            delEdge(metaEdge);

          // e is a sub-edge of metaEdge
          subEdges[metaEdge].insert(e);

          if (!multiEdges)
            // record metaEdge
            metaEdges[tgt] = metaEdge;

          if (!super->isElement(metaEdge))
            super->addEdge(metaEdge);
        } else if (!multiEdges)
          // e is a sub-edge of an already created meta edge
          subEdges[metaEdges[tgt]].insert(e);

        edges[tgt].insert(src);

        if (toDelete) {
          delEdge(e, edgeDelAll);
        }
      }
    }
  }

  // update metaInfo of new meta edges
  TLP_HASH_MAP<edge, set<edge>>::const_iterator it;

  for (it = subEdges.begin(); it != subEdges.end(); ++it) {
    edge mE = (*it).first;
    metaInfo->setEdgeValue(mE, (*it).second);
    // compute meta edge values
    for (PropertyInterface *property : getObjectProperties()) {
      Iterator<edge> *itE = getEdgeMetaInfo(mE);
      assert(itE->hasNext());
      property->computeMetaValue(mE, itE, this);
      delete itE;
    }
  }

  Observable::unholdObservers();
  return metaNode;
}

// map all nodes or embedded nodes (nodes of meta node subgraph)
// of a subgraph to metaNode
static void mapSubGraphNodes(Graph *sg, node metaNode, MutableContainer<node> &mappingM, GraphProperty *metaInfo) {
  for (node n : sg->getNodes()) {
    mappingM.set(n.id, metaNode);
    Graph *ssg = metaInfo->getNodeValue(n);

    if (ssg)
      mapSubGraphNodes(ssg, metaNode, mappingM, metaInfo);
  }
}
//====================================================================================
void Graph::openMetaNode(node metaNode, bool updateProperties) {
  if (getRoot() == this) {
    tlp::warning() << __PRETTY_FUNCTION__ << std::endl;
    tlp::warning() << "\t Error: Could not ungroup a meta node in the root graph" << std::endl;
    return;
  }

  GraphProperty *metaInfo = ((GraphAbstract *)getRoot())->getMetaGraphProperty();
  Graph *metaGraph = metaInfo->getNodeValue(metaNode);

  if (metaGraph == nullptr)
    return;

  Observable::holdObservers();
  MutableContainer<node> mappingM;
  mappingM.setAll(node());
  // add node from meta to graph
  {
    // stable in case of fractal graph
    for (node n : stableIterator(metaGraph->getNodes())) {
      addNode(n);
      mappingM.set(n.id, n);
      Graph *sg = metaInfo->getNodeValue(n);

      if (sg)
        // map all nodes or embedded nodes
        // of this subgraph to n
        mapSubGraphNodes(sg, n, mappingM, metaInfo);
    }
    StableIterator<edge> ite(metaGraph->getEdges());
    addEdges(&ite);
  }

  if (updateProperties)
    updatePropertiesUngroup(this, metaNode, metaInfo);

  // check for edges from or to the meta node
  Graph *super = getSuperGraph();
  Iterator<edge> *metaEdges = super->getInOutEdges(metaNode);

  if (!metaEdges->hasNext()) {
    delete metaEdges;
    // no edge so just remove the meta node
    getRoot()->delNode(metaNode, true);
    Observable::unholdObservers();
    return;
  }

  bool hasSubEdges = super->isMetaEdge(metaEdges->next());
  delete metaEdges;

  ColorProperty *graphColors = getProperty<ColorProperty>(colorProperty);

  if (hasSubEdges) {
    // compute mapping for neighbour nodes
    // and their sub nodes
    for (node mn : super->getInOutNodes(metaNode)) {
      mappingM.set(mn.id, mn);
      Graph *mnGraph = metaInfo->getNodeValue(mn);

      if (mnGraph != nullptr) {
        node mnn;
        forEach(mnn, mnGraph->getNodes()) {
          mappingM.set(mnn.id, mn);
        }
      }
    }

    edge metaEdge;
    stableForEach(metaEdge, super->getInOutEdges(metaNode)) {
      Color metaColor = graphColors->getEdgeValue(metaEdge);
      TLP_HASH_MAP<node, TLP_HASH_MAP<node, set<edge>>> newMetaEdges;

      edge e;
      forEach(e, getEdgeMetaInfo(metaEdge)) {
        const std::pair<node, node> &eEnds = super->ends(e);

        if (isElement(eEnds.first)) {
          if (isElement(eEnds.second)) {
            addEdge(e);

            if (!isElement(metaEdge))
              delEdge(e);

            graphColors->setEdgeValue(e, metaColor);
          } else {
            node tgt = mappingM.get(eEnds.second.id);

            // tgt may not be valid because at this time
            // when deleting a node from a subgraph pointed
            // by a metanode we are not able to correctly
            // update the meta edges embedding the inout edges
            // of this node
            if (tgt.isValid())
              newMetaEdges[eEnds.first][tgt].insert(e);
          }
        } else {
          node src = mappingM.get(eEnds.first.id);

          // src may not be valid because at this time
          // when deleting a node from a subgraph pointed
          // by a metanode we are not able to correctly
          // update the meta edges embedding the inout edges
          // of this node
          if (src.isValid())
            newMetaEdges[src][eEnds.second].insert(e);
        }
      }

      // iterate on newMetaEdges
      TLP_HASH_MAP<node, TLP_HASH_MAP<node, set<edge>>>::iterator itme = newMetaEdges.begin();

      while (itme != newMetaEdges.end()) {
        node src = (*itme).first;
        TLP_HASH_MAP<node, set<edge>>::iterator itnme = (*itme).second.begin();
        TLP_HASH_MAP<node, set<edge>>::iterator itnmeEnd = (*itme).second.end();

        while (itnme != itnmeEnd) {
          Graph *graph = this;
          node tgt(itnme->first);

          // add edge in the right graph
          if (!isElement(src) || !isElement(tgt))
            graph = super;

          edge mE = graph->addEdge(src, tgt);
          metaInfo->setEdgeValue(mE, itnme->second);
          // compute meta edge values
          for (PropertyInterface *property : graph->getObjectProperties()) {
            Iterator<edge> *itE = getEdgeMetaInfo(mE);
            assert(itE->hasNext());
            property->computeMetaValue(mE, itE, graph);
            delete itE;
          }
          ++itnme;
        }

        ++itme;
      }
    }

    getRoot()->delNode(metaNode, true);
  } else {
    MutableContainer<node> mappingC;
    MutableContainer<node> mappingN;
    mappingC.setAll(node());
    mappingN.setAll(node());
    Graph *root = getRoot();
    buildMapping(root->getInOutNodes(metaNode), mappingC, metaInfo, node());
    buildMapping(metaGraph->getNodes(), mappingN, metaInfo, node());

    TLP_HASH_MAP<node, Color> metaEdgeToColor;

    edge metaEdge;
    forEach(metaEdge, super->getInOutEdges(metaNode)) {
      metaEdgeToColor[opposite(metaEdge, metaNode)] = graphColors->getEdgeValue(metaEdge);
    }

    // Remove the metagraph from the hierarchy and remove the metanode
    root->delNode(metaNode, true);
    TLP_HASH_MAP<node, TLP_HASH_SET<node>> edges;
    //=================================
    edge e;
    stableForEach(e, root->getEdges()) {

      if (isElement(e))
        continue;

      pair<node, node> eEnds = root->ends(e);
      unsigned int srcId = eEnds.first.id;
      unsigned int tgtId = eEnds.second.id;
      node sourceC = mappingC.get(srcId);
      node targetN = mappingN.get(tgtId);
      node sourceN = mappingN.get(srcId);
      node targetC = mappingC.get(tgtId);
      node src, tgt;
      Color edgeColor;

      if (sourceC.isValid() && targetN.isValid()) {
        src = sourceC;
        tgt = targetN;
        edgeColor = metaEdgeToColor[src];
      } else {
        if (sourceN.isValid() && targetC.isValid()) {
          src = sourceN;
          tgt = targetC;
          edgeColor = metaEdgeToColor[tgt];
        } else
          continue;
      }

      if (metaInfo->getNodeValue(src) == nullptr && metaInfo->getNodeValue(tgt) == nullptr) {
        addEdge(e);
        continue;
      }

      if ((edges.find(src) == edges.end()) || (edges[src].find(tgt) == edges[src].end())) {
        edges[src].insert(tgt);

        if (!existEdge(src, tgt).isValid()) {
          edge addedEdge = addEdge(src, tgt);
          graphColors->setEdgeValue(addedEdge, edgeColor);
        } else
          tlp::error() << __PRETTY_FUNCTION__ << ": bug exist edge 1" << std::endl;
      }
    }
  }

  Observable::unholdObservers();
}
//====================================================================================
struct MetaEdge {
  unsigned int source, target;
  edge mE;
};

namespace std {
template <> struct less<MetaEdge> {
  bool operator()(const MetaEdge &c, const MetaEdge &d) const {
    /*if (c.source<d.source) return true;
    if (c.source>d.source) return false;
    if (c.target<d.target) return true;
    if (c.target>d.target) return false;
    return false;*/
    return (c.source > d.source) || ((c.source == d.source) && (c.target < d.target));
  }
};
} // namespace std
//====================================================================================
void Graph::createMetaNodes(Iterator<Graph *> *itS, Graph *quotientGraph, vector<node> &metaNodes) {
  GraphProperty *metaInfo = ((GraphAbstract *)getRoot())->getMetaGraphProperty();
  map<edge, set<edge>> eMapping;
  Observable::holdObservers();
  {
    map<node, set<node>> nMapping;

    while (itS->hasNext()) {
      Graph *its = itS->next();

      if (its != quotientGraph) {
        // Create one metanode for each subgraph(cluster)
        node metaN = quotientGraph->addNode();
        metaNodes.push_back(metaN);
        metaInfo->setNodeValue(metaN, its);
        // compute meta node values
        for (PropertyInterface *property : quotientGraph->getObjectProperties()) {
          property->computeMetaValue(metaN, its, quotientGraph);
        }
        for (node n : its->getNodes()) {
          // map each subgraph's node to a set of meta nodes
          // in order to deal consistently with overlapping clusters
          if (nMapping.find(n) == nMapping.end())
            nMapping[n] = set<node>();

          nMapping[n].insert(metaN);
        }
      }
    }

    {
      set<MetaEdge> myQuotientGraph;
      // for each existing edge in the current graph
      // add a meta edge for the corresponding couple
      // (meta source, meta target) if it does not already exists
      // and register the edge as associated to this meta edge
      for (edge e : stableIterator(getEdges())) {
        pair<node, node> eEnds = ends(e);
        set<node> &metaSources = nMapping[eEnds.first];
        set<node> &metaTargets = nMapping[eEnds.second];

        for (set<node>::const_iterator itms = metaSources.begin(); itms != metaSources.end(); ++itms) {
          node mSource = *itms;

          for (set<node>::const_iterator itmt = metaTargets.begin(); itmt != metaTargets.end(); ++itmt) {
            node mTarget = *itmt;

            if (mSource != mTarget) {
              MetaEdge tmp;
              tmp.source = mSource.id, tmp.target = mTarget.id;
              set<MetaEdge>::const_iterator itm = myQuotientGraph.find(tmp);

              if (itm == myQuotientGraph.end()) {
                edge mE = quotientGraph->addEdge(mSource, mTarget);
                tmp.mE = mE;
                myQuotientGraph.insert(tmp);
                eMapping[mE].insert(e);
              } else {
                // add edge
                eMapping[(*itm).mE].insert(e);
              }
            }
          }
        }
      }
    }
  }
  // set viewMetaGraph for added meta edges
  map<edge, set<edge>>::const_iterator itm = eMapping.begin();

  while (itm != eMapping.end()) {
    edge mE = (*itm).first;
    metaInfo->setEdgeValue(mE, (*itm).second);
    // compute meta edge values
    for (const string &pName : quotientGraph->getProperties()) {
      Iterator<edge> *itE = getRoot()->getEdgeMetaInfo(mE);
      PropertyInterface *property = quotientGraph->getProperty(pName);
      property->computeMetaValue(mE, itE, quotientGraph);
      delete itE;
    }
    ++itm;
  }

  Observable::unholdObservers();
}
//====================================================================================
Graph *Graph::getNthSubGraph(unsigned int n) const {
  unsigned int i = 0;
  tlp::Graph *sg = nullptr;
  forEach(sg, getSubGraphs()) {
    if (i++ == n) {
      return sg;
    }
  }
  return nullptr;
}
//====================================================================================
const std::string &GraphEvent::getPropertyName() const {
  assert((evtType > TLP_AFTER_DEL_SUBGRAPH && evtType < TLP_BEFORE_SET_ATTRIBUTE) || evtType > TLP_REMOVE_ATTRIBUTE);

  if (evtType == TLP_BEFORE_RENAME_LOCAL_PROPERTY || evtType == TLP_AFTER_RENAME_LOCAL_PROPERTY)
    return info.renamedProp->first->getName();

  return *(info.name);
}
//====================================================================================
void Graph::addNodes(const std::vector<node> &nodes) {
  StlIterator<node, vector<node>::const_iterator> vIterator(nodes.begin(), nodes.end());
  addNodes(&vIterator);
}
//====================================================================================
void Graph::delNodes(const std::vector<node> &nodes, bool deleteInAllGraphs) {
  StlIterator<node, vector<node>::const_iterator> vIterator(nodes.begin(), nodes.end());
  delNodes(&vIterator, deleteInAllGraphs);
}
//====================================================================================
void Graph::addEdges(const std::vector<edge> &edges) {
  StlIterator<edge, vector<edge>::const_iterator> vIterator(edges.begin(), edges.end());
  addEdges(&vIterator);
}
//====================================================================================
void Graph::delEdges(const std::vector<edge> &edges, bool deleteInAllGraphs) {
  StlIterator<edge, vector<edge>::const_iterator> vIterator(edges.begin(), edges.end());
  delEdges(&vIterator, deleteInAllGraphs);
}
//====================================================================================
struct DescendantGraphsIterator : public Iterator<Graph *> {
  // use a stack to store non empty iterators
  stack<Iterator<Graph *> *> iterators;
  // the current one
  Iterator<Graph *> *current;

  DescendantGraphsIterator(const Graph *g) {
    current = g->getSubGraphs();

    if (!current->hasNext()) {
      delete current;
      current = nullptr;
    }
  }

  ~DescendantGraphsIterator() {
    if (current)
      delete current;

    while (!iterators.empty()) {
      delete iterators.top();
      iterators.pop();
    }
  }

  bool hasNext() {
    return current != nullptr;
  }

  Graph *next() {
    if (current) {
      Graph *g = current->next();
      Iterator<Graph *> *itg = g->getSubGraphs();

      if (itg->hasNext()) {
        if (current->hasNext())
          // pushed iterators are always non empty
          iterators.push(current);
        else
          delete current;

        current = itg;
      } else {
        delete itg;

        if (!current->hasNext()) {
          delete current;

          if (!iterators.empty()) {
            current = iterators.top();
            iterators.pop();
          } else
            current = nullptr;
        }
      }

      return g;
    }

    return nullptr;
  }
};
//====================================================================================
Iterator<Graph *> *Graph::getDescendantGraphs() const {
  return new DescendantGraphsIterator(this);
}
//====================================================================================
BooleanProperty &Graph::getBooleanProperty(const std::string &name) {
  return *(getProperty<BooleanProperty>(name));
}
//====================================================================================
BooleanProperty &Graph::getLocalBooleanProperty(const std::string &name) {
  return *(getLocalProperty<BooleanProperty>(name));
}
//====================================================================================
ColorProperty &Graph::getColorProperty(const std::string &name) {
  return *(getProperty<ColorProperty>(name));
}
//====================================================================================
ColorProperty &Graph::getLocalColorProperty(const std::string &name) {
  return *(getLocalProperty<ColorProperty>(name));
}
//====================================================================================
DoubleProperty &Graph::getDoubleProperty(const std::string &name) {
  return *(getProperty<DoubleProperty>(name));
}
//====================================================================================
DoubleProperty &Graph::getLocalDoubleProperty(const std::string &name) {
  return *(getLocalProperty<DoubleProperty>(name));
}
//====================================================================================
IntegerProperty &Graph::getIntegerProperty(const std::string &name) {
  return *(getProperty<IntegerProperty>(name));
}
//====================================================================================
IntegerProperty &Graph::getLocalIntegerProperty(const std::string &name) {
  return *(getLocalProperty<IntegerProperty>(name));
}
//====================================================================================
LayoutProperty &Graph::getLayoutProperty(const std::string &name) {
  return *(getProperty<LayoutProperty>(name));
}
//====================================================================================
LayoutProperty &Graph::getLocalLayoutProperty(const std::string &name) {
  return *(getLocalProperty<LayoutProperty>(name));
}
//====================================================================================
SizeProperty &Graph::getSizeProperty(const std::string &name) {
  return *(getProperty<SizeProperty>(name));
}
//====================================================================================
SizeProperty &Graph::getLocalSizeProperty(const std::string &name) {
  return *(getLocalProperty<SizeProperty>(name));
}
//====================================================================================
StringProperty &Graph::getStringProperty(const std::string &name) {
  return *(getProperty<StringProperty>(name));
}
//====================================================================================
StringProperty &Graph::getLocalStringProperty(const std::string &name) {
  return *(getLocalProperty<StringProperty>(name));
}
//====================================================================================
BooleanVectorProperty &Graph::getBooleanVectorProperty(const std::string &name) {
  return *(getProperty<BooleanVectorProperty>(name));
}
//====================================================================================
BooleanVectorProperty &Graph::getLocalBooleanVectorProperty(const std::string &name) {
  return *(getLocalProperty<BooleanVectorProperty>(name));
}
//====================================================================================
ColorVectorProperty &Graph::getColorVectorProperty(const std::string &name) {
  return *(getProperty<ColorVectorProperty>(name));
}
//====================================================================================
ColorVectorProperty &Graph::getLocalColorVectorProperty(const std::string &name) {
  return *(getLocalProperty<ColorVectorProperty>(name));
}
//====================================================================================
DoubleVectorProperty &Graph::getDoubleVectorProperty(const std::string &name) {
  return *(getProperty<DoubleVectorProperty>(name));
}
//====================================================================================
DoubleVectorProperty &Graph::getLocalDoubleVectorProperty(const std::string &name) {
  return *(getLocalProperty<DoubleVectorProperty>(name));
}
//====================================================================================
IntegerVectorProperty &Graph::getIntegerVectorProperty(const std::string &name) {
  return *(getProperty<IntegerVectorProperty>(name));
}
//====================================================================================
IntegerVectorProperty &Graph::getLocalIntegerVectorProperty(const std::string &name) {
  return *(getLocalProperty<IntegerVectorProperty>(name));
}
//====================================================================================
CoordVectorProperty &Graph::getCoordVectorProperty(const std::string &name) {
  return *(getProperty<CoordVectorProperty>(name));
}
//====================================================================================
CoordVectorProperty &Graph::getLocalCoordVectorProperty(const std::string &name) {
  return *(getLocalProperty<CoordVectorProperty>(name));
}
//====================================================================================
SizeVectorProperty &Graph::getSizeVectorProperty(const std::string &name) {
  return *(getProperty<SizeVectorProperty>(name));
}
//====================================================================================
SizeVectorProperty &Graph::getLocalSizeVectorProperty(const std::string &name) {
  return *(getLocalProperty<SizeVectorProperty>(name));
}
//====================================================================================
StringVectorProperty &Graph::getStringVectorProperty(const std::string &name) {
  return *(getProperty<StringVectorProperty>(name));
}
//====================================================================================
StringVectorProperty &Graph::getLocalStringVectorProperty(const std::string &name) {
  return *(getLocalProperty<StringVectorProperty>(name));
}
