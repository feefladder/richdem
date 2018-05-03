#ifndef _richdem_Fairfield1991_hpp_
#define _richdem_Fairfield1991_hpp_

#include "richdem/common/constants.hpp"
#include "richdem/common/logger.hpp"
#include "richdem/common/Array2D.hpp"
#include "richdem/common/Array3D.hpp"
#include "richdem/common/ProgressBar.hpp"
#include "richdem/common/random.hpp"

namespace richdem {

template<class E>
void FM_FairfieldLeymarieD4(const Array2D<E> &elevations, Array3D<float> &props){
  RDLOG_ALG_NAME<<"Fairfield (1991) \"Rho4\" Flow Accumulation";
  RDLOG_CITATION<<"Fairfield, J., Leymarie, P., 1991. Drainage networks from grid digital elevation models. Water resources research 27, 709–717.";

  props.setAll(NO_FLOW_GEN);

  ProgressBar progress;
  progress.start(elevations.size());

  #pragma omp parallel for collapse(2)
  for(int y=1;y<elevations.height()-1;y++)
  for(int x=1;x<elevations.width()-1;x++){
    ++progress;

    const E e    = elevations(x,y);

    int    greatest_n     = 0; //TODO: Use a constant
    double greatest_slope = 0;
    for(int n=1;n<=8;n++){
      if(n_diag[n]) //Skip diagonals
        continue;

      const int nx = x+dx[n];
      const int ny = y+dy[n];

      if(!elevations.inGrid(nx,ny))
        continue;
      if(elevations.isNoData(nx,ny)) //TODO: Don't I want water to drain this way?
        continue;

      const E ne = elevations(nx,ny);

      if(ne>=e)
        continue;

      double rho_slop4 = (e-ne);
      if(n==D8_NORTH || n==D8_SOUTH)
        rho_slop4 *= 1/(1/uniform_rand_real(0,1)-1);

      if(rho_slop4>greatest_slope){
        greatest_n     = n;
        greatest_slope = rho_slop4;
      }
    }

    if(greatest_n==0)
      continue;

    props(x,y,0)          = HAS_FLOW_GEN;
    props(x,y,greatest_n) = 1;

    assert(elevations(x,y)>=elevations(x+dx[greatest_n],y+dy[greatest_n])); //Ensure flow goes downhill
  }
  progress.stop();
}



template<class E>
void FM_FairfieldLeymarieD8(
  const Array2D<E> &elevations, 
  Array3D<float> &props
){
  RDLOG_ALG_NAME<<"Fairfield (1991) \"Rho8\" Flow Accumulation";
  RDLOG_CITATION<<"Fairfield, J., Leymarie, P., 1991. Drainage networks from grid digital elevation models. Water resources research 27, 709–717.";

  props.setAll(NO_FLOW_GEN);

  ProgressBar progress;
  progress.start(elevations.size());

  #pragma omp parallel for collapse(2)
  for(int y=1;y<elevations.height()-1;y++)
  for(int x=1;x<elevations.width()-1;x++){
    ++progress;

    const E e    = elevations(x,y);

    int    greatest_n     = 0; //TODO: Use a constant
    double greatest_slope = 0;
    for(int n=1;n<=8;n++){
      const int nx = x+dx[n];
      const int ny = y+dy[n];

      if(!elevations.inGrid(nx,ny))
        continue;
      if(elevations.isNoData(nx,ny)) //TODO: Don't I want water to drain this way?
        continue;

      const E ne = elevations(nx,ny);

      if(ne>=e)
        continue;

      double rho_slope = (e-ne);
      if(n_diag[n])
        rho_slope *= 1/(2-uniform_rand_real(0,1));

      if(rho_slope>greatest_slope){
        greatest_n     = n;
        greatest_slope = rho_slope;
      }
    }

    if(greatest_n==0)
      continue;

    props(x,y,0)          = HAS_FLOW_GEN;
    props(x,y,greatest_n) = 1;

    assert(elevations(x,y)>=elevations(x+dx[greatest_n],y+dy[greatest_n])); //Ensure flow goes downhill
  }
  progress.stop();
}



template<class E>
void FM_Rho8(const Array2D<E> &elevations, Array3D<float> &props){
  //Algorithm headers are taken care of in FM_FairfieldLeymarie()
  FM_FairfieldLeymarieD8(elevations, props);
}



template<class E>
void FM_Rho4(const Array2D<E> &elevations, Array3D<float> &props){
  //Algorithm headers are taken care of in FM_FairfieldLeymarie()
  FM_FairfieldLeymarieD4(elevations, props);
}

}

#endif
