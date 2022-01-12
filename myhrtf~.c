#include "m_pd.h"
#include <fftw3.h>
#include "libEigenHRTF/libEigenHRTF.h"

static t_class *myhrtf_tilde_class;

typedef struct _myhrtf_tilde{

  t_object x_obj;
  t_float x_f;

/*define inlet and outlet*/

  //*x_in1 is a mono signal. This value is already defined.
  t_inlet *x_in2;//input azimuth.
  t_inlet *x_in3;//input elevation.
  t_inlet *x_in4;//input distance.
  t_inlet *x_in5;//input pinna value (0 or 1). 

  t_outlet *x_l_out;//output left signal.
  t_outlet *x_r_out;//output right signal.

  int fft_size;//fft size (n length + filter length - 1)

/*They are real sounds.*/

  float *s_in; //input mono signal
  float *l_out;//output left signal
  float *r_out;//output right signal

/*They are complex sounds in fftw.*/

  fftwf_complex *fftw_in;//complex list of s_in[]
  fftwf_complex *fftw_l_out;//complex list of l_out[]
  fftwf_complex *fftw_r_out;//complex list of r_out[]


/*They are fftw plans.*/

  fftwf_plan ffftw_plan;//performed Fourier Transform from s_in to fftw_in
  fftwf_plan iffftw_l_plan;//performed Inverse Fourier Transform from fftw_l_out to l_out
  fftwf_plan iffftw_r_plan;//performed Inverse Fourier Transform from fftw_r_out to r_out


/*EigenHRTF value*/

  char* path;//This must be written with an absolute path.
  struct interpolation* interpol; //Input the offline barycentric interpolation.
  struct Eigen* eigen; //Input the eigen decomposition.
  int sample_rate;//44100
  int pinna;//large pinna:1  small pinna:0
  float complex* filter_l;//Apply a filter to the signal left output.
  float complex* filter_r;//Apply a filter to the signal right output.
  int delays[2];//delays[0]: left signal delay, delays[1]: right signal delay
  int idxs[2];//idxs[0]:original database, idxs[1]: interpolated database


/*spherical-coordinate value*/

  float azimuth;
  float elevation;
  float distance;

/*storage buffer*/

  t_sample* l_buffer;
  t_sample* r_buffer;

} t_myhrtf_tilde;



static t_int *myhrtf_tilde_perform(t_int *w){
  t_myhrtf_tilde *x = (t_myhrtf_tilde *)(w[1]);
  t_sample *x_in1 = (t_sample *)(w[2]);
  t_sample *l_out = (t_sample *)(w[3]);
  t_sample *r_out = (t_sample *)(w[4]);
  int n = (int)(w[5]);

  int i;

  /*copy x->s_in from x_in*/
  memset(x->s_in,0.0f,sizeof(x->s_in));
  memcpy(x->s_in,x_in1,n*sizeof(float));

  /*execute　ffftw*/
  fftwf_execute(x->ffftw_plan);

  /*get filter_l, filter_r and delays*/
  get_filters(x->eigen, x->interpol, x->distance, x->elevation, x->azimuth, x->pinna, x->filter_l, x->filter_r, x->delays, x->idxs);

  /*manipulation: when a = w+xj and b = y+zj, a * b = wy - xz + (wz + xy)j */
  for(i=0;i<x->eigen->n_bins;i++){
    x->fftw_l_out[i][0] = ( x->fftw_in[i][0] * crealf(x->filter_l[i])  -  x->fftw_in[i][1] * cimagf(x->filter_l[i]) );
    x->fftw_l_out[i][1] = ( x->fftw_in[i][0] * cimagf(x->filter_l[i])  +  x->fftw_in[i][1] * crealf(x->filter_l[i]) );

    x->fftw_r_out[i][0] = ( x->fftw_in[i][0] * crealf(x->filter_r[i])  -  x->fftw_in[i][1] * cimagf(x->filter_r[i]) );
    x->fftw_r_out[i][1] = ( x->fftw_in[i][0] * cimagf(x->filter_r[i])  +  x->fftw_in[i][1] * crealf(x->filter_r[i]) );
 }
  /*execute　ifftw*/
  fftwf_execute(x->iffftw_l_plan);
  fftwf_execute(x->iffftw_r_plan);

/*input out signal to buffer*/
  for(i=0;i<x->fft_size;i++){
    x->l_buffer[i]=x->l_buffer[i]+x->l_out[i]/x->fft_size;
    x->r_buffer[i]=x->r_buffer[i]+x->r_out[i]/x->fft_size;
  }

/*output contents and shift buffer information*/
  for(i=0;i<x->fft_size;i++){
    if(i<n){
        r_out[i] = x->r_buffer[i];
        l_out[i] = x->l_buffer[i];
    }
        x->r_buffer[i] = x->r_buffer[i + n];
        x->l_buffer[i] = x->l_buffer[i + n];
  }

  return (w + 6);
}


