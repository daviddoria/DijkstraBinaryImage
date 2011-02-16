#ifndef BFSDistanceBinary_h
#define BFSDistanceBinary_h

// ITK
#include "itkImage.h"

class DijkstraBinaryImage
{
public:
  DijkstraBinaryImage();

  std::vector<itk::Index<2> > ComputeShortestPath(itk::Index<2> startPoint, itk::Index<2> endPoint);

  void SetImage(itk::Image<unsigned char, 2>::Pointer image);

private:
  bool Dijkstra(itk::Index<2> startPoint, itk::Index<2> endPoint);

  itk::Image<unsigned char, 2>::Pointer Image;
  itk::Image<bool, 2>::Pointer VisitedImage;
  itk::Image<int, 2>::Pointer DistanceImage;

  // Helpers
  std::vector<itk::Offset<2> > CreateNeighborhood();
};

#endif