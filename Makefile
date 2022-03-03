#####################
# Makefile pour LINUX
#####################

CC=gcc
CXXFLAGS= -I./XTool/

src_files_XTool = ./XTool/XAffinity2D.cpp ./XTool/XAlgoGeod.cpp ./XTool/XBaryMover.cpp ./XTool/XDBase.cpp ./XTool/XDelaunay.cpp ./XTool/XDomGeod.cpp ./XTool/XEndian.cpp ./XTool/XErrorXml.cpp ./XTool/XFile.cpp ./XTool/XFrame.cpp ./XTool/XGeoBase.cpp ./XTool/XGeoClass.cpp ./XTool/XGeocon.cpp ./XTool/XGeoconConverter.cpp ./XTool/XGeodGrid.cpp ./XTool/XGeoJson.cpp ./XTool/XGeoJsonConverter.cpp ./XTool/XGeoLayer.cpp ./XTool/XGeoLine.cpp ./XTool/XGeoMap.cpp ./XTool/XGeoMemVector.cpp ./XTool/XGeoPoint.cpp ./XTool/XGeoPoly.cpp ./XTool/XGeoPref.cpp ./XTool/XGeoProjection.cpp ./XTool/XGeoRaster.cpp ./XTool/XGeoRepres.cpp ./XTool/XGeoVector.cpp ./XTool/XGpx.cpp ./XTool/XInterpol.cpp ./XTool/XKml.cpp ./XTool/XKmlConverter.cpp ./XTool/XKmlWriter.cpp ./XTool/XLambert.cpp ./XTool/XMatcher.cpp ./XTool/XMifMid.cpp ./XTool/XMifMidConverter.cpp ./XTool/XNCGeod.cpp ./XTool/XParserXML.cpp ./XTool/XPath.cpp ./XTool/XPolyContour.cpp ./XTool/XPolygone2D.cpp ./XTool/XPt2D.cpp ./XTool/XPt3D.cpp ./XTool/XShapefile.cpp ./XTool/XShapefileConverter.cpp ./XTool/XShapefileRecord.cpp ./XTool/XShapefileVector.cpp ./XTool/XTrigMover.cpp ./XTool/XWebMercator.cpp ./XTool/XXml.cpp

#echo $(src_files_XTool)

src_files_RPCUMover := ${src_files_XTool} ./RPCUMover/RPCUMover.cpp
#echo $(src_files_RPCUMover)

#src_files_RPCUPairing := ${src_files_XTool} ./RPCUPairing/RPCUPairing.cpp

RPCUMover: $(src_files_RPCUMover)
     $(CC) $(CXXFLAGS) -o RPCUMover $(src_files_RPCUMover)
     
#RPCUPairing: $(src_files_RPCUPairing)
#$(CC) $(CXXFLAGS) -o RPCUPairing $(src_files_RPCUPairing)
     
