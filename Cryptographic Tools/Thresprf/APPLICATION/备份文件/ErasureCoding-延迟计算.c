#include "mw1268_app.h"
#include "mw1268_cfg.h"
#include "usart3.h"
#include "string.h"
#include "led.h"
#include "delay.h"
#include "tendermint2.h"
#include "core.h"
#include "bls_BN254.h"
#include "rs.h"


//u32 TIM5_Exceed_Times;
u32 TIM9_Exceed_Times;
//u32 TIM10_Exceed_Times;

////////定向传输
u32 obj_addr;//记录用户输入目标地址
u8 obj_chn;//记录用户输入目标信道
const struct uECC_Curve_t * curves[5];

u32 TIM2_Exceed_Times;
u8 Send_Data_buff[800] = {0};


void init(){
	while(LoRa_Init())//初始化LORA模块
	{
		delay_ms(300);
	}
	LoRa_CFG.chn = Data_channel;
	LoRa_CFG.tmode = LORA_TMODE_PT;
	LoRa_CFG.addr = 0;
	LoRa_CFG.power = LORA_TPW_9DBM;
	LoRa_CFG.wlrate = LORA_RATE_62K5;
	LoRa_CFG.lbt = LORA_LBT_ENABLE;
	LoRa_Set();
}

#include <stdio.h>
#include <time.h>
/*********************************Erasure Code*******************************************/

//void print_buf(gf* buf, char *fmt, size_t len) {
//    size_t i = 0;
//    while(i < len) {
//        //printf(fmt, buf[i]);
//		sprintf((char*)Send_Data_buff[i],"test_galois: Error");
//        i++;
//        if((i % 16) == 0) {
//			LoRa_SendData(Send_Data_buff);
//            printf("\n");
//        }
//    }
//    printf("\n");
//}


void test_galois(void) {
    printf("%s:\n", __FUNCTION__);

    //copy from golang rs version
//    assert(galMultiply(3, 4) == 12);
//    assert(galMultiply(7, 7) == 21);
//    assert(galMultiply(23, 45) == 41);

    if(galMultiply(3, 4) != 12) {sprintf((char*)Send_Data_buff,"test_galois: Error");LoRa_SendData(Send_Data_buff);return;}
	if(galMultiply(7, 7) != 21) {sprintf((char*)Send_Data_buff,"test_galois: Error");LoRa_SendData(Send_Data_buff);return;}
	if(galMultiply(23, 45) != 41) {sprintf((char*)Send_Data_buff,"test_galois: Error");LoRa_SendData(Send_Data_buff);return;}
	
	
	gf in[] = {0, 1, 2, 3, 4, 5, 6, 10, 50, 100, 150, 174, 201, 255, 99, 32, 67, 85};
	gf out[sizeof(in)/sizeof(gf)] = {0};
	gf expect[] = {0x0, 0x19, 0x32, 0x2b, 0x64, 0x7d, 0x56, 0xfa, 0xb8, 0x6d, 0xc7, 0x85, 0xc3, 0x1f, 0x22, 0x7, 0x25, 0xfe};
	gf expect2[] = {0x0, 0xb1, 0x7f, 0xce, 0xfe, 0x4f, 0x81, 0x9e, 0x3, 0x6, 0xe8, 0x75, 0xbd, 0x40, 0x36, 0xa3, 0x95, 0xcb};
	int rlt;
	addmul(out, in, 25, sizeof(int)/sizeof(gf));
	rlt = memcmp(out, expect, sizeof(int)/sizeof(gf));
	//assert(0 == rlt);
	if(0 != rlt) {sprintf((char*)Send_Data_buff,"test_galois: Error");LoRa_SendData(Send_Data_buff);return;}
		
		
	memset(out, 0, sizeof(in)/sizeof(gf));
	addmul(out, in, 177, sizeof(in)/sizeof(gf));
	rlt = memcmp(out, expect2, sizeof(int)/sizeof(gf));
	//assert(0 == rlt);
    if(0 != rlt) {sprintf((char*)Send_Data_buff,"test_galois: Error");LoRa_SendData(Send_Data_buff);return;}

//    assert(galExp(2,2) == 4);
//    assert(galExp(5,20) == 235);
//    assert(galExp(13,7) == 43);
	if(galExp(2,2) != 4) {sprintf((char*)Send_Data_buff,"test_galois: Error");LoRa_SendData(Send_Data_buff);return;}
	if(galExp(5,20) != 235) {sprintf((char*)Send_Data_buff,"test_galois: Error");LoRa_SendData(Send_Data_buff);return;}
	if(galExp(13,7) != 43) {sprintf((char*)Send_Data_buff,"test_galois: Error");LoRa_SendData(Send_Data_buff);return;}
	
	sprintf((char*)Send_Data_buff,"test_galois: Success");LoRa_SendData(Send_Data_buff);
}

