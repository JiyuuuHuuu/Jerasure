/* *
 * Copyright (c) 2014, James S. Plank and Kevin Greenan
 * All rights reserved.
 *
 * Jerasure - A C/C++ Library for a Variety of Reed-Solomon and RAID-6 Erasure
 * Coding Techniques
 *
 * Revision 2.0: Galois Field backend now links to GF-Complete
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 *  - Neither the name of the University of Tennessee nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* Jerasure's authors:

   Revision 2.x - 2014: James S. Plank and Kevin M. Greenan.
   Revision 1.2 - 2008: James S. Plank, Scott Simmerman and Catherine D. Schuman.
   Revision 1.0 - 2007: James S. Plank.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <string.h>
#include <gf_rand.h>
#include "jerasure.h"

#define talloc(type, num) (type *) malloc(sizeof(type)*(num))
#define TIMING_REPEAT 80

static void usage(char *s)
{
  fprintf(stderr, "usage: jerasure_06 k m w packetsize seed\n");
  fprintf(stderr, "Does a simple Cauchy Reed-Solomon coding example in GF(2^w).\n");
  fprintf(stderr, "       \n");
  fprintf(stderr, "       k+m must be < 2^w.  Packetsize must be a multiple of sizeof(long)\n");
  fprintf(stderr, "       It sets up a Cauchy generator matrix and encodes k devices of w*packetsize bytes.\n");
  fprintf(stderr, "       After that, it decodes device 0 by using jerasure_make_decoding_bitmatrix()\n");
  fprintf(stderr, "       and jerasure_bitmatrix_dotprod(). Verbose in range [0, 2]\n");
  fprintf(stderr, "       0: brief output, 1: detailed output, 2: timing info only\n");
  fprintf(stderr, "       \n");
  fprintf(stderr, "This demonstrates: jerasure_bitmatrix_encode()\n");
  fprintf(stderr, "                   jerasure_bitmatrix_decode()\n");
  fprintf(stderr, "                   jerasure_print_bitmatrix()\n");
  fprintf(stderr, "                   jerasure_make_decoding_bitmatrix()\n");
  fprintf(stderr, "                   jerasure_bitmatrix_dotprod()\n");
  if (s != NULL) fprintf(stderr, "\n%s\n\n", s);
  exit(1);
}

static void print_array(char **ptrs, int ndevices, int size, int packetsize, char *label)
{
  int i, j, x;
  unsigned char *up;

  printf("<center><table border=3 cellpadding=3><tr><td></td>\n");

  for (i = 0; i < ndevices; i++) printf("<td align=center>%s%x</td>\n", label, i);
  printf("</tr>\n");
  printf("<td align=right><pre>");
  for (j = 0; j < size/packetsize; j++) printf("Packet %d\n", j);
  printf("</pre></td>\n");
  for (i = 0; i < ndevices; i++) {
    printf("<td><pre>");
    up = (unsigned char *) ptrs[i];
    for (j = 0; j < size/packetsize; j++) {
      for (x = 0; x < packetsize; x++) {
        if (x > 0 && x%4 == 0) printf(" ");
        printf("%02x", up[j*packetsize+x]);
      }
      printf("\n");
    }
    printf("</td>\n");
  }
  printf("</tr></table></center>\n");
}

unsigned long long current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    unsigned long long microseconds = te.tv_sec*1000000LL + te.tv_usec; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return microseconds;
}

int main(int argc, char **argv)
{
  int k, w, i, j, m, psize, verbose;
  int *matrix, *bitmatrix;
  char **data, **coding, **recover;
  int *erasures, *erased;
  // int *decoding_matrix, *dm_ids;
  uint32_t seed;
  unsigned long long start, duration;
  
  if (argc != 7) usage(NULL);
  if (sscanf(argv[1], "%d", &k) == 0 || k <= 0) usage("Bad k");
  if (sscanf(argv[2], "%d", &m) == 0 || m <= 0) usage("Bad m");
  if (sscanf(argv[3], "%d", &w) == 0 || w <= 0 || w > 32) usage("Bad w");
  if (w < 30 && (k+m) > (1 << w)) usage("k + m is too big");
  if (sscanf(argv[4], "%d", &psize) == 0 || psize <= 0) usage("Bad packetsize");
  if(psize%sizeof(long) != 0) usage("Packetsize must be multiple of sizeof(long)");
  if (sscanf(argv[5], "%d", &seed) == 0) usage("Bad seed");
  if (sscanf(argv[6], "%d", &verbose) == 0) usage("Verbose in range [0, 2]");

  MOA_Seed(seed);
  matrix = talloc(int, m*k);
  for (i = 0; i < m; i++) {
    for (j = 0; j < k; j++) {
      matrix[i*k+j] = galois_single_divide(1, i ^ (m + j), w);
    }
  }
  bitmatrix = jerasure_matrix_to_bitmatrix(k, m, w, matrix);

  if (verbose != 2){
    printf("<HTML><TITLE>jerasure_tvm");
    for (i = 1; i < argc; i++) printf(" %s", argv[i]);
    printf("</TITLE>\n");
    printf("<h3>jerasure_tvm");
    for (i = 1; i < argc; i++) printf(" %s", argv[i]);
    printf("</h3>\n");
  } else {
    start = current_timestamp();
    for (int repeat = 0; repeat < TIMING_REPEAT; repeat++) {
      for (i = 0; i < m; i++) {
        for (j = 0; j < k; j++) {
          matrix[i*k+j] = galois_single_divide(1, i ^ (m + j), w);
        }
      }
    }
    duration = current_timestamp() - start;
    printf("%lf ", (double)duration/TIMING_REPEAT);

    bitmatrix = jerasure_matrix_to_bitmatrix(k, m, w, matrix);

    start = current_timestamp();
    for (int repeat = 0; repeat < TIMING_REPEAT; repeat++) {
      bitmatrix = jerasure_matrix_to_bitmatrix(k, m, w, matrix);
    }
    duration = current_timestamp() - start;
    printf("%lf ", (double)duration/TIMING_REPEAT);
  }

  if (verbose == 1) {
    printf("<hr>\n");
    printf("Last (m * w) rows of the Generator Matrix: (G^T):\n<pre>\n");
    jerasure_print_bitmatrix(bitmatrix, w*m, w*k, w);
    printf("</pre><hr>\n");
  }

  data = talloc(char *, k);
  for (i = 0; i < k; i++) {
    data[i] = talloc(char, psize*w);
    MOA_Fill_Random_Region(data[i], psize*w);
  }

  coding = talloc(char *, m);
  for (i = 0; i < m; i++) {
    coding[i] = talloc(char, psize*w);
  }

  tvm_ec_bitmatrix_encode(k, m, w, bitmatrix, data, coding, w*psize, psize);

  if (verbose != 2) {
    printf("Encoding Complete\n\n");
  } else {
    start = current_timestamp();
    for (int repeat = 0; repeat < TIMING_REPEAT; repeat++) {
      tvm_ec_bitmatrix_encode(k, m, w, bitmatrix, data, coding, w*psize, psize);
    }
    duration = current_timestamp() - start;
    printf("%lf ", (double)duration/TIMING_REPEAT);
  }
  if (k < 16 && m < 8 && psize < 64 && verbose == 1) {
    printf("Here is the state of the system\n\n");
    printf("<p>\n");
    print_array(data, k, psize*w, psize, "D");
    printf("<p>\n");
    print_array(coding, m, psize*w, psize, "C");
    printf("<hr>\n");
  }

  erasures = talloc(int, (m+1));
  erased = talloc(int, (k+m));
  for (i = 0; i < m+k; i++) erased[i] = 0;
  for (i = 0; i < m; ) {
    erasures[i] = MOA_Random_W(w, 1)%(k);
    if (erased[erasures[i]] == 0) {
      erased[erasures[i]] = 1;
      bzero((erasures[i] < k) ? data[erasures[i]] : coding[erasures[i]-k], psize*w);
      i++;
    }
  }
  erasures[i] = -1;

  if (verbose != 2) {
    printf("Erased %d random devices:", m);
    for (i = 0; erasures[i] != -1; i++) {
      printf(" %c%x", ((erasures[i] < k) ? 'D' : 'C'), (erasures[i] < k ? erasures[i] : erasures[i]-k));
    }
    printf("\n");
  }

  recover = tvm_ec_bitmatrix_decode(k, m, w, bitmatrix, erasures, data, coding, w*psize, psize, 0);

  if (verbose != 2) {
    printf("Dncoding Complete\n\n");
  } else {
    start = current_timestamp();
    for (int repeat = 0; repeat < TIMING_REPEAT; repeat++) {
      tvm_ec_bitmatrix_decode(k, m, w, bitmatrix, erasures, data, coding, w*psize, psize, 0);
    }
    duration = current_timestamp() - start;
    printf("%lf ", (double)duration/TIMING_REPEAT);
  }

  if (k < 16 && m < 8 && psize < 64 && verbose == 1) {
    printf("Here is the recoverd data\n\n");
    printf("<p>\n");
    print_array(recover, m, psize*w, psize, "C");
    printf("<hr>\n");
  }
  printf("\n");
  return 0;
}
