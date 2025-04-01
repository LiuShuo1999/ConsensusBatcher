#ifndef __RS_H_
#define __RS_H_


#define GF_BITS  8  /* code over GF(2**GF_BITS) - change to suit */
#define GF_SIZE ((1 << GF_BITS) - 1)    /* powers of \alpha */
typedef unsigned char gf;

extern gf gf_exp[2*GF_SIZE];    /* index->poly form conversion table    */
extern int gf_log[GF_SIZE + 1]; /* Poly->index form conversion table    */
extern gf inverse[GF_SIZE+1];   /* inverse of field elem.       */
                /* inv[\alpha**i]=\alpha**(GF_SIZE-i-1) */

void addmul(gf *dst, gf *src, gf c, int sz);
gf galMultiply(gf a, gf b);
gf galExp(gf a, gf n);


gf* sub_matrix(gf* matrix, int rmin, int cmin, int rmax, int cmax,  int nrows, int ncols);
gf* multiply1(gf *a, int ar, int ac, gf *b, int br, int bc);


int invert_mat(gf *src, int k);



/* use small value to save memory */
#ifndef DATA_SHARDS_MAX
#define DATA_SHARDS_MAX (255)
#endif

/* use other memory allocator */
#ifndef RS_MALLOC
#define RS_MALLOC(x)    malloc(x)
#endif

#ifndef RS_FREE
#define RS_FREE(x)      free(x)
#endif

#ifndef RS_CALLOC
#define RS_CALLOC(n, x) calloc(n, x)
#endif

typedef struct _reed_solomon {
    int data_shards;
    int parity_shards;
    int shards;
    unsigned char* m;
    unsigned char* parity;
} reed_solomon;

/**
 * MUST initial one time
 * */
void fec_init(void);

reed_solomon* reed_solomon_new(int data_shards, int parity_shards);
void reed_solomon_release(reed_solomon* rs);

/**
 * encode one shard
 * input:
 * rs
 * data_blocks[rs->data_shards][block_size]
 * fec_blocks[rs->data_shards][block_size]
 * */
int reed_solomon_encode(reed_solomon* rs,
        unsigned char** data_blocks,
        unsigned char** fec_blocks,
        int block_size);


/**
 * decode one shard
 * input:
 * rs
 * original data_blocks[rs->data_shards][block_size]
 * dec_fec_blocks[nr_fec_blocks][block_size]
 * fec_block_nos: fec pos number in original fec_blocks
 * erased_blocks: erased blocks in original data_blocks
 * nr_fec_blocks: the number of erased blocks
 * */
int reed_solomon_decode(reed_solomon* rs,
        unsigned char **data_blocks,
        int block_size,
        unsigned char **dec_fec_blocks,
        unsigned int *fec_block_nos,
        unsigned int *erased_blocks,
        int nr_fec_blocks);

/**
 * encode a big size of buffer
 * input:
 * rs
 * nr_shards: assert(0 == nr_shards % rs->data_shards)
 * shards[nr_shards][block_size]
 * */
int reed_solomon_encode2(reed_solomon* rs, unsigned char** shards, int nr_shards, int block_size);

/**
 * reconstruct a big size of buffer
 * input:
 * rs
 * nr_shards: assert(0 == nr_shards % rs->data_shards)
 * shards[nr_shards][block_size]
 * marks[nr_shards] marks as errors
 * */
int reed_solomon_reconstruct(reed_solomon* rs, unsigned char** shards, unsigned char* marks, int nr_shards, int block_size);
#endif