void test_sub_matrix(void) {
    int r, c, ptr, nrows = 10, ncols = 20;
    gf* m1 = (gf*)RS_MALLOC(nrows * ncols);
    gf *test1;

    printf("%s:\n", __FUNCTION__);

    ptr = 0;
    for(r = 0; r < nrows; r++) {
        for(c = 0; c < ncols; c++) {
            m1[ptr] = ptr;
            ptr++;
        }
    }
    test1 = sub_matrix(m1, 0, 0, 3, 4, nrows, ncols);
    for(r = 0; r < 3; r++) {
        for(c = 0; c < 4; c++) {
			
            //assert(test1[r*4 + c] == (r*ncols + c));
			if(test1[r*4 + c] != (r*ncols + c)) {sprintf((char*)Send_Data_buff,"test_sub_matrix: Error");LoRa_SendData(Send_Data_buff);return;}
        }
    }
    free(test1);

    test1 = sub_matrix(m1, 3, 2, 7, 9, nrows, ncols);
    for(r = 0; r < (7-3); r++) {
        for(c = 0; c < (9-2); c++) {
            //assert(test1[r*(9-2) + c] == ((r+3)*ncols + (c+2)));
			if(test1[r*(9-2) + c] != ((r+3)*ncols + (c+2))) {sprintf((char*)Send_Data_buff,"test_sub_matrix: Error");LoRa_SendData(Send_Data_buff);return;}

        }
    }

    free(m1);
	sprintf((char*)Send_Data_buff,"test_sub_matrix: Success");LoRa_SendData(Send_Data_buff);
}

void test_multiply(void) {
    gf a[] = {1,2,3,4};
    gf b[] = {5,6,7,8};
    gf exp[] = {11,22,19,42};
    gf *out;
    int rlt;

    printf("%s:\n", __FUNCTION__);

    out = multiply1(a, 2, 2, b, 2, 2);
    rlt = memcmp(out, exp, 4);
    //assert(0 == rlt);
	if(0 != rlt) {sprintf((char*)Send_Data_buff,"test_multiply: Error");LoRa_SendData(Send_Data_buff);return;}
	
	sprintf((char*)Send_Data_buff,"test_multiply: Success");LoRa_SendData(Send_Data_buff);
}

void test_inverse(void) {
    printf("%s:\n", __FUNCTION__);
    {
        gf a[] = {56, 23, 98, 3, 100, 200, 45, 201, 123};
        gf ae[] = {175, 133, 33, 130, 13, 245, 112, 35, 126};
        int rlt = invert_mat(a, 3);
        //assert(0 == rlt);
		if(0 != rlt) {sprintf((char*)Send_Data_buff,"test_inverse: Error");LoRa_SendData(Send_Data_buff);return;}
        rlt = memcmp(a, ae, 3*3);
        //assert(0 == rlt);
		if(0 != rlt) {sprintf((char*)Send_Data_buff,"test_inverse: Error");LoRa_SendData(Send_Data_buff);return;}
    }

    {
        gf a[] = {  1, 0, 0, 0, 0,
                    0, 1, 0, 0, 0,
                    0, 0, 0, 1, 0,
                    0, 0, 0, 0, 1,
                    7, 7, 6, 6, 1};
        gf ae[] = {1, 0, 0, 0, 0,
                   0, 1, 0, 0, 0,
                   123, 123, 1, 122, 122,
                   0, 0, 1, 0, 0,
                   0, 0, 0, 1, 0};
        int rlt = invert_mat(a, 5);
        //assert(0 == rlt);
		if(0 != rlt) {sprintf((char*)Send_Data_buff,"test_inverse: Error");LoRa_SendData(Send_Data_buff);return;}
        rlt = memcmp(a, ae, 5*5);
        //assert(0 == rlt);
		if(0 != rlt) {sprintf((char*)Send_Data_buff,"test_inverse: Error");LoRa_SendData(Send_Data_buff);return;}
    }

    {
        /* error matrix */
        gf a[] = {4,2,12,6};
        int rlt = invert_mat(a, 2);
        //assert(0 != rlt);
		if(0 == rlt) {sprintf((char*)Send_Data_buff,"test_inverse: Error");LoRa_SendData(Send_Data_buff);return;}
    }
	
	sprintf((char*)Send_Data_buff,"test_inverse: Success");LoRa_SendData(Send_Data_buff);
}

