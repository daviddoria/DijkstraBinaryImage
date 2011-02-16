#include "DijkstraBinaryImage.h"

// ITK
#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkNumericTraits.h"

// STL
#include <vector>
#include <queue>

DijkstraBinaryImage::DijkstraBinaryImage()
{
  this->Image = itk::Image<unsigned char, 2>::New();
  this->VisitedImage = itk::Image<bool, 2>::New();
  this->DistanceImage = itk::Image<int, 2>::New();
}

void DijkstraBinaryImage::SetImage(itk::Image<unsigned char, 2>::Pointer image)
{
  this->Image->Graft(image);
}

std::vector<itk::Index<2> > DijkstraBinaryImage::ComputeShortestPath(itk::Index<2> startPoint, itk::Index<2> endPoint)
{
  // This function first computes the distance image (using Dijkstra()), then finds the shortest path by starting at the
  // endPoint and moving to neighbors with the smallest value.

  // The list of pixels in the path
  std::vector<itk::Index<2> > path;

  // Compute the DistanceImage
  bool reached = this->Dijkstra(startPoint, endPoint);

  // If the endPoint was not reached during the BFS, there is no valid path between startPoint and endPoint
  if(!reached)
    {
    std::cout << "Not reached!" << std::endl;
    return path;
    }

  // Initialize the traversal
  std::vector<itk::Offset<2> > neighborhood = CreateNeighborhood();

  itk::Index<2> currentPixel = endPoint;
  path.push_back(currentPixel);

  // Move greedily from the endPoint to the StartPoint

  while(currentPixel != startPoint)
    {
    // Initialize
    itk::Index<2> minimumPixel;
    int minimumDistance = itk::NumericTraits<int>::max();

    // Find the neighbor with the smallest value
    for(unsigned int i = 0; i < neighborhood.size(); ++i)
      {
      itk::Index<2> nextPixel = currentPixel + neighborhood[i];

      // Don't consider moving to pixels outside the image
      if(!this->Image->GetLargestPossibleRegion().IsInside(nextPixel))
        {
        continue;
        }

      // Don't consider moving to pixels which are not non-zero
      if(!this->Image->GetPixel(nextPixel))
        {
        continue; // not a non-zero pixel
        }

      // Check if this is the current smallest neighbor
      if(this->DistanceImage->GetPixel(nextPixel) < minimumDistance)
        {
        minimumPixel = nextPixel;
        minimumDistance = this->DistanceImage->GetPixel(nextPixel);
        }
      }//end neighborhood loop

    // Move to the neighbor with the smallest value
    currentPixel = minimumPixel;
    path.push_back(currentPixel);
    }//end while

  return path;
}

bool DijkstraBinaryImage::Dijkstra(itk::Index<2> startPoint, itk::Index<2> endPoint)
{
  // Return: Was the end point reached?
  // Description: This function performs a breadth first search while tracking the shortest distance to every non-zero pixel
  // in the image reachable through an 8-nearest neighbor connectivity search.
  // The parameter endPoint is used simply to determine whether or not the end point was reached (the return value).

  // Initialize
  this->VisitedImage->SetRegions(this->Image->GetLargestPossibleRegion());
  this->VisitedImage->Allocate();
  this->VisitedImage->FillBuffer(false);

  this->DistanceImage->SetRegions(this->Image->GetLargestPossibleRegion());
  this->DistanceImage->Allocate();
  this->DistanceImage->FillBuffer(itk::NumericTraits< int >::max());

  bool reached = false;
  std::queue< itk::Index<2> > q;

  q.push(startPoint);
  this->DistanceImage->SetPixel(startPoint, 0);

  std::vector<itk::Offset<2> > neighborhood = CreateNeighborhood();

  while(!q.empty())
    {
    // Get the next pixel in the queue and remove it frmo the queue
    itk::Index<2> pixel = q.front();
    q.pop();

    // If we have already visited this pixel, skip it.
    if(this->VisitedImage->GetPixel(pixel))
      {
      continue;
      }

    // Get the shortest distance from the startPoint to the current pixel
    int currentDistance = this->DistanceImage->GetPixel(pixel);

    // Loop through the neighbors
    for(unsigned int i = 0; i < neighborhood.size(); ++i)
      {
      itk::Index<2> nextPixel = pixel + neighborhood[i];
      if(nextPixel == endPoint)
        {
        reached = true;
        }

      // If the neighbor is out of bounds, skip it
      if(!this->Image->GetLargestPossibleRegion().IsInside(nextPixel))
        {
        continue;
        }

      // If the neighbor is not a non-zero pixel, skip it
      if(!this->Image->GetPixel(nextPixel))
        {
        continue; // not a non-zero pixel
        }

      // Add the neighbor to the queue
      q.push(nextPixel);

      // Update the neighbor's distance if it is currently the shortest path (the Dijkstra idea)
      if(currentDistance + 1 < this->DistanceImage->GetPixel(nextPixel))
        {
        this->DistanceImage->SetPixel(nextPixel, currentDistance + 1);
        }
      }//end neighborhood loop

    // Mark the current pixel as visited
    this->VisitedImage->SetPixel(pixel, true);

    }//end while

  // Write the distance image for inspection.
  typedef  itk::ImageFileWriter< itk::Image<int,2>  > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName("distance.mhd");
  writer->SetInput(this->DistanceImage);
  writer->Update();

  return reached;
}

std::vector<itk::Offset<2> > DijkstraBinaryImage::CreateNeighborhood()
{
  // Create a list of offsets of the 8-connected neighbors
  std::vector<itk::Offset<2> > neighbors;

  for(int i = -1; i <= 1; i++)
    {
    for(int j = -1; j <= 1; j++)
      {
      if(!(i==0 && j==0))
        {
        itk::Offset<2> offset = {{i,j}};
        neighbors.push_back(offset);
        } // end if
      } // end j loop
    } // end i loop
  return neighbors;
}