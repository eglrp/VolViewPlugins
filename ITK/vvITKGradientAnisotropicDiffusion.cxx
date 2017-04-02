/*=========================================================================

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/VolViewCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/* perform smoothing using an anisotropic diffusion filter */

#include "vvITKFilterModuleWithCasting.h"

#include "itkGradientAnisotropicDiffusionImageFilter.h"



template <class InputPixelType>
class GradientAnisotropicDiffusionRunner
  {
  public:
      typedef   float       InternalPixelType;
      typedef   itk::Image< InternalPixelType, 3 > InternalImageType; 

      typedef   itk::GradientAnisotropicDiffusionImageFilter< 
                                    InternalImageType,  
                                    InternalImageType >   FilterType;
     
      typedef  InputPixelType                            OutputPixelType;                                          

      typedef  VolView::PlugIn::FilterModuleWithCasting< InputPixelType, 
                                                         FilterType,
                                                         OutputPixelType > ModuleType;
  public:
    GradientAnisotropicDiffusionRunner() {}
    void Execute( vtkVVPluginInfo *info, vtkVVProcessDataStruct *pds )
    {
      const unsigned int numberOfIterations = atoi( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ) );
      const float        timeStep           = atof( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ) );
      const float        conductance        = atof( info->GetGUIProperty(info, 2, VVP_GUI_VALUE ) );

      ModuleType  module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Smoothing with Gradient Anisotropic Diffusion...");
      // Set the parameters on it
      module.GetFilter()->SetNumberOfIterations(     numberOfIterations );
      module.GetFilter()->SetTimeStep(               timeStep           );
      module.GetFilter()->SetConductanceParameter(   conductance        );
      // Execute the filter
      module.ProcessData( pds  );
    }
  };




static int ProcessData(void *inf, vtkVVProcessDataStruct *pds)
{

  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  try 
  {
  switch( info->InputVolumeScalarType )
    {
    case VTK_CHAR:
      {
      GradientAnisotropicDiffusionRunner<signed char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_CHAR:
      {
      GradientAnisotropicDiffusionRunner<unsigned char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_SHORT:
      {
      GradientAnisotropicDiffusionRunner<signed short> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_SHORT:
      {
      GradientAnisotropicDiffusionRunner<unsigned short> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_INT:
      {
      GradientAnisotropicDiffusionRunner<signed int> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_INT:
      {
      GradientAnisotropicDiffusionRunner<unsigned int> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_LONG:
      {
      GradientAnisotropicDiffusionRunner<signed long> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_LONG:
      {
      GradientAnisotropicDiffusionRunner<unsigned long> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_FLOAT:
      {
      GradientAnisotropicDiffusionRunner<float> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_DOUBLE:
      {
      GradientAnisotropicDiffusionRunner<double> runner;
      runner.Execute( info, pds );
      break; 
      }
    }

  }
  catch( itk::ExceptionObject & except )
  {
    info->SetProperty( info, VVP_ERROR, except.what() ); 
    return -1;
  }
  return 0;
}


static int UpdateGUI(void *inf)
{
  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  info->SetGUIProperty(info, 0, VVP_GUI_LABEL, "Number of Iterations ");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT, "5");
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "Number of times that the diffusion approximation will be computed. The more iterations, the stronger the smoothing");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS , "1 100 1");

  info->SetGUIProperty(info, 1, VVP_GUI_LABEL, "Time Step");
  info->SetGUIProperty(info, 1, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 1, VVP_GUI_DEFAULT, "0.05");
  info->SetGUIProperty(info, 1, VVP_GUI_HELP, "Discretization of time for approximating the diffusion process.");
  info->SetGUIProperty(info, 1, VVP_GUI_HINTS , "0.01 1.0 0.005");

  info->SetGUIProperty(info, 2, VVP_GUI_LABEL, "Conductance");
  info->SetGUIProperty(info, 2, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 2, VVP_GUI_DEFAULT, "3.0");
  info->SetGUIProperty(info, 2, VVP_GUI_HELP, "Factor that multiplies the image gradient in order to compute the effective conductance locally. The higher the value of this parameter, the stronger the diffusion will be");
  info->SetGUIProperty(info, 2, VVP_GUI_HINTS , "0.1 10.0 0.1");

  const char * stringValue = info->GetGUIProperty(info, 0, VVP_GUI_VALUE );
  if( !stringValue )
    {
    info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP, "0");
    }
  else
    {
    info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP, stringValue);
    }

  info->OutputVolumeScalarType = info->InputVolumeScalarType;
  info->OutputVolumeNumberOfComponents = info->InputVolumeNumberOfComponents;

  info->OutputVolumeDimensions[0] = info->InputVolumeDimensions[0]; // + 2 * numberOfIterations;
  info->OutputVolumeDimensions[1] = info->InputVolumeDimensions[1]; // + 2 * numberOfIterations;
  info->OutputVolumeDimensions[2] = info->InputVolumeDimensions[2]; // + 2 * numberOfIterations;

  memcpy(info->OutputVolumeSpacing,info->InputVolumeSpacing,
         3*sizeof(float));
  memcpy(info->OutputVolumeOrigin,info->InputVolumeOrigin,
         3*sizeof(float));

  // provide accurate estimate of memory required
  // always requires 2 floats per voxel
  int sizeReq = 2*sizeof(float);
  // if multicomponent also requires one scalar more
  if (info->InputVolumeNumberOfComponents > 1)
    {
    sizeReq += info->InputVolumeScalarSize;
    }
  char tmps[500];
  sprintf(tmps,"%i",sizeReq);
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED, tmps); 

  return 1;
}


extern "C" {
  
void VV_PLUGIN_EXPORT vvITKGradientAnisotropicDiffusionInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Gradient Anisotropic Diffusion (ITK)");
  info->SetProperty(info, VVP_GROUP, "Noise Suppression");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                  "Anisotropic diffusion smoothing");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This filter applies an edge-preserving smoothing to a volume by computing the evolution of an anisotropic diffusion partial differential equation. Diffusion is regulated by the gradient of the image. This filter processes the whole image in one piece, and does not change the dimensions, data type, or spacing of the volume.");
  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "3");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "8"); 
  info->SetProperty(info, VVP_REQUIRES_SERIES_INPUT,        "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_SERIES_BY_VOLUMES, "0");
  info->SetProperty(info, VVP_PRODUCES_OUTPUT_SERIES, "0");
  info->SetProperty(info, VVP_PRODUCES_PLOTTING_OUTPUT, "0");
}

}