unsigned char* test_create_random(reed_solomon *rs, int data_size, int block_size) {
    //struct timeval tv;
    unsigned char* data;
    int i, n, seed, nr_blocks;

    //gettimeofday(&tv, 0);
    //seed = tv.tv_sec ^ tv.tv_usec;
    //srandom(seed);

    nr_blocks = (data_size+block_size-1)/block_size;
    nr_blocks = ((nr_blocks + rs->data_shards - 1)/ rs->data_shards) * rs->data_shards;
    n = nr_blocks / rs->data_shards;
    nr_blocks += n * rs->parity_shards;

    data = (unsigned char *) malloc(nr_blocks * block_size);
    for(i = 0; i < data_size; i++) {
        data[i] = (unsigned char)(RNG_Get_RandomNum() % 255);
    }
    memset(data + data_size, 0, nr_blocks*block_size - data_size);

    return data;
}

int test_create_encoding(
        reed_solomon *rs,
        unsigned char *data,
        int data_size,
        int block_size
        ) {
    unsigned char **data_blocks;
    int data_shards, parity_shards;
    int i, n, nr_shards, nr_blocks, nr_fec_blocks;

    data_shards = rs->data_shards;
    parity_shards = rs->parity_shards;
    nr_blocks = (data_size+block_size-1)/block_size;
    nr_blocks = ((nr_blocks+data_shards-1)/data_shards) * data_shards;
    n = nr_blocks / data_shards;
    nr_fec_blocks = n * parity_shards;
    nr_shards = nr_blocks + nr_fec_blocks;

    data_blocks = (unsigned char**)malloc(nr_shards * sizeof(unsigned char*));
    for(i = 0; i < nr_shards; i++) {
        data_blocks[i] = data + i*block_size;
    }

    n = reed_solomon_encode2(rs, data_blocks, nr_shards, block_size);
    free(data_blocks);

    return n;
}


void test_one_encoding(void) {
    reed_solomon *rs;
    unsigned char* data;
    int block_size = 500;
    int data_size = 10*block_size;
    int err;

    printf("%s:\n", __FUNCTION__);

    rs = reed_solomon_new(10, 3);
    data = test_create_random(rs, data_size, block_size);
    err = test_create_encoding(rs, data, data_size, block_size);

    free(data);
    reed_solomon_release(rs);

    //assert(0 == err);
	if(0 != err) {sprintf((char*)Send_Data_buff,"test_one_encoding: Error");LoRa_SendData(Send_Data_buff);return;}
	
	sprintf((char*)Send_Data_buff,"test_one_encoding: Success");LoRa_SendData(Send_Data_buff);
}

int test_data_decode(
        reed_solomon *rs,
        unsigned char *data,
        int data_size,
        int block_size,
        int *erases,
        int erase_count) {
    unsigned char **data_blocks;
    unsigned char *zilch;
    int data_shards, parity_shards;
    int i, j, n, nr_shards, nr_blocks, nr_fec_blocks;

    data_shards = rs->data_shards;
    parity_shards = rs->parity_shards;
    nr_blocks = (data_size+block_size-1)/block_size;
    nr_blocks = ((nr_blocks+data_shards-1)/data_shards) * data_shards;
    n = nr_blocks / data_shards;
    nr_fec_blocks = n * parity_shards;
    nr_shards = nr_blocks + nr_fec_blocks;

    data_blocks = (unsigned char**)malloc(nr_shards * sizeof(unsigned char*));
    for(i = 0; i < nr_shards; i++) {
        data_blocks[i] = data + i*block_size;
    }

    zilch = (unsigned char*)calloc(1, nr_shards);
    for(i = 0; i < erase_count; i++) {
        j = erases[i];
        memset(data + j*block_size, 137, block_size);
        zilch[j] = 1; //mark as erased
    }

    n = reed_solomon_reconstruct(rs, data_blocks, zilch, nr_shards, block_size);
    free(data_blocks);
    free(zilch);

    return n;
}

int test_one_decoding_13(int *erases, int erase_count) {
    reed_solomon *rs;
    unsigned char *data, *origin;
    int block_size = 500;
    int data_size = 10*block_size;
    int err, err2;

    rs = reed_solomon_new(10, 3);
    data = test_create_random(rs, data_size, block_size);
    err = test_create_encoding(rs, data, data_size, block_size);
    //assert(0 == err);
	if(0 != err) {sprintf((char*)Send_Data_buff,"test_one_decoding_13: Error");LoRa_SendData(Send_Data_buff);return 1;}
	
    origin = (unsigned char*)malloc(data_size);
    memcpy(origin, data, data_size);

    err = test_data_decode(rs, data, data_size, block_size, erases, erase_count);
    if(0 == err) {
        err2 = memcmp(origin, data, data_size);
        //assert(0 == err2);
		if(0 != err2) {sprintf((char*)Send_Data_buff,"test_one_decoding_13: Error");LoRa_SendData(Send_Data_buff);return 1;}
    } else {
        //failed here
        err2 = memcmp(origin, data, data_size);
        //assert(0 != err2);
		if(0 != err2) {sprintf((char*)Send_Data_buff,"test_one_decoding_13: Error");LoRa_SendData(Send_Data_buff);return 1;}
    }

    free(data);
    free(origin);
    reed_solomon_release(rs);

    return err;
}

