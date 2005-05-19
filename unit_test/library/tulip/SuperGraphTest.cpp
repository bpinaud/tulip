#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include "SuperGraphTest.h"

using namespace std;
#include <cppunit/extensions/HelperMacros.h>
CPPUNIT_TEST_SUITE_REGISTRATION( SuperGraphTest );

//==========================================================
void SuperGraphTest::setUp() {
  graph = tlp::newSuperGraph();
}
//==========================================================
void SuperGraphTest::tearDown() {
  delete graph;
}
//==========================================================
void SuperGraphTest::build(unsigned int nbNodes, unsigned int edgeRatio) {
  vector<node> nodes;
  vector<edge> edges;
  unsigned int NB_ADD  = nbNodes;
  unsigned int EDGE_RATIO = edgeRatio;
  for (unsigned int i=0; i<NB_ADD; ++i)
    nodes.push_back(graph->addNode());
  unsigned int NB_EDGES = EDGE_RATIO * NB_ADD;
  for (unsigned int i=0; i< NB_EDGES; ++i)
    graph->addEdge(nodes[rand()%NB_ADD], nodes[rand()%NB_ADD]);
}
//==========================================================
void SuperGraphTest::testIterators() {
  graph->clear();
  vector<node> nodes;
  const unsigned int NB_NODES = 100;
  for (unsigned int j=0; j<NB_NODES; ++j)
    nodes.push_back(graph->addNode());
  Iterator<node> *itN = graph->getNodes();
  unsigned int i = 0;
  while(itN->hasNext()){
    CPPUNIT_ASSERT_EQUAL( nodes[i].id , itN->next().id );
    ++i;
  }delete itN;
  CPPUNIT_ASSERT_EQUAL( i , NB_NODES );
}
//==========================================================
void degreeCheck(SuperGraph *graph) {
  Iterator<node> *itN = graph->getNodes();
  while (itN->hasNext()) {
    node n = itN->next();
    unsigned int outdeg = 0;
    unsigned int indeg = 0;
    unsigned int deg = 0;
    Iterator<edge> *it = graph->getOutEdges(n);
    for (;it->hasNext();it->next())
      ++outdeg;
    delete it;
    it = graph->getInEdges(n);
    for (;it->hasNext();it->next())
      ++indeg;
    delete it;
    it = graph->getInOutEdges(n);
    for (;it->hasNext();it->next())
      ++deg;
    delete it;
    CPPUNIT_ASSERT_EQUAL(graph->indeg(n) , indeg);
    CPPUNIT_ASSERT_EQUAL(graph->outdeg(n), outdeg);
    CPPUNIT_ASSERT_EQUAL(graph->deg(n)   , deg);
    CPPUNIT_ASSERT_EQUAL(indeg + outdeg  , deg);
  } delete itN;  
}
//==========================================================
void SuperGraphTest::testDegree() {
  graph->clear();
  build(1000, 100);
  degreeCheck(graph);
  SuperGraph *gr = tlp::newCloneSubGraph(graph);
  SuperGraph *gr1 = tlp::newCloneSubGraph(graph);
  SuperGraph *gr2 = tlp::newCloneSubGraph(gr1);

  degreeCheck(graph);
  degreeCheck(gr);
  degreeCheck(gr1);
  degreeCheck(gr2);
  Iterator<edge> *it = graph->getEdges();
  while(it->hasNext()) {
    graph->reverse(it->next());
  } delete it;
  degreeCheck(graph);
  degreeCheck(gr);
  degreeCheck(gr1);
  degreeCheck(gr2);
  it = gr2->getEdges();
  while(it->hasNext()) {
    gr2->reverse(it->next());
  } delete it;
  degreeCheck(graph);
  degreeCheck(gr);
  degreeCheck(gr1);
  degreeCheck(gr2);
}
//==========================================================
void SuperGraphTest::testAddDel() {
  //  cerr << __PRETTY_FUNCTION__ << endl;
  vector<node> nodes;
  vector<edge> edges;
  unsigned int NB_ADD = 1000;
  unsigned int EDGE_RATIO = 100;

  unsigned int NB_EDGES = EDGE_RATIO * NB_ADD;
  vector<bool> edgePresent(NB_EDGES);
  vector<bool> nodePresent(NB_ADD);

  for (unsigned int i=0; i<NB_ADD; ++i) {
    nodes.push_back(graph->addNode());
    CPPUNIT_ASSERT(graph->isElement(nodes[i]));
    nodePresent[i] = true;
  }
  CPPUNIT_ASSERT(graph->numberOfNodes() == NB_ADD);


  for (unsigned int i=0; i< NB_EDGES; ++i) {
    edges.push_back(graph->addEdge(nodes[rand()%NB_ADD],nodes[rand()%NB_ADD]));
    CPPUNIT_ASSERT(graph->isElement(edges[i]));
    edgePresent[i] = true;
  }
  CPPUNIT_ASSERT(graph->numberOfEdges() == NB_EDGES);

  for (unsigned int i=0; i < NB_EDGES / 2; ++i) {
    graph->delEdge(edges[i]);
    CPPUNIT_ASSERT(!graph->isElement(edges[i]));
  }
  CPPUNIT_ASSERT(graph->numberOfEdges() == NB_EDGES - NB_EDGES/2);

  for (unsigned int i=0; i<NB_ADD; ++i) {
    Iterator<edge> *itE = graph->getInOutEdges(nodes[i]);
    while (itE->hasNext())
      CPPUNIT_ASSERT(graph->isElement(itE->next()));
    delete itE;
    graph->delNode(nodes[i]);
    CPPUNIT_ASSERT(!graph->isElement(nodes[i]));
  }
  CPPUNIT_ASSERT(graph->numberOfNodes() == 0);
  CPPUNIT_ASSERT(graph->numberOfEdges() == 0);
}
//==========================================================
void SuperGraphTest::testClear() {
  build(100, 1000);
  graph->clear();
  CPPUNIT_ASSERT(graph->numberOfNodes() == 0);
  CPPUNIT_ASSERT(graph->numberOfEdges() == 0);
}
//==========================================================
void SuperGraphTest::testOrderEdgeAndSwap() {
  graph->clear();
  vector<node> nodes;
  vector<edge> edges;
  unsigned int NB_NODES = 1000;
  for (unsigned int i=0; i<NB_NODES; ++i)
    nodes.push_back(graph->addNode());
  unsigned int NB_EDGES = NB_NODES - 1;
  for (unsigned int i=0; i<NB_EDGES; ++i)
    edges.push_back(graph->addEdge(nodes[0],nodes[i+1]));
  {
    Iterator<edge> *it = graph->getInOutEdges(nodes[0]);
    unsigned int i = 0;
    while(it->hasNext()) {
      CPPUNIT_ASSERT( it->next() == edges[i]);
      ++i;
    }delete it;
  }
  //change order
  for (unsigned int j=0;j < NB_EDGES; ++j) {
    unsigned int u = rand()%NB_EDGES;
    unsigned int v = rand()%NB_EDGES;
    edge tmp = edges[u];
    edges[u] = edges[v];
    edges[v] = tmp;
  }
  graph->setEdgeOrder(nodes[0],edges);
  {
    Iterator<edge> *it = graph->getInOutEdges(nodes[0]);
    unsigned int i = 0;
    while(it->hasNext()) {
      CPPUNIT_ASSERT(it->next() == edges[i]);
      ++i;
    }delete it;
  }
  //Swap two edge
  for (unsigned int j=0;j < NB_EDGES; ++j) {
    unsigned int u = rand()%NB_EDGES;
    unsigned int v = rand()%NB_EDGES;
    graph->swapEdgeOrder(nodes[0],edges[u],edges[v]);
    edge tmp = edges[u];
    edges[u] = edges[v];
    edges[v] = tmp;
  }
  {
    Iterator<edge> *it = graph->getInOutEdges(nodes[0]);
    unsigned int i = 0;
    while(it->hasNext()) {
      CPPUNIT_ASSERT(it->next() == edges[i]);
      ++i;
    }delete it;
  }
  graph->clear();
}
//==========================================================
void SuperGraphTest::testDeleteSubgraph() {
  graph->clear();
  SuperGraph *g1, *g2, *g3, *g4;
  g1 = graph->addSubGraph();
  g2 = graph->addSubGraph();
  g3 = g2->addSubGraph();
  g4 = g2->addSubGraph();

  CPPUNIT_ASSERT(graph->getFather()==graph);
  CPPUNIT_ASSERT(g1->getFather()==graph);
  CPPUNIT_ASSERT(g2->getFather()==graph);
  CPPUNIT_ASSERT(g3->getFather()==g2);
  CPPUNIT_ASSERT(g4->getFather()==g2);

  CPPUNIT_ASSERT(graph->getRoot()==graph);
  CPPUNIT_ASSERT(g1->getRoot()==graph);
  CPPUNIT_ASSERT(g2->getRoot()==graph);
  CPPUNIT_ASSERT(g3->getRoot()==graph);
  CPPUNIT_ASSERT(g4->getRoot()==graph);

  graph->delSubGraph(g2);
  CPPUNIT_ASSERT(graph->getFather()==graph);
  CPPUNIT_ASSERT(g1->getFather()==graph);
  CPPUNIT_ASSERT(g3->getFather()==graph);
  CPPUNIT_ASSERT(g4->getFather()==graph);

  graph->delSubGraph(g3);
  graph->delSubGraph(g4);
  g2 = g1->addSubGraph();
  g3 = g2->addSubGraph();
  g4 = g3->addSubGraph();
  g1->delAllSubGraphs(g2);
  Iterator<SuperGraph *> *itGr = g1->getSubGraphs();
  CPPUNIT_ASSERT(!itGr->hasNext());
  delete itGr;
  graph->clear();
}
//==========================================================
void SuperGraphTest::testSubgraph() {
  graph->clear();
  SuperGraph *g1, *g2, *g3, *g4;
  g1 = graph->addSubGraph();
  g2 = graph->addSubGraph();
  g3 = g2->addSubGraph();
  g4 = g2->addSubGraph();

  CPPUNIT_ASSERT(graph->getFather()==graph);
  CPPUNIT_ASSERT(g1->getFather()==graph);
  CPPUNIT_ASSERT(g2->getFather()==graph);
  CPPUNIT_ASSERT(g3->getFather()==g2);
  CPPUNIT_ASSERT(g4->getFather()==g2);

  CPPUNIT_ASSERT(graph->getRoot()==graph);
  CPPUNIT_ASSERT(g1->getRoot()==graph);
  CPPUNIT_ASSERT(g2->getRoot()==graph);
  CPPUNIT_ASSERT(g3->getRoot()==graph);
  CPPUNIT_ASSERT(g4->getRoot()==graph);

  Iterator<SuperGraph *> *it= g2->getSubGraphs();
  SuperGraph *a,*b;
  CPPUNIT_ASSERT(it->hasNext());
  a = it->next();
  CPPUNIT_ASSERT(it->hasNext());
  b = it->next();
  CPPUNIT_ASSERT((a==g3 && b==g4) || (a==g4 && b==g3));
  CPPUNIT_ASSERT(!it->hasNext());
  delete it;
  g2->clear();
  it= g2->getSubGraphs();
  CPPUNIT_ASSERT(!it->hasNext());
  delete it;

  g3 = g2->addSubGraph();
  g4 = g2->addSubGraph();  
  node n1 = g3->addNode();
  node n2 = g3->addNode();
  CPPUNIT_ASSERT(g3->isElement(n1) && g3->isElement(n2));
  CPPUNIT_ASSERT(g2->isElement(n1) && graph->isElement(n1));
  CPPUNIT_ASSERT(g2->isElement(n2) && graph->isElement(n2));
  CPPUNIT_ASSERT(!g4->isElement(n1) && !g1->isElement(n1));  
  CPPUNIT_ASSERT(!g4->isElement(n2) && !g1->isElement(n2));  
  
  edge e = g2->addEdge(n1,n2);
  CPPUNIT_ASSERT(!g3->isElement(e) && !g4->isElement(e) && !g1->isElement(e));
  CPPUNIT_ASSERT(g2->isElement(e) && graph->isElement(e) && g2->isElement(n2));
  g2->delNode(n2);
  CPPUNIT_ASSERT(!g2->isElement(n2) && !g2->isElement(e));
  CPPUNIT_ASSERT(graph->isElement(n2) && graph->isElement(e));
  g2->addNode(n2);
  g2->addEdge(e);
  CPPUNIT_ASSERT(g2->isElement(n2) && g2->isElement(e));
  g2->delAllNode(n2);
  CPPUNIT_ASSERT(!g2->isElement(n2) && !g2->isElement(e));
  CPPUNIT_ASSERT(!graph->isElement(n2) && !graph->isElement(e));
  graph->clear();

  g1 = graph->addSubGraph();
  g2 = graph;
  graph = g1;
  testAddDel();
  testClear();
  testOrderEdgeAndSwap();
  graph = g2;
  graph->clear();
}
//==========================================================
void SuperGraphTest::testInheritance() {
  graph->clear();
  SuperGraph *g1, *g2, *g3, *g4;

  g1 = graph->addSubGraph();
  g2 = graph->addSubGraph();
  g3 = g2->addSubGraph();
  g4 = g2->addSubGraph();
  MetricProxy *m = graph->getProperty<MetricProxy>("metric");
  CPPUNIT_ASSERT(graph->existProperty("metric"));
  CPPUNIT_ASSERT(g1->existProperty("metric"));
  CPPUNIT_ASSERT(g2->existProperty("metric"));
  CPPUNIT_ASSERT(g3->existProperty("metric"));
  CPPUNIT_ASSERT(g4->existProperty("metric"));

  graph->delLocalProperty("metric");
  CPPUNIT_ASSERT(!graph->existProperty("metric"));
  CPPUNIT_ASSERT(!g1->existProperty("metric"));
  CPPUNIT_ASSERT(!g2->existProperty("metric"));
  CPPUNIT_ASSERT(!g3->existProperty("metric"));
  CPPUNIT_ASSERT(!g4->existProperty("metric"));


  MetricProxy *m2 = g2->getLocalProperty<MetricProxy>("metric");
  CPPUNIT_ASSERT(!graph->existProperty("metric"));
  CPPUNIT_ASSERT(!g1->existProperty("metric"));
  CPPUNIT_ASSERT(g2->existProperty("metric"));
  CPPUNIT_ASSERT(g3->existProperty("metric"));
  CPPUNIT_ASSERT(g4->existProperty("metric"));

  m = graph->getProperty<MetricProxy>("metric");
  CPPUNIT_ASSERT(graph->getProperty("metric") == m);
  CPPUNIT_ASSERT(g1->getProperty("metric") == m);
  CPPUNIT_ASSERT(g2->getProperty("metric") == m2);
  CPPUNIT_ASSERT(g3->getProperty("metric") == m2);
  CPPUNIT_ASSERT(g4->getProperty("metric") == m2);

  g2->delLocalProperty("metric");
  CPPUNIT_ASSERT(graph->getProperty("metric") == m);
  CPPUNIT_ASSERT(g1->getProperty("metric") == m);
  CPPUNIT_ASSERT(g2->getProperty("metric") == m);
  CPPUNIT_ASSERT(g3->getProperty("metric") == m);
  CPPUNIT_ASSERT(g4->getProperty("metric") == m);

  graph->clear();
}
//==========================================================
void SuperGraphTest::testPropertiesIteration() {
  graph->clear();
  SuperGraph *g1, *g2, *g3, *g4;
  set<string> propList1;
  set<string> propList2;
  propList1.insert("m1");
  propList1.insert("m2");
  propList1.insert("m3");
  propList2.insert("m4");
  propList2.insert("m5");
  propList2.insert("m6");
  g1 = graph->addSubGraph();
  g2 = graph->addSubGraph();
  g3 = g2->addSubGraph();
  g4 = g2->addSubGraph();

  set<string>::const_iterator it;
  for (it=propList1.begin();it!=propList1.end();++it) {
    graph->getProperty<IntProxy>(*it);
    CPPUNIT_ASSERT(g4->existProperty(*it));
  }
  Iterator<string> *itS = graph->getProperties();
  while(itS->hasNext()) {
    CPPUNIT_ASSERT(propList1.find(itS->next())!=propList1.end());
  } delete itS;
  itS = g2->getProperties();
  while(itS->hasNext()) {
    CPPUNIT_ASSERT(propList1.find(itS->next())!=propList1.end());
  } delete itS;
  itS = g1->getLocalProperties();
  while(itS->hasNext()) {
    CPPUNIT_ASSERT(propList1.find(itS->next())==propList1.end());
  } delete itS;
  itS = g1->getLocalProperties();
  while(itS->hasNext()) {
    CPPUNIT_ASSERT(propList1.find(itS->next())==propList1.end());
  } delete itS;
  itS = g4->getInheritedProperties();
  while(itS->hasNext()) {
    CPPUNIT_ASSERT(propList1.find(itS->next())!=propList1.end());
  } delete itS;

  for (it=propList2.begin();it!=propList2.end();++it) {
    g2->getProperty<IntProxy>(*it);
    CPPUNIT_ASSERT(g4->existProperty(*it));
  }

  itS = graph->getProperties();
  while(itS->hasNext()) {
    string str = itS->next();
    CPPUNIT_ASSERT(propList1.find(str)!=propList1.end()  && propList2.find(str)==propList2.end());
  } delete itS;
  itS = g1->getProperties();
  while(itS->hasNext()) {
    string str = itS->next();
    CPPUNIT_ASSERT(propList1.find(str)!=propList1.end()  && propList2.find(str)==propList2.end());
  } delete itS;
  itS = g2->getProperties();
  while(itS->hasNext()) {
    string str = itS->next();
    CPPUNIT_ASSERT(propList1.find(str)!=propList1.end()  || propList2.find(str)!=propList2.end());
  } delete itS;
  itS = g3->getProperties();
  while(itS->hasNext()) {
    string str = itS->next();
    CPPUNIT_ASSERT(propList1.find(str)!=propList1.end()  || propList2.find(str)!=propList2.end());
  } delete itS;
  itS = g4->getProperties();
  while(itS->hasNext()) {
    string str = itS->next();
    CPPUNIT_ASSERT(propList1.find(str)!=propList1.end()  || propList2.find(str)!=propList2.end());
  } delete itS;
  graph->clear();
}
//==========================================================
CppUnit::Test * SuperGraphTest::suite() {
  CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "Tulip SuperGraph function Test Suite" );
  suiteOfTests->addTest( new CppUnit::TestCaller<SuperGraphTest>( "Add/Del operations (Simple Graph)", 
								  &SuperGraphTest::testAddDel) );
  suiteOfTests->addTest( new CppUnit::TestCaller<SuperGraphTest>( "Clean operations (Simple Graph)", 
								  &SuperGraphTest::testClear) );
  suiteOfTests->addTest( new CppUnit::TestCaller<SuperGraphTest>( "Iteration nodes/edges", 
								  &SuperGraphTest::testIterators) );
  suiteOfTests->addTest( new CppUnit::TestCaller<SuperGraphTest>( "Edge ordering and swap(Simple Graph)", 
								  &SuperGraphTest::testOrderEdgeAndSwap) );
  suiteOfTests->addTest( new CppUnit::TestCaller<SuperGraphTest>( "Outdeg, indeg, deg on graph and sub-graph", 
								  &SuperGraphTest::testDegree) );
  suiteOfTests->addTest( new CppUnit::TestCaller<SuperGraphTest>( "Sub Graph operations (add/del/clean)", 
								  &SuperGraphTest::testSubgraph) );
  suiteOfTests->addTest( new CppUnit::TestCaller<SuperGraphTest>( "Test Sub Graph delete", 
								  &SuperGraphTest::testDeleteSubgraph) );
  suiteOfTests->addTest( new CppUnit::TestCaller<SuperGraphTest>( "Inheritance of properties", 
								  &SuperGraphTest::testInheritance) );
  suiteOfTests->addTest( new CppUnit::TestCaller<SuperGraphTest>( "Iteration of properties", 
								  &SuperGraphTest::testPropertiesIteration) );

  return suiteOfTests;
}
//==========================================================
