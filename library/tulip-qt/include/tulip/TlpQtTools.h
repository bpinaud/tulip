//-*-c++-*-
#ifndef _TLPQTTOOLS_H
#define _TLPQTTOOLS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "tulip/Reflect.h"
#include "tulip/Graph.h"
#include "tulip/GlGraphWidget.h"

class QWidget;

namespace tlp {

  /**
   * Open a QT Dialog Box to fill-up an user DataSet according to a required
   * StructDef
   * inDef		: Input-variable informations
   * outSet		: Output (filled) set
   * [inSet		: Input (optional) set, containing some default values]
   */

  TLP_QT_SCOPE bool openDataSetDialog	(DataSet & outSet,
					 const StructDef *sysDef,
					 StructDef *inDef,
					 const DataSet *inSet	= NULL,
					 const char *inName = NULL,
					 Graph *inG = NULL);

  /**
   * Create a main layout in GlGraphWidget and display the graph on it 
   */

  TLP_QT_SCOPE void openGraphOnGlGraphWidget(Graph *graph,DataSet *dataSet,GlGraphWidget *glGraphWidget);

}

#endif