/*Work to be done when dsp is executed*/
static void myhrtf_tilde_dsp(t_myhrtf_tilde *x, t_signal **sp){
  dsp_add(myhrtf_tilde_perform,
          5,
          x,
          sp[0]->s_vec,
          sp[1]->s_vec,
          sp[2]->s_vec,
          sp[0]->s_n);

  x->sample_rate = sp[0]->s_sr;

  /*read EigenHRTFs information*/
  x->path = MAKEFILE_DIR;//get a absolute　path of EigenHRTFs

  read_interpolation(x->interpol,x->path);//read informations of interpolation
  read_eigen(x->eigen, x->sample_rate, x->path);//read informations of EigenHRTFs

  x->fft_size =x->eigen->n_bins+(sp[0]->s_n)-1;//fft size = n length + filter length - 1

  post("path = %s",x->path);
  post("block size = %d",sp[0]->s_n);
  post("filter size of EigenHRTFs = %d",x->eigen->n_bins);

  x->filter_l = malloc(sizeof(float complex)*(x->eigen->n_bins));
  x->filter_r = malloc(sizeof(float complex)*(x->eigen->n_bins));

  x->l_buffer = malloc(sizeof(t_sample)*x->fft_size);
  x->r_buffer = malloc(sizeof(t_sample)*x->fft_size);
  memset(x->l_buffer,0.0f,sizeof(x->l_buffer));
  memset(x->r_buffer,0.0f,sizeof(x->r_buffer));

/*create fftwf_plans*/

  /*ffftw_plan is changing from time domain to frequency domain.*/
  x->s_in = fftwf_alloc_real(x->fft_size);
  x->fftw_in = fftwf_alloc_complex(x->fft_size);
  x->ffftw_plan = fftwf_plan_dft_r2c_1d(x->fft_size, x->s_in, x->fftw_in, FFTW_ESTIMATE);

  /*ifftw_l_plan is changing from frequency domain to time domain.*/
  x->fftw_l_out = fftwf_alloc_complex(x->fft_size);
  x->l_out = fftwf_alloc_real(x->fft_size);
  x->iffftw_l_plan = fftwf_plan_dft_c2r_1d(x->fft_size, x->fftw_l_out, x->l_out, FFTW_ESTIMATE);

  /*ifftw_r_plan is changing from frequency domain to time domain.*/
  x->fftw_r_out = fftwf_alloc_complex(x->fft_size);
  x->r_out = fftwf_alloc_real(x->fft_size);
  x->iffftw_r_plan = fftwf_plan_dft_c2r_1d(x->fft_size, x->fftw_r_out, x->r_out, FFTW_ESTIMATE);

}



static void *myhrtf_tilde_new(void){
  t_myhrtf_tilde *x = (t_myhrtf_tilde *)pd_new(myhrtf_tilde_class);

  /* create a new passive inlet for the mixing-factor */
  x->x_in2 = floatinlet_new(&x->x_obj, &x->azimuth);
  x->x_in3 = floatinlet_new(&x->x_obj, &x->elevation);
  x->x_in4 = floatinlet_new(&x->x_obj, &x->distance);
  x->x_in5 = floatinlet_new(&x->x_obj, &x->pinna);

  /* create a new signal-outlet */
  x->x_l_out = outlet_new(&x->x_obj, &s_signal);
  x->x_r_out = outlet_new(&x->x_obj, &s_signal);

  /*allocated interpol and eigen*/
  x->interpol = malloc(sizeof(struct interpolation));
  x->eigen = malloc(sizeof(struct Eigen));

  return (void *)x;
}



void myhrtf_tilde_free(t_myhrtf_tilde *x){

  /*inlet and outlet free*/

  inlet_free(x->x_in2);
  inlet_free(x->x_in3);
  inlet_free(x->x_in4);
  inlet_free(x->x_in5);
  outlet_free(x->x_r_out);
  outlet_free(x->x_l_out);

  /*fftw free*/

  fftwf_free(x->s_in);
  fftwf_free(x->fftw_in);
  fftwf_destroy_plan(x->ffftw_plan);
  fftwf_free(x->fftw_l_out);
  fftwf_free(x->l_out);
  fftwf_destroy_plan(x->iffftw_l_plan);
  fftwf_free(x->fftw_r_out);
  fftwf_free(x->r_out);
  fftwf_destroy_plan(x->iffftw_r_plan);

  /*malloc free*/

  free(x->interpol);
  free(x->eigen);
  free(x->filter_l);
  free(x->filter_r);
  free(x->l_buffer);
  free(x->r_buffer);

}

void myhrtf_tilde_setup(void){
  myhrtf_tilde_class = class_new(gensym("myhrtf~"),
                                 (t_newmethod)myhrtf_tilde_new,
                                 (t_method)myhrtf_tilde_free,
                                 sizeof(t_myhrtf_tilde),
                                 CLASS_DEFAULT,
                                 0);

  CLASS_MAINSIGNALIN(myhrtf_tilde_class, t_myhrtf_tilde, x_f);
  class_addmethod(myhrtf_tilde_class, (t_method)myhrtf_tilde_dsp, gensym("dsp"), 0);
}