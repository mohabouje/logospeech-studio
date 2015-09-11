#include "logo_util.h"



void logo_autocor(const logo_vector& frame, float* R, const int& order ){
    const int size = frame.size();
    double sum;
    for (int i=0;i<order;i++) {
        sum = 0.0;
        for (int j=0;j<size-i;j++)
           sum += float(frame[j]*frame[j+i]);
        R[i] = sum;
    }
}



double logo_levinsonDurbin(const unsigned int& P,float *R, logo_real *A, float *K)
{
    float Am1[50];

   if (R[0]==0.0) {
        for (unsigned int i=1; i<=P; i++) {
            K[i]=0.0;
            A[i]=0.0;
        }
        return 0;
   } else {
        double km = 0.0, Em1 = 0.0, Em = 0.0;
        unsigned int k = 0, s = 0, m = 0;
        for (k=0;k<=P;k++){
            A[0]=0;
            Am1[0]=0;
        }
        A[0]=1;
        Am1[0]=1;
        km=0;
        Em1=R[0];
        for (m=1;m<=P;m++) {
            double err=0.0f;                    //err = 0;
            for (k=1;k<=m-1;k++)            //for k=2:m-1
                err += Am1[k]*R[m-k];        // err = err + am1(k)*R(m-k+1);
            km = (R[m]-err)/Em1;            //km=(R(m)-err)/Em1;
            K[m-1] = -float(km);
            A[m]=(float)km;                        //am(m)=km;
            for (k=1;k<=m-1;k++)            //for k=2:m-1
                A[k]=float(Am1[k]-km*Am1[m-k]);  // am(k)=am1(k)-km*am1(m-k+1);
            Em=(1-km*km)*Em1;                //Em=(1-km*km)*Em1;
            for(s=0;s<=P;s++)                //for s=1:N+1
                Am1[s] = A[s];                // am1(s) = am(s)
            Em1 = Em;                        //Em1 = Em;
        }
        for (k=1;k<=m-1;k++) {
            A[k]=-A[k];

        }
         return Em;
    }
}


void logo_lpc(const logo_vector& data, const int& order, logo_vector& A, logo_real& G) {
    int samplesRequired = order + 1;
    QVector<float> K(samplesRequired,0);
    QVector<float> R(samplesRequired,0);
    logo_autocor(data, R.data(), samplesRequired);
    G=logo_levinsonDurbin(order,R.data(),A.data(),K.data());
}



void logo_lpc2Cepstrum(logo_vector& A, const int& orderLPC, logo_vector& C, const int& orderCepstrum){
    float sum = 0;
    C[0]=0;
    for(int m=1;m < orderCepstrum;m++){
        if(m<orderLPC+1){
            sum=0;
            for(int k=1; k<=(m-1); k++)
                sum += double(-1.0*(m-k)/m)*A[k]*C[m-k];

            C[m]=-A[m]+sum;
        }else{
            sum=0;
            for(int k=1; k<=orderLPC; k++)
                sum += double(-1.0*(m-k)/m)*A[k]*C[m-k];

            C[m]=sum;
        }
    }
}

void logo_cepsLPC(const logo_vector& data, const int& orderLPC, const int& orderCepstrum, logo_vector& C) {
    logo_vector A = new_vec(orderLPC + 1);
    logo_real G;
    logo_lpc(data,orderLPC,A,G);
    logo_lpc2Cepstrum(A,orderLPC,C,orderCepstrum);
}