void test_one_decoding(void) {
    printf("%s:\n", __FUNCTION__);

    {
        int erases[] = {0};
        int err;

        // lost nothing
        err = test_one_decoding_13(erases, 0);
        //assert(0 == err);
		if(0 != err) {sprintf((char*)Send_Data_buff,"test_one_decoding: Error");LoRa_SendData(Send_Data_buff);return;}
    }

    {
        int erases[] = {0};
        int erases_count = sizeof(erases)/sizeof(int);
        int err;

        // lost only one
        err = test_one_decoding_13(erases, erases_count);
        //assert(0 == err);
		if(0 != err) {sprintf((char*)Send_Data_buff,"test_one_decoding: Error");LoRa_SendData(Send_Data_buff);return;}
        erases[0] = 5;
        err = test_one_decoding_13(erases, erases_count);
        //assert(0 == err);
		if(0 != err) {sprintf((char*)Send_Data_buff,"test_one_decoding: Error");LoRa_SendData(Send_Data_buff);return;}
        erases[0] = 9;
        err = test_one_decoding_13(erases, erases_count);
        //assert(0 == err);
		if(0 != err) {sprintf((char*)Send_Data_buff,"test_one_decoding: Error");LoRa_SendData(Send_Data_buff);return;}
        erases[0] = 11;
        err = test_one_decoding_13(erases, erases_count);
        //assert(0 == err);
		if(0 != err) {sprintf((char*)Send_Data_buff,"test_one_decoding: Error");LoRa_SendData(Send_Data_buff);return;}
    }

    {
        int erases[] = {0, 1};
        int erases_count = sizeof(erases)/sizeof(int);
        int err;

        // lost two
        err = test_one_decoding_13(erases, erases_count);
        //assert(0 == err);
		if(0 != err) {sprintf((char*)Send_Data_buff,"test_one_decoding: Error");LoRa_SendData(Send_Data_buff);return;}
        erases[0] = 3;
        erases[1] = 7;
        err = test_one_decoding_13(erases, erases_count);
        //assert(0 == err);
		if(0 != err) {sprintf((char*)Send_Data_buff,"test_one_decoding: Error");LoRa_SendData(Send_Data_buff);return;}
        erases[0] = 11;
        erases[1] = 9;
        err = test_one_decoding_13(erases, erases_count);
        //assert(0 == err);
		if(0 != err) {sprintf((char*)Send_Data_buff,"test_one_decoding: Error");LoRa_SendData(Send_Data_buff);return;}
        erases[0] = 11;
        erases[1] = 12;
        err = test_one_decoding_13(erases, erases_count);
        //assert(0 == err);
		if(0 != err) {sprintf((char*)Send_Data_buff,"test_one_decoding: Error");LoRa_SendData(Send_Data_buff);return;}
    }

    {
        int erases[] = {0, 1, 4};
        int erases_count = sizeof(erases)/sizeof(int);
        int err;

        // lost three
        err = test_one_decoding_13(erases, erases_count);
        //assert(0 == err);
		if(0 != err) {sprintf((char*)Send_Data_buff,"test_one_decoding: Error");LoRa_SendData(Send_Data_buff);return;}
        erases[0] = 3;
        erases[1] = 8;
        erases[2] = 7;
        err = test_one_decoding_13(erases, erases_count);
        //assert(0 == err);
		if(0 != err) {sprintf((char*)Send_Data_buff,"test_one_decoding: Error");LoRa_SendData(Send_Data_buff);return;}
        erases[0] = 11;
        erases[1] = 9;
        erases[2] = 1;
        err = test_one_decoding_13(erases, erases_count);
        //assert(0 == err);
		if(0 != err) {sprintf((char*)Send_Data_buff,"test_one_decoding: Error");LoRa_SendData(Send_Data_buff);return;}
        erases[0] = 11;
        erases[1] = 12;
        erases[2] = 9;
        err = test_one_decoding_13(erases, erases_count);
        //assert(0 == err);
		if(0 != err) {sprintf((char*)Send_Data_buff,"test_one_decoding: Error");LoRa_SendData(Send_Data_buff);return;}
        erases[0] = 11;
        erases[1] = 12;
        erases[2] = 9;
        err = test_one_decoding_13(erases, erases_count);
        //assert(0 == err);
		if(0 != err) {sprintf((char*)Send_Data_buff,"test_one_decoding: Error");LoRa_SendData(Send_Data_buff);return;}
        erases[0] = 11;
        erases[1] = 12;
        erases[2] = 10;
        err = test_one_decoding_13(erases, erases_count);
        //assert(0 == err);
		if(0 != err) {sprintf((char*)Send_Data_buff,"test_one_decoding: Error");LoRa_SendData(Send_Data_buff);return;}
    }

    {
        int erases[] = {0, 1, 4, 8};
        int erases_count = sizeof(erases)/sizeof(int);
        int err;

        // lost 4, failed!
        err = test_one_decoding_13(erases, erases_count);
        //assert(0 != err);
		if(0 != err) {sprintf((char*)Send_Data_buff,"test_one_decoding: Error");LoRa_SendData(Send_Data_buff);return;}
        erases[0] = 11;
        erases[1] = 12;
        erases[2] = 10;
        erases[3] = 9;
        err = test_one_decoding_13(erases, erases_count);
        //assert(0 != err);
		if(0 != err) {sprintf((char*)Send_Data_buff,"test_one_decoding: Error");LoRa_SendData(Send_Data_buff);return;}
    }
}

