#include "ghb.h"

// Library headers.
#ifndef INCLUDED_SSTREAM
#include <sstream>
#define INCLUDED_SSTREAM
#endif

#ifndef INCLUDED_IOSTREAM
#include <iostream>
#define INCLUDED_IOSTREAM
#endif

#ifndef INCLUDED_IOMANIP
#include <iomanip>
#define INCLUDED_IOMANIP
#endif

#ifndef INCLUDED_FSTREAM
#include <fstream>
#define INCLUDED_FSTREAM
#endif

#ifndef INCLUDED_CASSERT
#include <cassert>
#define INCLUDED_CASSERT
#endif


// PCRaster library headers.
#ifndef INCLUDED_CALC_SPATIAL
#include "calc_spatial.h"
#define INCLUDED_CALC_SPATIAL
#endif


// Module headers.
#ifndef INCLUDED_COMMON
#include "common.h"
#define INCLUDED_COMMON
#endif

#ifndef INCLUDED_GRIDCHECK
#include "gridcheck.h"
#define INCLUDED_GRIDCHECK
#endif

#ifndef INCLUDED_PCRMODFLOW
#include "pcrmodflow.h"
#define INCLUDED_PCRMODFLOW
#endif
#ifndef INCLUDED_MF_BINARYREADER
#include "mf_BinaryReader.h"
#define INCLUDED_MF_BINARYREADER
#endif

#include "mf_utils.h"



GHB::~GHB(){
}


GHB::GHB(PCRModflow *mf) :
  d_mf(mf),
  d_ghbUpdated(false),
  d_nr_ghb_cells(0),
  d_output_unit_number(255),
  d_input_unit_number(256){
}


bool GHB::ghbUpdated() const
{
  return d_ghbUpdated;
}


void GHB::setGhbUpdated(bool value)
{
  d_ghbUpdated = value;
}


void GHB::setGHB(const calc::Field * head, const calc::Field * cond, size_t layer){
  layer--; // layer number passed by user starts with 1
  d_mf->d_gridCheck->isGrid(layer, "setGeneralHead");
  d_mf->d_gridCheck->isConfined(layer, "setGeneralHead");

  d_mf->d_methodName = "setGeneralHead head values";
  d_mf->setBlockData(*(d_mf->d_ghbHead), head->src_f(), layer);

  d_mf->d_methodName = "setGeneralHead conductance values";
  d_mf->setBlockData(*(d_mf->d_ghbCond), cond->src_f(), layer);

  setGhbUpdated(true);
}


void GHB::setGHB(const float * head, const float * cond, size_t layer){
  layer--; // layer number passed by user starts with 1
  d_mf->d_methodName = "setGeneralHead head values";
  d_mf->setBlockData(*(d_mf->d_ghbHead), head, layer);
  d_mf->d_methodName = "setGeneralHead conductance values";
  d_mf->setBlockData(*(d_mf->d_ghbCond), cond, layer);

  setGhbUpdated(true);
}


void GHB::write(std::string const& path){

  // # ghb cells is calculated by write_list
  assert(d_nr_ghb_cells != 0);

  std::string filename = mf::execution_path(path, "pcrmf.ghb");

  std::ofstream content(filename);

  if(!content.is_open()){
    std::cerr << "Can not write " << filename << std::endl;
    exit(1);
  }

  content << "# Generated by PCRaster Modflow\n";
  content << d_nr_ghb_cells;
  content << " " << d_output_unit_number;
  content << " NOPRINT\n";
  content << d_nr_ghb_cells;
  content << " 0\n";
  content << "EXTERNAL " << d_input_unit_number << "\n";

  d_nr_ghb_cells = 0;
}


void GHB::write_list(std::string const& path){
  // This method also calculates the nr of ghb cells,
  // needs to be called before write
  std::string filename = mf::execution_path(path, "pcrmf_ghb.asc");
  std::ofstream content(filename);

  if(!content.is_open()){
    std::cerr << "Can not write " << filename << std::endl;
    exit(1);
  }

  int mfLayer = 1;

  for(size_t layer = 1; layer<= d_mf->d_nrMFLayer; layer++){
    size_t count = 0;
    size_t size = d_mf->d_layer2BlockLayer.size();
    size_t blockLayer = d_mf->d_layer2BlockLayer.at(size - layer);

    for(size_t row = 0; row < d_mf->d_nrOfRows; row++){
      for(size_t col = 0; col < d_mf->d_nrOfColumns; col++){

        double head = d_mf->d_ghbHead->cell(count)[blockLayer];
        double cond = d_mf->d_ghbCond->cell(count)[blockLayer];

        if(cond > 0.0){
          content << mfLayer;
          content << " " << (row + 1);
          content << " " << (col + 1);
          content << " " << head;
          content << " " << cond;
          content << "\n";
          d_nr_ghb_cells++;
        }
        count++;
      }
    }
    mfLayer++;
  }
}

calc::Field* GHB::getGhbLeakage(size_t layer, std::string const& path) const {
  layer--; // layer number passed by user starts with 1
  d_mf->d_gridCheck->isGrid(layer, "getGeneralHeadLeakage");
  d_mf->d_gridCheck->isConfined(layer, "getGeneralHeadLeakage");

  const std::string desc(" HEAD DEP BOUNDS");
  std::stringstream stmp;
  stmp << "Can not open file containing GHB cell-by-cell flow terms";

  // modflow reports from top to bottom, thus
  // get the 'inverse' layer number to start from the right position
  int pos_multiplier = d_mf->get_modflow_layernr(layer);

  auto* spatial = new calc::Spatial(VS_S, calc::CRI_f, d_mf->d_nrOfCells);
  auto* cells = static_cast<REAL4*>(spatial->dest());

  mf::BinaryReader reader;
  const std::string filename(mf::execution_path(path, "fort." + std::to_string(d_output_unit_number)));
  reader.read(stmp.str(), filename, cells, desc, pos_multiplier);

  return spatial;
}


