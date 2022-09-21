/**
 * @file trackbase/TrkrHitTruthAssocv1.cc
 * @author R. Boucher
 * @date August 2022 
 * @brief Implementation of alignmentTransformationContainer
 */

#include "alignmentTransformationContainer.h"

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <algorithm>
#include <ostream>  
 
bool alignmentTransformationContainer::use_alignment = false;

void alignmentTransformationContainer::Reset()
{ transformVec.clear(); }

void alignmentTransformationContainer::identify(std::ostream &os)
{

  os << "-----alignmentTransformationContainer-----" << std::endl;
  for(unsigned int i=0; i< transformVec.size(); ++i)
    {
      auto& layerVec = transformVec[i];
      if(layerVec.size() == 0) continue;

      os << " Layer: "  << i << std::endl; 

      for(unsigned int il=0; il< layerVec.size(); ++il)
	{
	  os << " Sensor Id: "  << il+1
	     << " Transform: " << layerVec[il].matrix()
	     << std::endl;
	}
    }
  os << "------------------------------" << std::endl;
  
  return;
}

void alignmentTransformationContainer::addTransform(const Acts::GeometryIdentifier id, Acts::Transform3 transform)
{
  unsigned int sphlayer = getsphlayer(id);
  unsigned int sensor = id.sensitive() - 1;  // Acts sensor numbering starts at 1

  // We are filling a super-vector of layer-vectors
  // first check that the layer-vector for sphlayer is present

  if(sphlayer < transformVec.size())
    {
      // There will be a layer-vector, it may be empty
      auto& layerVec = transformVec[sphlayer]; 

      if(sensor < layerVec.size())
	{
	  layerVec[sensor] = transform;
	}     
      else if (sensor == layerVec.size())
	{
	  layerVec.push_back(transform);
	}
      else
	{
	  layerVec.resize(sensor+1, transform);
	}
    }
  else if(sphlayer == transformVec.size())
      {
	// add new layer-vector to transformVec and put transform in it
	std::vector<Acts::Transform3> newVec;
	newVec.resize(sensor + 1, transform);
	transformVec.push_back(newVec);
      }
    else
      {
	std::vector<Acts::Transform3> newVec;
	newVec.resize(sensor + 1, transform);
	transformVec.resize(sphlayer+1, newVec);
      }
}

/*
void alignmentTransformationContainer::removeTransform(const Acts::GeometryIdentifier id)
{
  // in the fixed length vector of vectors scheme, removing a transform makes no sense

  unsigned int sphlayer = getsphlayer(id);
  unsigned int sensor = id.sensitive() - 1;  // Acts sensor numbering starts at 1

  auto& layerVec = transformVec[sphlayer];
  
  auto transform = layerVec[sensor];
  if(layerVec.size() > sensor)
    {
      return;
    }
}  
*/

Acts::Transform3& alignmentTransformationContainer::getTransform(const Acts::GeometryIdentifier id)
{
  unsigned int sphlayer = getsphlayer(id);
  unsigned int sensor = id.sensitive() - 1;  // Acts sensor numbering starts at 1

  auto& layerVec = transformVec[sphlayer];
  if(layerVec.size() > sensor)
    {
      return layerVec[sensor];
    }
  
  std::cout << "Unable to find Acts Id: "<< id<<  " in alignmentTransformationContainer" << std::endl;
  exit(1); 
}

const std::vector<std::vector<Acts::Transform3>>& alignmentTransformationContainer::getMap()
{
  return transformVec;
}

unsigned int alignmentTransformationContainer::getsphlayer(Acts::GeometryIdentifier id)
{
  unsigned int layer = id.layer(); 
  unsigned int volume = id.volume(); 
  unsigned int sphlayer = base_layer_map.find(volume)->second + layer / 2 -1;
  
  return sphlayer;
}

void alignmentTransformationContainer::set(){}