void test_encoding(void) {
    reed_solomon *rs;
    unsigned char *data;
    int block_size = 100;
    //multi shards encoding
    int data_size = 13*block_size*6;
    int err;

    printf("%s:\n", __FUNCTION__);

    rs = reed_solomon_new(10, 3);
    data = test_create_random(rs, data_size, block_size);
    err = test_create_encoding(rs, data, data_size, block_size);

    free(data);
    reed_solomon_release(rs);

    //assert(0 == err);
	if(0 != err) {sprintf((char*)Send_Data_buff,"test_encoding: Error");LoRa_SendData(Send_Data_buff);return;}
	
	sprintf((char*)Send_Data_buff,"test_encoding: Success");LoRa_SendData(Send_Data_buff);return;
}


int test_one_decoding_13_6(int *erases, int erase_count) {
    reed_solomon *rs;
    unsigned char *data, *origin;
    int block_size = 100;
    int data_size = 10*block_size*6;
    int err, err2;

    rs = reed_solomon_new(10, 3);
    data = test_create_random(rs, data_size, block_size);
    err = test_create_encoding(rs, data, data_size, block_size);
    //assert(0 == err);
	if(0 != err) {sprintf((char*)Send_Data_buff,"test_one_decoding_13_6: Error");LoRa_SendData(Send_Data_buff);return 1;}
	
    origin = (unsigned char*)malloc(data_size);
    memcpy(origin, data, data_size);

    err = test_data_decode(rs, data, data_size, block_size, erases, erase_count);
    if(0 == err) {
        err2 = memcmp(origin, data, data_size);
        //assert(0 == err2);
		if(0 != err2) {sprintf((char*)Send_Data_buff,"test_one_decoding_13_6: Error");LoRa_SendData(Send_Data_buff);return 1;}
    } else {
        //failed here
        err2 = memcmp(origin, data, data_size);
        //assert(0 != err2);
		if(0 == err2) {sprintf((char*)Send_Data_buff,"test_one_decoding_13_6: Error");LoRa_SendData(Send_Data_buff);return 1;}
    }

    free(data);
    free(origin);
    reed_solomon_release(rs);

    return err;
}

