/*
 * lis3dsh.h
 *
 *  Created on: 2017/08/09
 *      Author: take-iwiw
 */

#ifndef LIS3DSH_LIS3DSH_H_
#define LIS3DSH_LIS3DSH_H_

RET lis3dsh_init();
RET lis3dsh_readAcc(int32_t out[3]);

#endif /* LIS3DSH_LIS3DSH_H_ */
