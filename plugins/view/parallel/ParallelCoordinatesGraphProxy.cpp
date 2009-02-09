#include <tulip/ColorProperty.h>
#include <tulip/BooleanProperty.h>
#include <tulip/SizeProperty.h>
#include <tulip/StringProperty.h>
#include <tulip/ForEach.h>
#include <tulip/IntegerProperty.h>
#include <tulip/DoubleProperty.h>


#include "ParallelCoordinatesGraphProxy.h"

using namespace std;

namespace tlp {

ParallelCoordinatesGraphProxy::ParallelCoordinatesGraphProxy(Graph *g, const ElementType location):
	GraphDecorator(g), dataLocation(location) {
  dataColors = graph_component->getProperty<ColorProperty>("viewColor");
  originalDataColors = new ColorProperty(graph_component);
  *originalDataColors = *(graph_component->getProperty<ColorProperty>("viewColor"));
}

ParallelCoordinatesGraphProxy::~ParallelCoordinatesGraphProxy() {
	Observable::holdObservers();
	*dataColors = *originalDataColors;
	delete originalDataColors;
	originalDataColors = NULL;
	Observable::unholdObservers();
}

unsigned int ParallelCoordinatesGraphProxy::getNumberOfSelectedProperties() const {
	return selectedProperties.size();
}

const vector<string> &ParallelCoordinatesGraphProxy::getSelectedProperties() {
  vector<string> selectedPropertiesTmp;
  vector<string>::iterator it;

  // check if one of the selected properties has not been deleted by an undo operation
  for (it = selectedProperties.begin() ; it != selectedProperties.end() ; ++it) {
	  if (existProperty(*it)) {
		  selectedPropertiesTmp.push_back(*it);
	  }
  }
  selectedProperties = selectedPropertiesTmp;
  return selectedProperties;
}

void ParallelCoordinatesGraphProxy::setSelectedProperties(const vector<string>& properties) {
  selectedProperties = properties;
}

void ParallelCoordinatesGraphProxy::removePropertyFromSelection(const std::string &propertyName) {
	vector<string> selectedPropertiesCopy(selectedProperties);
	vector<string>::iterator it;
	selectedProperties.clear();
	for (it = selectedPropertiesCopy.begin() ; it != selectedPropertiesCopy.end() ; ++it) {
		if (*it != propertyName) {
			selectedProperties.push_back(*it);
		}
	}
}

ElementType ParallelCoordinatesGraphProxy::getDataLocation() const {
  return dataLocation;
}

unsigned int ParallelCoordinatesGraphProxy::getDataCount() const {
  if (getDataLocation() == NODE) {
    return numberOfNodes();
  } else {
    return numberOfEdges();
  }
}

Color ParallelCoordinatesGraphProxy::getDataColor(const unsigned int dataId) {
	return getPropertyValueForData<ColorProperty, ColorType>("viewColor", dataId);
}

string ParallelCoordinatesGraphProxy::getDataTexture(const unsigned int dataId) {
	return getPropertyValueForData<StringProperty, StringType>("viewTexture", dataId);
}

bool ParallelCoordinatesGraphProxy::isDataSelected(const unsigned int dataId) {
	return getPropertyValueForData<BooleanProperty, BooleanType>("viewSelection", dataId);
}

void ParallelCoordinatesGraphProxy::setDataSelected(const unsigned int dataId, const bool dataSelected) {
	setPropertyValueForData<BooleanProperty, BooleanType>("viewSelection", dataId, dataSelected);
}

Size ParallelCoordinatesGraphProxy::getDataViewSize(const unsigned int dataId) {
	return getPropertyValueForData<SizeProperty, SizeType>("viewSize", dataId);
}

string ParallelCoordinatesGraphProxy::getDataLabel(const unsigned int dataId) {
	return getPropertyValueForData<StringProperty, StringType>("viewLabel", dataId);
}

void ParallelCoordinatesGraphProxy::resetSelection() {
	setPropertyValueForAllData<BooleanProperty, BooleanType>("viewSelection", false);
}

void ParallelCoordinatesGraphProxy::deleteData(const unsigned int dataId) {
	if (getDataLocation() == NODE) {
		delNode(node(dataId));
	} else {
		delEdge(edge(dataId));
	}
}

Iterator<unsigned int> *ParallelCoordinatesGraphProxy::getDataIterator() {
	if (getDataLocation() == NODE) {
		return new ParallelCoordinatesDataIterator<node>(getNodes());
	} else {
		return new ParallelCoordinatesDataIterator<edge>(getEdges());
	}
}

string ParallelCoordinatesGraphProxy::getToolTipTextforData(const unsigned int dataId) {
	string ttipText;
	if (getDataLocation() == NODE) {
		ttipText = "node ";
	} else {
		ttipText = "edge ";
	}
	ttipText += getStringFromNumber(dataId);
	string label = getDataLabel(dataId);
	if (!label.empty()) {
		ttipText = label + " (" + ttipText +")";
	}
	return ttipText;
}

void ParallelCoordinatesGraphProxy::addOrRemoveEltToHighlight(const unsigned int eltId) {
	if (isDataHighlighted(eltId)) {
		highlightedElts.erase(eltId);
	} else {
		highlightedElts.insert(eltId);
	}
}

void ParallelCoordinatesGraphProxy::unsetHighlightedElts() {
	highlightedElts.clear();

}

void ParallelCoordinatesGraphProxy::resetHighlightedElts(const set<unsigned int> &highlightedData) {
	highlightedElts.clear();
	set<unsigned int>::iterator it;
	for (it = highlightedData.begin() ; it != highlightedData.end() ; ++it) {
		addOrRemoveEltToHighlight(*it);
	}
}

bool ParallelCoordinatesGraphProxy::isDataHighlighted(const unsigned int dataId) {
	return highlightedElts.find(dataId) != highlightedElts.end();
}

bool ParallelCoordinatesGraphProxy::highlightedEltsSet() const {
	return highlightedElts.size() != 0;
}

void ParallelCoordinatesGraphProxy::selectHighlightedElements() {
	set<unsigned int>::iterator it;
	for (it = highlightedElts.begin() ; it != highlightedElts.end() ; ++it) {
		setDataSelected(*it, true);
	}
}

void ParallelCoordinatesGraphProxy::colorDataAccordingToHighlightedElts() {

	if (originalDataColors == NULL) {
		return;
	}

	// If new colors have been set for the graph elements, backup the change to restore the correct ones
	// when unhighlighting
	Iterator<unsigned int> *dataIt = getDataIterator();
	while (dataIt->hasNext()) {
		unsigned int dataId = dataIt->next();
		Color currentColor = getPropertyValueForData<ColorProperty, ColorType>("viewColor", dataId);
		Color originalColor;
		if (getDataLocation() == NODE) {
			originalColor = originalDataColors->getNodeValue(node(dataId));
		} else {
			originalColor = originalDataColors->getEdgeValue(edge(dataId));
		}
		if (currentColor != originalColor && currentColor != COLOR_NON_HIGHLIGHT) {
			if (getDataLocation() == NODE) {
				originalDataColors->setNodeValue(node(dataId), currentColor);
			} else {
				originalDataColors->setEdgeValue(edge(dataId), currentColor);
			}
		}
	}
	delete dataIt;

	if (!highlightedEltsSet()) {
		*(graph_component->getProperty<ColorProperty>("viewColor")) = *originalDataColors;
		return;
	}

	dataIt = getDataIterator();
	while (dataIt->hasNext()) {
		unsigned int dataId = dataIt->next();
		if (isDataHighlighted(dataId)) {
			setPropertyValueForData<ColorProperty, ColorType>("viewColor", dataId, getOriginalDataColor(dataId));
		} else {
			setPropertyValueForData<ColorProperty, ColorType>("viewColor", dataId, COLOR_NON_HIGHLIGHT);
		}
	}
	delete dataIt;
}

Color ParallelCoordinatesGraphProxy::getOriginalDataColor(const unsigned int dataId) {
	if (getDataLocation() == NODE) {
		return originalDataColors->getNodeValue(node(dataId));
	} else {
		return originalDataColors->getEdgeValue(edge(dataId));
	}
}

bool ParallelCoordinatesGraphProxy::unhighlightedEltsColorOk() {
	bool ret = true;
	Iterator<unsigned int> *dataIt = getDataIterator();
	while (dataIt->hasNext()) {
		unsigned int dataId = dataIt->next();
		if (highlightedElts.find(dataId) == highlightedElts.end()) {
			if (getDataColor(dataId) != COLOR_NON_HIGHLIGHT) {
				ret = false;
				break;
			}
		}
	}
	delete dataIt;

	return ret;
}

void ParallelCoordinatesGraphProxy::removeHighlightedElement(const unsigned int dataId) {
	highlightedElts.erase(dataId);
}

}
