/*=========================================================================

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/VolViewCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/* This plugin adapts the GeodesicActveContour filter.
   It does not perform any preprocessing. The user should provide
   the speed image and the initial level set as inputs. */

#include "vvITKGeodesicActiveContour.h"



template <class InputPixelType>
class GeodesicActiveContoureRunner
  {
  public:
    typedef itk::Image< InputPixelType, 3 > InputImageType;
    typedef InputImageType                  InputSpeedImageType;
    
    typedef VolView::PlugIn::GeodesicActiveContour< InputImageType,
                                                    InputSpeedImageType >   ModuleType;
    
  public:
    GeodesicActiveContoureRunner() {}
    void Execute( vtkVVPluginInfo *info, vtkVVProcessDataStruct *pds )
    {
      char tmp[1024];

      ModuleType  module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Computing Geodesic Active Contour...");
      module.ProcessData( pds  );
      sprintf(tmp,"Total number of iterations = %d \n Final RMS error = %g",
                         module.GetFilter()->GetElapsedIterations(),
                         module.GetFilter()->GetRMSChange());
      info->SetProperty( info, VVP_REPORT_TEXT, tmp );

    }
  };




static int ProcessData(void *inf, vtkVVProcessDataStruct *pds)
{

  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  if( info->InputVolumeNumberOfComponents != 1 )
    {
    info->SetProperty( info, VVP_ERROR, "This filter requires a single-component data set as input" ); 
    return -1;
    }

  try 
  {
  switch( info->InputVolumeScalarType )
    {
    case VTK_CHAR:
      {
      GeodesicActiveContoureRunner<signed char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_CHAR:
      {
      GeodesicActiveContoureRunner<unsigned char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_SHORT:
      {
      GeodesicActiveContoureRunner<signed short> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_SHORT:
      {
      GeodesicActiveContoureRunner<unsigned short> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_INT:
      {
      GeodesicActiveContoureRunner<signed int> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_INT:
      {
      GeodesicActiveContoureRunner<unsigned int> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_LONG:
      {
      GeodesicActiveContoureRunner<signed long> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_LONG:
      {
      GeodesicActiveContoureRunner<unsigned long> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_FLOAT:
      {
      GeodesicActiveContoureRunner<float> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_DOUBLE:
      {
      GeodesicActiveContoureRunner<double> runner;
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

  info->UpdateProgress( info, 1.0, "Geodesic Active Contour LevelSet Done !");

  return 0;
}


static int UpdateGUI(void *inf)
{
  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  info->SetGUIProperty(info, 0, VVP_GUI_LABEL, "Sigma for gradient magnitude.");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT, "1.0");
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "Sigma used by the smoothing previous to computing the gradient magnitude. Large values of sigma will reduce noise in the image but will also degrade contours. Sigma is measured in millimeters, not pixels");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS , "0.1 10.0 0.1");

  info->SetGUIProperty(info, 1, VVP_GUI_LABEL, "Curvature scaling.");
  info->SetGUIProperty(info, 1, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 1, VVP_GUI_DEFAULT, "1.0");
  info->SetGUIProperty(info, 1, VVP_GUI_HELP, "Scaling factor for the curvature contribution. Larger values will result in smoother contours");
  info->SetGUIProperty(info, 1, VVP_GUI_HINTS , "0.1 10.0 0.1");

  info->SetGUIProperty(info, 2, VVP_GUI_LABEL, "Propagation scaling.");
  info->SetGUIProperty(info, 2, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 2, VVP_GUI_DEFAULT, "1.0");
  info->SetGUIProperty(info, 2, VVP_GUI_HELP, "Scaling factor for the inflation factor. Larger factors will result in rapid expansion with irregular borders");
  info->SetGUIProperty(info, 2, VVP_GUI_HINTS , "0.1 10.0 0.1");

  info->SetGUIProperty(info, 3, VVP_GUI_LABEL, "Advection scaling.");
  info->SetGUIProperty(info, 3, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 3, VVP_GUI_DEFAULT, "1.0");
  info->SetGUIProperty(info, 3, VVP_GUI_HELP, "Scaling factor for the advection factor. This factor generates a force that is proportional to the slant of the level set at a point.");
  info->SetGUIProperty(info, 3, VVP_GUI_HINTS , "0.1 10.0 0.1");

  info->SetGUIProperty(info, 4, VVP_GUI_LABEL, "Maximum RMS Error.");
  info->SetGUIProperty(info, 4, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 4, VVP_GUI_DEFAULT, "0.06");
  info->SetGUIProperty(info, 4, VVP_GUI_HELP, "Threshold of the RMS change between one iteration and the previous one. This is a convergence criteria, the process will stop when the RMS change is lower than the value set here");
  info->SetGUIProperty(info, 4, VVP_GUI_HINTS , "0.01 0.5 0.01");

  info->SetGUIProperty(info, 5, VVP_GUI_LABEL, "Maximum iterations.");
  info->SetGUIProperty(info, 5, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 5, VVP_GUI_DEFAULT, "100.0");
  info->SetGUIProperty(info, 5, VVP_GUI_HELP, "The maximum number of iteration to apply the time step in the partial differental equation.");
  info->SetGUIProperty(info, 5, VVP_GUI_HINTS , "1.0 500.0 1.0");

  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP, "0");
  
  info->OutputVolumeScalarType = VTK_UNSIGNED_CHAR;
  info->OutputVolumeNumberOfComponents = 1;

  memcpy(info->OutputVolumeDimensions,info->InputVolumeDimensions,
         3*sizeof(int));
  memcpy(info->OutputVolumeSpacing,info->InputVolumeSpacing,
         3*sizeof(float));
  memcpy(info->OutputVolumeOrigin,info->InputVolumeOrigin,
         3*sizeof(float));

  return 1;
}


extern "C" {
  
void VV_PLUGIN_EXPORT vvITKGeodesicActiveContourInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI   = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Geodesic Active Contour (ITK)");
  info->SetProperty(info, VVP_GROUP, "Segmentation - Level Sets");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                    "Geodesic Active Contour");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This module applies the Geodesic Active Contour method for segmenting a volume. No preprocessing is performed here. The user must provide as inputs an initial level set and the feature image which will be used to compute speeds. The current image being visualized will be taken as the initial level set. The speed image required  as a second input can be specified by providinng a file name in the GUI.");

  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "6");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,   "16");
  info->SetProperty(info, VVP_REQUIRES_SECOND_INPUT,        "1");
  info->SetProperty(info, VVP_REQUIRES_SERIES_INPUT,        "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_SERIES_BY_VOLUMES, "0");
  info->SetProperty(info, VVP_PRODUCES_OUTPUT_SERIES, "0");
  info->SetProperty(info, VVP_PRODUCES_PLOTTING_OUTPUT, "0");
}

}
