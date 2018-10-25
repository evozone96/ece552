#include "predictor.h"

/////////////////////////////////////////////////////////////
// 2bitsat
/////////////////////////////////////////////////////////////
int state_2bitsat;
int total_2bitsat;
int correct_2bitsat;
int bht_2level[512];
int pht_2level[64][8];

void InitPredictor_2bitsat() {
  state = 1;
  total_2bitsat = 0;
  correct_2bitsat = 0;
}

bool GetPrediction_2bitsat(UINT32 PC) {
  switch (state_2bitsat) {
    case 0: return NOT_TAKEN;
    case 1: return NOT_TAKEN;
    case 2: return TAKEN;
    case 3: return TAKEN;
  }
}

void UpdatePredictor_2bitsat(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {
  if (resolveDir == TAKEN) {
    state_2bitsat += 1;
  } else {
    state_2bitsat -= 1;
  }
  if (state_2bitsat < 0) {
    state_2bitsat = 0;
  }
  if (state_2bitsat > 3) {
    state_2bitsat = 3;
  }
  if (predDir == resolveDir) {
    correct_2bitsat += 1;
  }
  total_2bitsat += 1;
}

/////////////////////////////////////////////////////////////
// 2level
/////////////////////////////////////////////////////////////

void InitPredictor_2level() {
  for (int i = 0 ; i < 512 ; i++)
  {
     bht_2level = 0;
  }
  for (int i = 0 ; i < 64 ; i++) {
    for (int j = 0 ; j < 8 ; j++) {
      pht_2level[i][j] = 1;
    }
  }
}

bool GetPrediction_2level(UINT32 PC) {

  int a = PC % 8, b = (PC / 8)%512;
  int bht = bht_2level[b];
  int state = pht_2level[bht][a];
  switch (state) {
    case 0: return NOT_TAKEN;
    case 1: return NOT_TAKEN;
    case 2: return TAKEN;
    case 3: return TAKEN;
  }
}

void UpdatePredictor_2level(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {

  int a = PC % 8, b = (PC / 8)%512;
  int prev_bht = bht_2level[b], bht = (bht_2level[b] * 2) % 64;
  if (resolveDir) {
    bht += 1;
  }
  bht_2level[b] = bht;
  if (resolveDir == TAKEN) {
    pht_2level[prev_bht][a] += 1;
  } else {
    pht_2level[prev_bht][a] -= 1;
  }
  if (pht_2level[prev_bht][a] < 0) {
    pht_2level[prev_bht][a] = 0;
  }
  if (pht_2level[prev_bht][a] > 3) {
    pht_2level[prev_bht][a] = 3;
  }

}

/////////////////////////////////////////////////////////////
// openend
/////////////////////////////////////////////////////////////


void InitPredictor_openend() {
	for (int i=0; i<65536; i++) {
		GPT_open[i] = 1;
	}

	for (int j=0; j<1024; j++) {
		BHT_2level_open[j] = 0;
	}

	for (int k=0; k<16; k++) {
		for (int l=0; l<1024; l++) {
			PPT_2level_open[k][l] = 1;
		}
	}

	for (int m=0; m<16; m++) {
		for (int n=0; n<512; n++) {
			selector_PPT[m][n] = 1;
		}
		selector_BHT[m] = 0;
	}

}

bool GetPrediction_openend(UINT32 PC) {
	bool pred, pred0, pred1;
	unsigned int BHT_index, BHR, PPT_index, selector_index;

//pred0
	if (GPT_open[GHR_open] <= 1) {
		pred0 = NOT_TAKEN;
	}
	else {
		pred0 = TAKEN;
	}

//pred1
	BHT_index = (PC & 0x3FF0) >> 4;
	BHR = BHT_2level_open[BHT_index];
	PPT_index = PC & 0xF;

	if (PPT_2level_open[PPT_index][BHR] <= 1) {
		pred1 = NOT_TAKEN;
		pred1_open = NOT_TAKEN;
	}
	else {
		pred1 = TAKEN;
		pred1_open = TAKEN;
	}

//selector
	selector_index = PC & 0xF;
	if (selector_PPT[selector_index][(selector_BHT[selector_index])] <= 1) {
		pred = pred0;
	}
	else {
		pred = pred1;
	}

	return pred;
}

void UpdatePredictor_openend(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {
	unsigned int BHT_index, BHR, PPT_index, selector_index;

	BHT_index = (PC & 0x3FF0) >> 4;
	BHR = BHT_2level_open[BHT_index];
	PPT_index = PC & 0xF;
	selector_index = PC & 0xF;


	if (resolveDir == TAKEN) {
		//pred0
		if (GPT_open[GHR_open] < 3) {
			GPT_open[GHR_open] ++;
		}
		GHR_open = ((GHR_open << 1) + 1) & 0xFFFF;

		//pred1
		if (PPT_2level_open[PPT_index][BHR] < 3) {
			PPT_2level_open[PPT_index][BHR] ++;
		}
		BHT_2level_open[BHT_index] = ((BHT_2level_open[BHT_index] << 1) + 1) & 0x3FF;
	}
	else {
		//pred0
		if (GPT_open[GHR_open] > 0) {
			GPT_open[GHR_open] --;
		}
		GHR_open = ((GHR_open << 1) + 0) & 0xFFFF;

		//pred1
		if (PPT_2level_open[PPT_index][BHR] > 0) {
			PPT_2level_open[PPT_index][BHR] --;
		}
		BHT_2level_open[BHT_index] = ((BHT_2level_open[BHT_index] << 1) + 0) & 0x3FF;
  }

//selector
	if (resolveDir == pred1_open) {
		if (selector_PPT[selector_index][(selector_BHT[selector_index])] < 3) {
			selector_PPT[selector_index][(selector_BHT[selector_index])] ++;
		}
		selector_BHT[selector_index] = (((selector_BHT[selector_index]) << 1) + 1) & 0x1FF;
	}
	else {
		if (selector_PPT[selector_index][(selector_BHT[selector_index])] > 0) {
			selector_PPT[selector_index][(selector_BHT[selector_index])] --;
		}
		selector_BHT[selector_index] = (((selector_BHT[selector_index]) << 1) + 0) & 0x1FF;
	}

