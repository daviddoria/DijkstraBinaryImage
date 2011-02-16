#include "DijkstraBinaryImage.h"

// ITK
#include "itkImageFileWriter.h"

typedef itk::Image<unsigned char, 2> ImageType;

void CreateCircleImage(ImageType::Pointer image);

int main (int argc, char *argv[])
{
  ImageType::Pointer image = ImageType::New();
  CreateCircleImage(image);

  DijkstraBinaryImage DistanceFilter;
  DistanceFilter.SetImage(image);
  itk::Index<2> startPoint;
  startPoint[0] = 30;
  startPoint[1] = 49;

  itk::Index<2> endPoint;
  endPoint[0] = 69;
  endPoint[1] = 44;

  if(!image->GetPixel(startPoint))
    {
    std::cerr << "start Pixel is not white in image!" << std::endl;
    return EXIT_FAILURE;
    }

  if(!image->GetPixel(endPoint))
    {
    std::cerr << "end Pixel is not white in image!" << std::endl;
    return EXIT_FAILURE;
    }

  std::vector<itk::Index<2> > path = DistanceFilter.ComputeShortestPath(startPoint, endPoint);

  for(unsigned int i = 0; i < path.size(); i++)
    {
    image->SetPixel(path[i], 100);
    }

  typedef  itk::ImageFileWriter< ImageType  > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName("result.png");
  writer->SetInput(image);
  writer->Update();

  return EXIT_SUCCESS;
}


void CreateCircleImage(ImageType::Pointer image)
{
  itk::Size<2> size;
  size.Fill(100);

  itk::Index<2> start;
  start.Fill(0);

  itk::ImageRegion<2> region(start, size);

  image->SetRegions(region);
  image->Allocate();
  image->FillBuffer(0);

  for(unsigned int i = 0; i < 1000; i++)
    {
    int x = 20 * cos(i) + 50;
    int y = 20 * sin(i) + 50;
    itk::Index<2> pixel;
    pixel[0] = x;
    pixel[1] = y;
    image->SetPixel(pixel, 255);
    }

  typedef  itk::ImageFileWriter< ImageType  > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName("circle.png");
  writer->SetInput(image);
  writer->Update();
}