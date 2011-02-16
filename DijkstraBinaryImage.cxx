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
  this->VisitedImage->SetRegions(this->Image->GetLargestPossibleRegion());
  this->VisitedImage->Allocate();
  this->VisitedImage->FillBuffer(false);

  this->DistanceImage->SetRegions(this->Image->GetLargestPossibleRegion());
  this->DistanceImage->Allocate();
  this->DistanceImage->FillBuffer(itk::NumericTraits< int >::max());
}

std::vector<itk::Index<2> > DijkstraBinaryImage::ComputeShortestPath(itk::Index<2> startPoint, itk::Index<2> endPoint)
{
  std::vector<itk::Index<2> > path;

  // Compute the DistanceImage
  bool reached = this->Dijkstra(startPoint, endPoint);
  if(!reached)
    {
    std::cout << "Not reached!" << std::endl;
    return path;
    }

  // Move greedily from the endPoint to the StartPoint

  std::vector<itk::Offset<2> > neighborhood = CreateNeighborhood();

  itk::Index<2> currentPixel = endPoint;
  path.push_back(currentPixel);

  while(currentPixel != startPoint)
    {
    itk::Index<2> minimumPixel;
    int minimumDistance = itk::NumericTraits<int>::max();
    for(unsigned int i = 0; i < neighborhood.size(); ++i)
      {
      itk::Index<2> nextPixel = currentPixel + neighborhood[i];
      if(!this->Image->GetLargestPossibleRegion().IsInside(nextPixel))
        {
        continue; // out of bounds
        }
      if(!this->Image->GetPixel(nextPixel))
        {
        continue; // not a non-zero pixel
        }

      if(this->DistanceImage->GetPixel(nextPixel) < minimumDistance)
        {
        minimumPixel = nextPixel;
        minimumDistance = this->DistanceImage->GetPixel(nextPixel);
        }
      }//end neighborhood loop
    currentPixel = minimumPixel;
    path.push_back(currentPixel);
    }//end while

  return path;
}

bool DijkstraBinaryImage::Dijkstra(itk::Index<2> startPoint, itk::Index<2> endPoint)
{
  // This function accepts the endPoint simply to determine whether or not it was reached (the return value)
  bool reached = false;
  std::queue< itk::Index<2> > q;

  q.push(startPoint);
  this->DistanceImage->SetPixel(startPoint, 0);

  std::vector<itk::Offset<2> > neighborhood = CreateNeighborhood();

  while(!q.empty())
    {
    itk::Index<2> pixel = q.front();
    q.pop();

    int currentDistance = this->DistanceImage->GetPixel(pixel);

    if(this->VisitedImage->GetPixel(pixel))
      {
      continue;
      }

    for(unsigned int i = 0; i < neighborhood.size(); ++i)
      {
      itk::Index<2> nextPixel = pixel + neighborhood[i];
      if(nextPixel == endPoint)
        {
        reached = true;
        }
      if(!this->Image->GetLargestPossibleRegion().IsInside(nextPixel))
        {
        continue; // out of bounds
        }
      if(!this->Image->GetPixel(nextPixel))
        {
        continue; // not a non-zero pixel
        }
      q.push(nextPixel);
      if(currentDistance + 1 < this->DistanceImage->GetPixel(nextPixel))
        {
        this->DistanceImage->SetPixel(nextPixel, currentDistance + 1);
        }
      }//end neighborhood loop

    this->VisitedImage->SetPixel(pixel, true);

    }//end while


  typedef  itk::ImageFileWriter< itk::Image<int,2>  > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName("distance.mhd");
  writer->SetInput(this->DistanceImage);
  writer->Update();


  return reached;
}

std::vector<itk::Offset<2> > DijkstraBinaryImage::CreateNeighborhood()
{
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