void test_reconstruct(void) {
#define FEC_START (10*6)
    printf("%s:\n", __FUNCTION__);

    {
        int erases[] = {0};
        int err;

        // lost nothing
        err = test_one_decoding_13_6(erases, 0);
        //assert(0 == err);
		if(0 != err) {sprintf((char*)Send_Data_buff,"test_reconstruct: Error");LoRa_SendData(Send_Data_buff);return;}
    }

    {
        int erases[] = {0, 1, 9, 10+2, 10+4, 10+9};
        int erases_count = sizeof(erases)/sizeof(int);
        int err;

        // shard1 shard2 both lost three 
        err = test_one_decoding_13_6(erases, erases_count);
        //assert(0 == err);
		if(0 != err) {sprintf((char*)Send_Data_buff,"test_reconstruct: Error");LoRa_SendData(Send_Data_buff);return;}
    }

    {
        int erases[] = {0, 9, FEC_START + 1, 10+2, 10+9, FEC_START + 3 + 2};
        int erases_count = sizeof(erases)/sizeof(int);
        int err;

        // shard1 shard2 both lost three, and both lost one in fec
        err = test_one_decoding_13_6(erases, erases_count);
        //assert(0 == err);
		if(0 != err) {sprintf((char*)Send_Data_buff,"test_reconstruct: Error");LoRa_SendData(Send_Data_buff);return;}
    }

    {
        int erases[] = {11, 12, 10, 9};
        int erases_count = sizeof(erases)/sizeof(int);
        int err;

        /* this is ok. not lost 4 but shard1 lost 1, shard2 lost 3, we can reconstruct it */
        err = test_one_decoding_13_6(erases, erases_count);
        //assert(0 == err);
		if(0 != err) {sprintf((char*)Send_Data_buff,"test_reconstruct: Error");LoRa_SendData(Send_Data_buff);return;}
    }

    {
        int erases[] = {0, 1, 4, 8};
        int erases_count = sizeof(erases)/sizeof(int);
        int err;

        // shard1 lost 4, failed!
        err = test_one_decoding_13_6(erases, erases_count);
        //assert(0 != err);
		if(0 == err) {sprintf((char*)Send_Data_buff,"test_reconstruct: Error");LoRa_SendData(Send_Data_buff);return;}
    }

    {
        int erases[] = {10, 11, 14, 18};
        int erases_count = sizeof(erases)/sizeof(int);
        int err;

        // shard2 lost 4, failed!
        err = test_one_decoding_13_6(erases, erases_count);
        //assert(0 != err);
		if(0 == err) {sprintf((char*)Send_Data_buff,"test_reconstruct: Error");LoRa_SendData(Send_Data_buff);return;}
    }

    {
        int erases[] = {0, 1, 4, 8, 10, 11, 14, 18};
        int erases_count = sizeof(erases)/sizeof(int);
        int err;

        // shard1 and shard2 both lost 4, failed!
        err = test_one_decoding_13_6(erases, erases_count);
        //assert(0 != err);
		if(0 == err) {sprintf((char*)Send_Data_buff,"test_reconstruct: Error");LoRa_SendData(Send_Data_buff);return;}
    }

    {
        int erases[] = {11, 12, 10, 9, FEC_START+3+0, FEC_START+3+1, FEC_START+3+2};
        int erases_count = sizeof(erases)/sizeof(int);
        int err;

        err = test_one_decoding_13_6(erases, erases_count);
        //assert(0 != err);
		if(0 == err) {sprintf((char*)Send_Data_buff,"test_reconstruct: Error");LoRa_SendData(Send_Data_buff);return;}
    }
}


//void test_001(void) {
//    reed_solomon* rs = reed_solomon_new(11, 6);
//    print_matrix1(rs->m, rs->data_shards, rs->data_shards);
//    print_matrix1(rs->parity, rs->parity_shards, rs->data_shards);
//    reed_solomon_release(rs);
//}

//void test_002(void) {
//    char text[] = "hello world", output[256];
//    int block_size = 1;
//    int nrDataBlocks = sizeof(text)/sizeof(char) - 1;
//    unsigned char* data_blocks[128];
//    unsigned char* fec_blocks[128];
//    int nrFecBlocks = 6;

//    //decode
//    unsigned int fec_block_nos[128], erased_blocks[128];
//    unsigned char* dec_fec_blocks[128];
//    int nr_fec_blocks;

//    int i;
//    reed_solomon* rs = reed_solomon_new(nrDataBlocks, nrFecBlocks);

//    printf("%s:\n", __FUNCTION__);

//    for(i = 0; i < nrDataBlocks; i++) {
//        data_blocks[i] = (unsigned char*)&text[i];
//    }

//    memset(output, 0, sizeof(output));
//    memcpy(output, text, nrDataBlocks);
//    for(i = 0; i < nrFecBlocks; i++) {
//        fec_blocks[i] = (unsigned char*)&output[i + nrDataBlocks];
//    }

//    reed_solomon_encode(rs, data_blocks, fec_blocks, block_size);
//    print_buf((gf*)output, "%d ", nrFecBlocks+nrDataBlocks);

//    text[1] = 'x';
//    text[3] = 'y';
//    text[4] = 'z';
//    erased_blocks[0] = 4;
//    erased_blocks[1] = 1;
//    erased_blocks[2] = 3;

//    fec_block_nos[0] = 1;
//    fec_block_nos[1] = 3;
//    fec_block_nos[2] = 5;
//    dec_fec_blocks[0] = fec_blocks[1];
//    dec_fec_blocks[1] = fec_blocks[3];
//    dec_fec_blocks[2] = fec_blocks[5];
//    nr_fec_blocks = 3;

//    printf("erased:%s\n", text);

//    reed_solomon_decode(rs, data_blocks, block_size, dec_fec_blocks,
//            fec_block_nos, erased_blocks, nr_fec_blocks);

//    printf("fixed:%s\n", text);

//    reed_solomon_release(rs);
//}

