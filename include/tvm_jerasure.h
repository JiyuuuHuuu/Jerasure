#ifndef tvm_jerasure__
#define tvm_jerasure__

#ifdef __cplusplus
extern "C" {
#endif

extern void tvm_ec_bitmatrix_encode(int k, int m, int w, int *bitmatrix,
                            char **data_ptrs, char **coding_ptrs, int size, int packetsize);

#ifdef __cplusplus
}
#endif

#endif