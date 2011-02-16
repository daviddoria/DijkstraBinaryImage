cmake_minimum_required(VERSION 2.6)

PROJECT(DijkstraBinaryImage)

FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

ADD_EXECUTABLE(DijkstraBinaryImage Test.cxx DijkstraBinaryImage.cxx)
TARGET_LINK_LIBRARIES(DijkstraBinaryImage
vtkHybrid vtkIO
ITKCommon ITKBasicFilters ITKIO)