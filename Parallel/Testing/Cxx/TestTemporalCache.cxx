/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestTemporalCache.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkActor.h"
#include "vtkCommand.h"
#include "vtkCompositeDataPipeline.h"
#include "vtkContourFilter.h"
#include "vtkInformation.h"
#include "vtkMultiGroupDataGeometryFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkTemporalDataSet.h"
#include "vtkTemporalDataSetCache.h"
#include "vtkTemporalFractal.h"
#include "vtkTemporalInterpolator.h"
#include "vtkThreshold.h"

class ExecuteCallback
  : public vtkCommand
{
public:
  static ExecuteCallback *New()
  { return new ExecuteCallback; }
  
  virtual void Execute(vtkObject *caller, unsigned long, void*)
  { 
    // count the number of time steps requested
    vtkTemporalFractal *frac = vtkTemporalFractal::SafeDownCast(caller);
    this->Count += frac->GetExecutive()->GetOutputInformation(0)
      ->Length(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS());
  }

  unsigned int Count;
};

//-------------------------------------------------------------------------
int main(int , char *[])
{
  // we have to use a compsite pipeline
  vtkCompositeDataPipeline* prototype = vtkCompositeDataPipeline::New();
  vtkAlgorithm::SetDefaultExecutivePrototype(prototype);
  prototype->Delete();

  // create temporal fractals
  vtkSmartPointer<vtkTemporalFractal> fractal = 
    vtkSmartPointer<vtkTemporalFractal>::New();
  fractal->SetMaximumLevel(2);
  fractal->DiscreteTimeStepsOn();
  fractal->GenerateRectilinearGridsOn();

  ExecuteCallback *executecb =ExecuteCallback::New();
  executecb->Count = 0;
  fractal->AddObserver(vtkCommand::StartEvent,executecb);
  executecb->Delete();

  // cache the data to prevent regenerating some of it
  vtkSmartPointer<vtkTemporalDataSetCache> cache = 
    vtkSmartPointer<vtkTemporalDataSetCache>::New();
  cache->SetInputConnection(fractal->GetOutputPort());
  cache->SetCacheSize(2);

  // interpolate if needed
  vtkSmartPointer<vtkTemporalInterpolator> interp = 
    vtkSmartPointer<vtkTemporalInterpolator>::New();
  //interp->SetInputConnection(fractal->GetOutputPort());
  interp->SetInputConnection(cache->GetOutputPort());
  
  // cache the data coming out of the interpolator
  vtkSmartPointer<vtkTemporalDataSetCache> cache2 = 
    vtkSmartPointer<vtkTemporalDataSetCache>::New();
  cache2->SetInputConnection(interp->GetOutputPort());
  cache2->SetCacheSize(11);

  vtkSmartPointer<vtkThreshold> contour = 
    vtkSmartPointer<vtkThreshold>::New();
  //contour->SetInputConnection(interp->GetOutputPort());
  contour->SetInputConnection(cache2->GetOutputPort());
  contour->ThresholdByUpper(0.5);

  vtkSmartPointer<vtkMultiGroupDataGeometryFilter> geom = 
    vtkSmartPointer<vtkMultiGroupDataGeometryFilter>::New();
  geom->SetInputConnection(contour->GetOutputPort());

  // map them
  vtkSmartPointer<vtkPolyDataMapper> mapper = 
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(geom->GetOutputPort());
  
  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  vtkSmartPointer<vtkRenderer> renderer = 
    vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renWin = 
    vtkSmartPointer<vtkRenderWindow>::New();
  vtkSmartPointer<vtkRenderWindowInteractor> iren = 
    vtkSmartPointer<vtkRenderWindowInteractor>::New();

  renderer->AddActor( actor );
  renderer->SetBackground(0.5, 0.5, 0.5);

  renWin->AddRenderer( renderer );
  renWin->SetSize( 300, 300 ); 
  iren->SetRenderWindow( renWin );
  renWin->Render();

  // ask for some specific data points
  vtkStreamingDemandDrivenPipeline *sdd = 
    vtkStreamingDemandDrivenPipeline::SafeDownCast(geom->GetExecutive());
  double times[1];
  times[0] = 0;
  int i;
  int j;
  for (j = 0; j < 5; ++j)
    {
    for (i = 0; i < 11; ++i)
      {
      times[0] = i/2.0;
      sdd->SetUpdateTimeSteps(0, times, 1);
      mapper->Modified();
      renderer->ResetCameraClippingRange();
      renWin->Render();
      }
    }

  vtkAlgorithm::SetDefaultExecutivePrototype(0);

  if (executecb->Count == 8)
    {
    return 0;
    }
  return 1;
}
