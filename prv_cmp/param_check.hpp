void param_check(double& root_hermit_, usint& pt, uint32_t& relWindow_, size_t& numDcrtBits_, size_t& numbit_, unsigned int& mul_depth_) {
	if (numDcrtBits_ == 30) {
     if (relWindow_ == 30) {
        switch (numbit_) {
          case 10: {
            root_hermit_ = params_N4095_rel30[mul_depth_ - 1][1];
                      pt = params_N4095_rel30[mul_depth_ - 1][3];
            break;
          }

          case 11: {
            root_hermit_ = params_N4095_rel30[mul_depth_ - 1][1];
                      pt = params_N4095_rel30[mul_depth_ - 1][3];
            break;
          }

          case 12: {
            root_hermit_ = params_N4095_rel30[mul_depth_ - 1][1];
                      pt = params_N4095_rel30[mul_depth_ - 1][3];
            break;
          }

          case 13: {
            root_hermit_ = params_N8191_rel30[mul_depth_ - 1][1];
                      pt = params_N8191_rel30[mul_depth_ - 1][3];
            break;
          }

          case 14: {
            root_hermit_ = params_N16383_rel30[mul_depth_ - 1][1];
                      pt = params_N16383_rel30[mul_depth_ - 1][3];
            break;
          }

          case 15: {
            root_hermit_ = params_N32767_rel30[mul_depth_ - 1][1];
                     pt = params_N32767_rel30[mul_depth_ - 1][3];
            break;
          }

          case 16: {
            cerr << "logN = 16 is not supported" << endl;
            exit(1);
            break;
          }

          default : {
            cerr << " non-supported bitlen" << endl;
            exit(1);
             break;
          }
        }
      } else if (relWindow_ == 0) {
        switch (numbit_) {
          case 10: {
            root_hermit_ = params_N4095_rel0[mul_depth_ - 1][1];
                      pt = params_N4095_rel0[mul_depth_ - 1][3];
            break;
          }

          case 11: {
            root_hermit_ = params_N4095_rel0[mul_depth_ - 1][1];
                      pt = params_N4095_rel0[mul_depth_ - 1][3];
            break;
          }

          case 12: {
            root_hermit_ = params_N4095_rel0[mul_depth_ - 1][1];
                      pt = params_N4095_rel0[mul_depth_ - 1][3];
            break;
          }

          case 13: {
            root_hermit_ = params_N8191_rel0[mul_depth_][1];
                      pt = params_N8191_rel0[mul_depth_][3];
            break;
          }

          case 14: {
            root_hermit_ = params_N16383_rel0[mul_depth_][1];
                      pt = params_N16383_rel0[mul_depth_][3];
            break;
          }

          case 15: {
            root_hermit_ = params_N32767_rel0[mul_depth_ - 1][1];
                     pt = params_N32767_rel0[mul_depth_ - 1][3];
            break;
          }

          case 16: {
            cerr << "logN = 16 is not supported" << endl;
            exit(1);
            break;
          }

          default : {
            cerr << "un-supported bit" << endl;
            exit(1);
             break;
          }
        }
      } else {
        cerr << "Unsupported window" << relWindow_ << endl;
				exit(1);
			}
	} else if (numDcrtBits_ == 60) {
     if (relWindow_ == 30) {
		 /*
        switch (numbit_) {
          case 10: {
            root_hermit_ = params_N4095_rel30_crt60[mul_depth_ - 1][1];
                      pt = params_N4095_rel30_crt60[mul_depth_ - 1][3];
            break;
          }

          case 11: {
            root_hermit_ = params_N4095_rel30_crt60[mul_depth_ - 1][1];
                      pt = params_N4095_rel30_crt60[mul_depth_ - 1][3];
            break;
          }

          case 12: {
            root_hermit_ = params_N4095_rel30_crt60[mul_depth_ - 1][1];
                      pt = params_N4095_rel30_crt60[mul_depth_ - 1][3];
            break;
          }

          case 13: {
            root_hermit_ = params_N8191_rel30_crt60[mul_depth_ - 1][1];
                      pt = params_N8191_rel30_crt60[mul_depth_ - 1][3];
            break;
          }

          case 14: {
            root_hermit_ = params_N16383_rel30_crt60[mul_depth_ - 1][1];
                      pt = params_N16383_rel30[mul_depth_ - 1][3];
            break;
          }

          case 15: {
            cerr << "logN = 15 is not supported" << endl;
            exit(1);
            break;
          }

          case 16: {
            cerr << "logN = 16 is not supported" << endl;
            exit(1);
            break;
          }

          default : {
             break;
          }
        }
			*/
      } else if (relWindow_ == 0) {
        switch (numbit_) {
          case 10: {
            root_hermit_ = params_N8191_rel0_crt60[mul_depth_ - 1][1];
                      pt = params_N8191_rel0_crt60[mul_depth_ - 1][3];
            break;
          }

          case 11: {
            root_hermit_ = params_N8191_rel0_crt60[mul_depth_ - 1][1];
                      pt = params_N8191_rel0_crt60[mul_depth_ - 1][3];
            break;
          }

          case 12: {
            root_hermit_ = params_N8191_rel0_crt60[mul_depth_ - 1][1];
                      pt = params_N8191_rel0_crt60[mul_depth_ - 1][3];
            break;
          }

          case 13: {
					  cout << "**********   crtbit = 60 && relwindow = 0, bit=13" << endl;
            root_hermit_ = params_N8191_rel0_crt60[mul_depth_][1];
                      pt = params_N8191_rel0_crt60[mul_depth_][3];
            break;
          }

          case 14: {
					  cout << "***********   crtbit = 60 && relwindow = 0, bit=14, dept=" << mul_depth_ << endl;
            root_hermit_ = params_N16383_rel0_crt60[mul_depth_][1];
                      pt = params_N16383_rel0_crt60[mul_depth_][3];
            break;
          }

          case 15: {
            cerr << "logN = 15 is not supported" << endl;
            exit(1);
            break;
          }

          case 16: {
            cerr << "logN = 16 is not supported" << endl;
            exit(1);
            break;
          }

          default : {
        		 cerr << "Unsupported bitlent"  << endl;
						 exit(1);
             break;
          }
        }
      } else {
        cerr << "Unsupported window" << relWindow_ << endl;
				exit(1);
			}
	}
}