void test_003(void) {
    char text[] = "hello world hello world ", output[256];
    int block_size = 2;
    int nrDataBlocks = (sizeof(text)/sizeof(char) - 1) / block_size;
    unsigned char* data_blocks[128];
    unsigned char* fec_blocks[128];
    int nrFecBlocks = 6;

    //decode
    unsigned int fec_block_nos[128], erased_blocks[128];
    unsigned char* dec_fec_blocks[128];
    int nr_fec_blocks;

    int i;
    reed_solomon* rs = reed_solomon_new(nrDataBlocks, nrFecBlocks);

    printf("%s:\n", __FUNCTION__);
    //printf("text size=%d\n", (int)(sizeof(text)/sizeof(char) - 1) );

    for(i = 0; i < nrDataBlocks; i++) {
        data_blocks[i] = (unsigned char*)&text[i*block_size];
    }

    memset(output, 0, sizeof(output));
    memcpy(output, text, nrDataBlocks*block_size);
    //print_matrix1((gf*)output, nrDataBlocks + nrFecBlocks, block_size);
    for(i = 0; i < nrFecBlocks; i++) {
        fec_blocks[i] = (unsigned char*)&output[i*block_size + nrDataBlocks*block_size];
    }
    reed_solomon_encode(rs, data_blocks, fec_blocks, block_size);
    //printf("golang output(example/test_rs.go):\n [[104 101] [108 108] [111 32] [119 111] [114 108] [100 32] [104 101] [108 108] [111 32] [119 111] [114 108] [100 32] \n[157 178] [83 31] [48 240] [254 93] [31 89] [151 184]]\n");
    //printf("c verion output:\n");
    //print_buf((gf*)output, "%d ", nrFecBlocks*block_size + nrDataBlocks*block_size);
    //print_matrix1((gf*)output, nrDataBlocks + nrFecBlocks, block_size);

    //decode
    text[1*block_size] = 'x';
    text[10*block_size+1] = 'y';
    text[4*block_size] = 'z';
    erased_blocks[0] = 4;
    erased_blocks[1] = 1;
    erased_blocks[2] = 10;

    fec_block_nos[0] = 1;
    fec_block_nos[1] = 3;
    fec_block_nos[2] = 5;
    dec_fec_blocks[0] = fec_blocks[1];
    dec_fec_blocks[1] = fec_blocks[3];
    dec_fec_blocks[2] = fec_blocks[5];
    nr_fec_blocks = 3;

    //printf("erased:%s\n", text);

    reed_solomon_decode(rs, data_blocks, block_size, dec_fec_blocks,
            fec_block_nos, erased_blocks, nr_fec_blocks);

    //printf("fixed:%s\n", text);

    reed_solomon_release(rs);
}


int size = 240;
unsigned char origin[240];
unsigned char data[500];
unsigned char *data_blocks[100];
unsigned char zilch[100];

