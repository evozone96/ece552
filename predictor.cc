#include "predictor.h"

/////////////////////////////////////////////////////////////
// 2bitsat
/////////////////////////////////////////////////////////////
int state;

void InitPredictor_2bitsat() {
  state = 1;

}

bool GetPrediction_2bitsat(UINT32 PC) {
  switch (state) {
    case 0: return NOT_TAKEN;
    case 1: return NOT_TAKEN;
    case 2: return TAKEN;
    case 3: return TAKEN;
  }
}

void UpdatePredictor_2bitsat(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {
  if (resolveDir == TAKEN) {
    state += 1;
  } else {
    state -= 1;
  }
  if (state < 0) {
    state = 0;
  }
  if (state > 3) {
    state = 3;
  }
}

/////////////////////////////////////////////////////////////
// 2level
/////////////////////////////////////////////////////////////

void InitPredictor_2level() {

}

bool GetPrediction_2level(UINT32 PC) {

  return TAKEN;
}

void UpdatePredictor_2level(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {

}

/////////////////////////////////////////////////////////////
// openend
/////////////////////////////////////////////////////////////

void InitPredictor_openend() {

}

bool GetPrediction_openend(UINT32 PC) {

  return TAKEN;
}

void UpdatePredictor_openend(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {

}

