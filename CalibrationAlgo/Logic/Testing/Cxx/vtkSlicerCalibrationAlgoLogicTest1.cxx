/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer

=========================================================================auto=*/

// Logic includes
#include "vtkSlicerCalibrationAlgoLogic.h"

// MRML includes
#include "vtkMRMLScene.h"

// VTK includes
#include <vtkMatrix4x4.h>

// STL includes
#include <fstream>

//-----------------------------------------------------------------------------
int vtkSlicerCalibrationAlgoLogicTest1(int argc, char* argv [])
{
  if (argc < 2)
  {
    std::cerr << "Missing transform file name." << std::endl;
    return EXIT_FAILURE;
  }

  vtkMRMLScene* scene = vtkMRMLScene::New();

  vtkSlicerCalibrationAlgoLogic* moduleLogic = vtkSlicerCalibrationAlgoLogic::New();
  moduleLogic->SetMRMLScene(scene);
  if (moduleLogic->GetMRMLScene() != scene)
  {
    std::cerr << "A MRML Scene must be set to go further." << std::endl;
    return EXIT_FAILURE;
  }

  std::ifstream input(argv[1]);
  if (!input.is_open())
  {
    std::cerr << "Unable to open input file: " << argv[1] << std::endl;
    return EXIT_FAILURE;
  }

  std::string line;
  while (std::getline(input, line))
  {
    if (line.empty())
    {
      // Point line pairs are done, move to matrix
      break;
    }
    std::stringstream ss;
    ss << line;
    // data is in format point lineOrigin lineDirection
    double point[3];
    double origin[3];
    double direction[3];
    ss >> point[0] >> point[1] >> point[2];
    ss >> origin[0] >> origin[1] >> origin[2];
    ss >> direction[0] >> direction[1] >> direction[2];
    moduleLogic->AddPointAndLine(point, origin, direction);
  }

  if (input.eof())
  {
    std::cerr << "Missing matrix in data input file." << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkMatrix4x4> goldStandard;
  int i = 0;
  while (std::getline(input, line))
  {
    std::stringstream ss;
    ss << line;
    float row[4];
    ss >> row[0] >> row[1] >> row[2] >> row[3];
    goldStandard->SetElement(i, 0, row[0]);
    goldStandard->SetElement(i, 1, row[1]);
    goldStandard->SetElement(i, 2, row[2]);
    goldStandard->SetElement(i, 3, row[3]);
    i++;
  }

  vtkMatrix4x4* calculatedResult;
  calculatedResult = moduleLogic->CalculateRegistration();

  if (calculatedResult != goldStandard)
  {
    std::cerr << "Matrices do not match." << std::endl;
    return EXIT_FAILURE;
  }

  calculatedResult->Delete();
  moduleLogic->Delete();
  scene->Delete();

  return EXIT_SUCCESS;
}