u32 Encoding_Time[300];
u32 Construction_Time[300];
int Run_IDX = 0;
void test_004(void) {
    char text[] = "hello world hello world hello world hello world hello world hello world hello world hello world ";
    int dataShards = 13;//N = 13 f = 4
    int parityShards = 5;
    int blockSize = 10;
    //struct timeval tv;
    int i, j, n, seed, nrShards, nrBlocks, nrFecBlocks;
    //unsigned char *data;
    //unsigned char *data_blocks;
    //unsigned char *zilch;
    reed_solomon *rs;

    //gettimeofday(&tv, 0);
    //seed = tv.tv_sec ^ tv.tv_usec;
    //srandom(seed);
	if(Run_IDX == 0)
    fec_init();

    size = sizeof(text)/sizeof(char)-1;
    size = 240;
    //origin = (unsigned char *) malloc(size);
    
	//memcpy(origin, text, size);
    for(i = 0; i < size; i++) {
        origin[i] = (unsigned char)(RNG_Get_RandomNum() % 255);
    }
	
	TIM9_Exceed_Times = 0;
	TIM9->CNT = 0;
	
    nrBlocks = (size+blockSize-1) / blockSize;
    nrBlocks = ((nrBlocks+dataShards-1)/dataShards) * dataShards;
    n = nrBlocks / dataShards;
    nrFecBlocks = n*parityShards;
    nrShards = nrBlocks + nrFecBlocks;
    //data = (unsigned char *) malloc(nrShards * blockSize);
    memcpy(data, origin, size);
    memset(data + size, 0, nrShards*blockSize - size);
	
    //printf("nrBlocks=%d nrFecBlocks=%d nrShards=%d n=%d left=%d\n", nrBlocks, nrFecBlocks, nrShards, n, nrShards*blockSize - size);
	//sprintf((char*)Send_Data_buff,"nrBlocks=%d nrFecBlocks=%d nrShards=%d n=%d left=%d\n", nrBlocks, nrFecBlocks, nrShards, n, nrShards*blockSize - size);LoRa_SendData(Send_Data_buff);
    //print_buf(origin, "%d ", size);
    //print_buf(data, "%d ", nrShards*blockSize);

    //data_blocks = (unsigned char**)malloc( nrShards * sizeof(unsigned char**) );
    for(i = 0; i < nrShards; i++) {
        data_blocks[i] = data + i*blockSize;
    }
	
	rs = reed_solomon_new(dataShards, parityShards);
    reed_solomon_encode2(rs, data_blocks, nrShards, blockSize);
	
	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	Encoding_Time[Run_IDX] = time;
	sprintf((char*)Send_Data_buff,"Encoding_time %d : %d",Run_IDX,time);
	LoRa_SendData(Send_Data_buff);
	
	
    i = memcmp(origin, data, size);
    //assert(0 == i);
	if(0 != i) {sprintf((char*)Send_Data_buff,"test_004: Error1");LoRa_SendData(Send_Data_buff);return;}
	
    //print_matrix2(data_blocks, nrShards, blockSize);
	
	for(int i=0;i<100;i++)zilch[i] = NULL;
    //zilch = (unsigned char*)calloc(1, nrShards);
    n = parityShards;

    /* int es[100];
    es[0] = 3;
    es[1] = 3;
    es[2] = 2;
    es[3] = 8; */

    for(i = 0; i < n-2; i++) {
        j = RNG_Get_RandomNum() % (nrBlocks-1);
        //j = es[i];
        memset(data + j*blockSize, 137, blockSize);
        zilch[j] = 1; //erased!
        printf("erased %d\n", j);
    }
    if(nrFecBlocks > 2) {
        for(i = 0; i < 2; i++) {
            j = nrBlocks + (RNG_Get_RandomNum() % nrFecBlocks);
            memset(data + j*blockSize, 139, blockSize);
            zilch[j] = 1;
            printf("erased %d\n", j);
        }
    }

	
	TIM9_Exceed_Times = 0;
	TIM9->CNT = 0;
	
    reed_solomon_reconstruct(rs, data_blocks, zilch, nrShards, blockSize);
	
	time = TIM9->CNT + TIM9_Exceed_Times * 5000;	Construction_Time[Run_IDX] = time;
	sprintf((char*)Send_Data_buff,"Construction_Time %d : %d",Run_IDX,time);
	LoRa_SendData(Send_Data_buff);
	
	
    i = memcmp(origin, data, size);
    //print_buf(origin, "%d ", nrBlocks);
    //print_buf(data, "%d ", nrBlocks);
    printf("rlt=%d\n", i);
    //assert(0 == i);
	if(0 != i) {sprintf((char*)Send_Data_buff,"test_004: Error2");LoRa_SendData(Send_Data_buff);return;}

//    free(origin);
//    free(data);
//    free(data_blocks);
//    free(zilch);
    reed_solomon_release(rs);
	
	sprintf((char*)Send_Data_buff,"test_004: Success");LoRa_SendData(Send_Data_buff);return;
	
}

void tendermint2(){
	
	init();
	init_public_key();
   //fec_init();
//	test_galois();
//	test_sub_matrix();
//	test_multiply();
//	test_inverse();
//	test_one_encoding();
//	test_one_decoding();
//	test_encoding();
//	test_reconstruct();
	
	//test_001();
    //test_002();
    //test_003();
	
	Run_IDX = 0;
	int Run_Times = 30;
	for(Run_IDX=0;Run_IDX<Run_Times;Run_IDX++)
		test_004();
	delay_ms(200);
	sprintf((char*)Send_Data_buff,"Encoding Time:");LoRa_SendData(Send_Data_buff);
	for(int i=0;i<Run_Times;i++){
		delay_ms(100);
		sprintf((char*)Send_Data_buff,"%d",Encoding_Time[i]);LoRa_SendData(Send_Data_buff);
	}
	
	sprintf((char*)Send_Data_buff,"Construction Time:");LoRa_SendData(Send_Data_buff);
	for(int i=0;i<Run_Times;i++){
		delay_ms(100);
		sprintf((char*)Send_Data_buff,"%d",Construction_Time[i]);LoRa_SendData(Send_Data_buff);
	}
	
//    test_galois();
//    test_sub_matrix();
//    test_multiply();
//    test_inverse();
//    test_one_encoding();
//    test_one_decoding();
//    test_encoding();
//    test_reconstruct();
//    printf("reach here means test all ok\n");

//    benchmarkEncode();

    //test_001();
    //test_002();
//    test_003();
    //test_004();
	while(1);


}