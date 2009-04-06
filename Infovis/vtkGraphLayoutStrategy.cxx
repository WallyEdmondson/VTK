/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGraphLayoutStrategy.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*-------------------------------------------------------------------------
  Copyright 2008 Sandia Corporation.
  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
  the U.S. Government retains certain rights in this software.
-------------------------------------------------------------------------*/
#include "vtkGraphLayoutStrategy.h"

#include "vtkGraph.h"

vtkCxxRevisionMacro(vtkGraphLayoutStrategy, "1.9");

void vtkGraphLayoutStrategy::SetGraph(vtkGraph *graph)
{
  // This method is a cut and paste of vtkCxxSetObjectMacro
  // except for the call to Initialize in the middle :)
  if (graph != this->Graph)
    {
    vtkGraph *tmp = this->Graph;
    this->Graph = graph;
    if (this->Graph != NULL)
      {
      this->Graph->Register(this);
      this->Initialize();
      }
    if (tmp != NULL)
      {
      tmp->UnRegister(this);
      }
    this->Modified();
    }
}

vtkGraphLayoutStrategy::vtkGraphLayoutStrategy()
{
  this->Graph = NULL;
  this->EdgeWeightField = NULL;
}

vtkGraphLayoutStrategy::~vtkGraphLayoutStrategy()
{
  // Unregister vtk objects that were passed in
  this->SetGraph(NULL);
  this->SetEdgeWeightField(NULL);
}

void vtkGraphLayoutStrategy::SetEdgeWeightField(const char* weights)
{
  // This method is a cut and paste of vtkSetStringMacro
  // except for the call to Initialize at the end :)
  if ( this->EdgeWeightField == NULL && weights == NULL) { return;}
  if ( this->EdgeWeightField && weights && (!strcmp(this->EdgeWeightField,weights))) { return;}
  if (this->EdgeWeightField) { delete [] this->EdgeWeightField; }
  if (weights)
    {
    size_t n = strlen(weights) + 1;
    char *cp1 =  new char[n];
    const char *cp2 = (weights);
    this->EdgeWeightField = cp1;
    do { *cp1++ = *cp2++; } while ( --n );
    }
   else
    {
    this->EdgeWeightField = NULL;
    }
  
  this->Modified();

  if(this->Graph)
    {
    this->Initialize();
    }
}

void vtkGraphLayoutStrategy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Graph: " << (this->Graph ? "" : "(none)") << endl;
  if (this->Graph)
    {
    this->Graph->PrintSelf(os, indent.GetNextIndent());
    }
  os << indent << "EdgeWeightField: " << (this->EdgeWeightField ? this->EdgeWeightField : "(none)") << endl;
}
