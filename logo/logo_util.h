#ifndef LOGO_UTIL_H
#define LOGO_UTIL_H
#include <logo/logo_structs.h>
#include <logo/logo_sta.h>



void logo_lpc(const logo_vector& data, const int& order, logo_vector& A,logo_real& G);
void logo_cepsLPC(const logo_vector& data, const int& orderLPC,  const int& orderCepstrum, logo_vector& C);


#endif // UTIL_H
