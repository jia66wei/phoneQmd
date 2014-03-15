#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include <dirent.h>

#define Hash 101 // 俺照uid哈希，方便查找
#define Col  1000000 // Hash*Col = UID 数目

#define NUM  700000000// 关系数
#define Array  3  // 将所需要的空间动态分隔malloc 

/*
 * @jiawei
 * 与其他版本的不同点是必须经过排序
 * */

//int32_t HASH_PID_SIZE[Hash][Col];

//int32_t HASH_PID_SIZE_2[Hash][Col];
//int32_t HASH_SIZE_2[Hash]; //count:统计每个中第三维的个数; HASH_SIZE: 统计第二维的个数

typedef struct Index
{
	int64_t pid;
	int64_t index;
	int32_t count;
	int32_t array; //关系在第几个动态分配数组中
}Index;


void Initial(int64_t *PID,int64_t size)
{
	memset(PID,0,sizeof(int64_t)*size);
}
void InitialHash(int32_t *HASH_SIZE, int size)
{
	int i;
	for(i = 0 ; i < size ; ++i)
	{
		HASH_SIZE[i] = 0;
	}
}

int Find(Index *phone,int32_t SIZE[], int64_t lhv)
{
	int pos= -1;
	int hash = lhv % Hash;
	int left = 0, right = SIZE[hash]-1;
	int mid;
	while(left <= right)
	{
		mid =   (( right + left)) >> 1;
		if(phone[hash*Col+ mid].pid  == lhv) // hash*grade+mid*Row=phone[hash][mid][0]
		{
			return mid;
		}	
		else if(phone[hash*Col + mid].pid < lhv )
		{
			left = mid + 1;
		}
		else
		{
			right = mid -1;
		}

	}
	return pos;
}

void InsertKey(Index *phone, int64_t key,int64_t index, int32_t count, int32_t hash_size[])
{
	int hash = key % Hash ;
	int mod = key % Array ;
	int len = hash_size[hash]-1;
	while(len >= 0 && phone[hash*Col+len].pid > key )
	{
		phone[hash*Col+len+1].pid=phone[hash*Col+len].pid;
		phone[hash*Col+len+1].index=phone[hash*Col+len].index;
		phone[hash*Col+len+1].count=phone[hash*Col+len].count;
		phone[hash*Col+len+1].array=phone[hash*Col+len].array;
		--len;
	}	
	phone[hash*Col+len+1].pid = key;
	phone[hash*Col+len+1].index = index;
	phone[hash*Col+len+1].count = count;
	phone[hash*Col+len+1].array = mod;
	hash_size[hash]++;
}

/*
void Traverse(int64_t *PID[],Index *phone)
{
	printf("Traverse begining\n");
	int i,j,k;
	int mod;
	for(i = 0; i < Hash ; i++)
	{
		for(j = 0; j < HASH_SIZE[i] ; j++)
		{
			printf("---%lld---",phone[i*Col +j].pid);
			mod = phone[i*Col +j].pid % Array;
			for(k = 0; k < phone[i*Col +j].count ; k++)
			{
//				printf("%d\t",phone[i][j][k]);	
				printf("%lld\t",PID[mod][phone[i*Col +j].index +k]);
			}
			printf("\n");
		}
	}
	printf("Traverse endding\n");
}
*/
void computeQmd(int64_t *PID[],int64_t *PID_2[], Index *phone, Index *phone_2,int32_t HASH_SIZE[], int32_t HASH_SIZE_2[], int64_t pid1, int64_t pid2,FILE*fp[],int outfileNum)
{
	int flag12 =0, flag21= 0;			
	int hash1 = pid1 % Hash;
	int hash2 = pid2 % Hash;
	int mod1 = pid1 % Array;
	int mod2 = pid2 % Array;
	int pos1 = Find(phone,HASH_SIZE, pid1);
	int pos2 = Find(phone_2,HASH_SIZE_2, pid2);
	int share = 0;
	int i, j;
	if( -1 == pos2) return; //不输出与手机号的亲密度
	for(i = 0 ; pos1 >= 0 && i < phone[hash1*Col+pos1].count;++i)
	{
		if(PID[mod1][phone[hash1*Col+pos1].index + i] == pid2) 
		{
			flag12 = 1;
			continue;
		}
	}
	for(i = 0 ; pos2 >= 0 && i <  phone_2[hash2*Col+pos2].count ; ++i)
	{
		if(PID_2[mod2][phone_2[hash2*Col+pos2].index + i] == pid1) 
		{
			flag21 = 1;
			continue;
		}
	}
	for(i = 0 ; pos1 >=0 && i < phone[hash1*Col+pos1].count ; ++i)
	{
		for(j = 0 ; pos2 >= 0 && j< phone_2[hash2*Col+pos2].count ;++j)
		{
			if(PID[mod1][phone[hash1*Col+pos1].index + i] == PID_2[mod2][phone_2[hash2*Col+pos2].index + j])
			{
				++share;	
				continue;
			}
		}
	}

	float w12 = flag12*32 + flag21*8 + share*0.5;
	int mod = pid1%outfileNum;
	fprintf(fp[mod],"%lld\t%lld\t%f\t%d\t%d\t%d\n",pid1,pid2,w12,flag12,flag21,share);
//	printf("%lld\t%lld\t%f\t%d\t%d\t%d\n",pid2,pid1,w21,flag21,flag12,share);

}

void phoneQmd(int64_t *PID[],int64_t *PID_2[], Index *phone,Index *phone_2,int32_t HASH_SIZE[], int32_t HASH_SIZE_2[],int infileNum, int fileID, FILE *fp[], int outfileNum)
{
	printf("beginning phoneQmd\n");
	int i,j,k;
	int64_t key, value;
	for(i = 0; i < Hash ; i++)
	{
		for(j = 0; j < HASH_SIZE[i] ; j++)
		{
			int32_t length = phone[i*Col + j].count;	
			int32_t position = phone[i*Col + j].index;	
			int32_t mod = phone[i*Col + j].array;
			for(k = 0; k < length ; ++k)
			{
				value =PID[mod][position+k] ;
				if( value % infileNum == fileID)
				{
					computeQmd(PID,PID_2,phone, phone_2, HASH_SIZE, HASH_SIZE_2, phone[i*Col+j].pid, PID[mod][position+k],fp, outfileNum); // 可以优化
				}
			}
		}
	}
	printf("endding phoneQmd\n");
}

void ReadFile(int64_t *PID[],Index *phone, char *infile,int32_t hash_size[])
{
	FILE *fp;	
	if((fp=fopen(infile,"r"))==NULL)
	{ //文本只读方式重新打开文件
		printf("cannot open file\n");
		return ;
	}
	int64_t key=0, value=0;
	int64_t index[Array]= {0}, pre = 0, count = 0;
	int32_t mod = 0, hash;
	while(fscanf(fp,"%lld,%lld",&key,&value) != EOF)
	{
		if(key == value) continue;
		mod = key % Array ;
		hash = key % Hash;
		if( pre != key && pre != 0)
		{
			InsertKey(phone,pre,index[pre%Array]-count,count,hash_size);
			count = 0;
		}
		PID[mod][index[mod]] = value;
		++index[mod];
		++count;
//		++hash_size[hash];
		pre = key;
	}
	InsertKey(phone,pre,index[pre%Array]-count,count,hash_size);
	printf("end ith readfile\n");
}

int main(int argc, char *argv[])
{
	printf("---hello world\n");
	if(argc < 5)
	{
		printf("need:exe,indir,outdir,infileNum,outfileNum\n");
		return 0;
	}
	printf("hello world\n");
	int32_t HASH_SIZE[Hash]; //count:统计每个中第三维的个数; HASH_SIZE: 统计第二维的个数
	int32_t HASH_SIZE_2[Hash]; //count:统计每个中第三维的个数; HASH_SIZE: 统计第二维的个数
	int64_t *PID[Array];// 存分割关系， 下同
	int64_t *PID_2[Array] ;
	int i,j;
	printf("beginning malloc Index: \n");
	Index *phone = (Index *)malloc(sizeof(Index)*Hash*Col); // 存储pid关系信息，关系对应PID；下对应PID_2
	Index *phone_2 = (Index *)malloc(sizeof(Index)*Hash*Col);
	printf("beginning memset: \n");
	memset(phone,0,sizeof(Index)*Hash*Col);
	memset(phone_2,0,sizeof(Index)*Hash*Col);
	printf("endding memset: \n");
	for( i = 0 ; i < Array ; i++)
	{
		printf("begin malloc: %d\n", i);
		PID[i] =  (int64_t *)malloc(sizeof(int64_t)*(NUM / Array));
		PID_2[i] =  (int64_t *)malloc(sizeof(int64_t)*(NUM / Array));
//		Initial(PID[i],NUM/Array);
//		Initial(PID_2[i],NUM/Array);
		printf("end malloc: %d\n", i);
	}
	int infileNum = atoi(argv[3]), outfileNum = atoi(argv[4]);
	FILE *fp[outfileNum];
	//使用这个方法打开目录文件，方便个性化……
	for(i =0 ; i < outfileNum ; ++i)		
	{
		char outfile[1000];
		sprintf(outfile,"%s\%d.txt",argv[2],i);
		if((fp[i] = fopen(outfile,"w")) == NULL)
		{
			printf("open file failed:%s\n",outfile);
			return -1;
		}
		//fprintf(fp[i],"%d\t%d\n",123,456);
	}
	// 切片度文件,求亲密度……
	//
	int k; 
	for(i = 0; i < infileNum ; i++ )
	{
		printf("i:%d\n",i);
//		for(k=0;k<Array;++k)Initial(PID[i],NUM/Array);
		InitialHash(HASH_SIZE,Hash);
//		memset(phone,0,sizeof(Index)*Hash*Col);
		char infile1[1000];
		sprintf(infile1,"%s%d.txt",argv[1],i);
		printf("ith readfile:\n");
		ReadFile(PID,phone,infile1,HASH_SIZE);
		printf("beginning:--i--%d:\n",i);
		for(j = 0; j < infileNum ; j++ ) //可以优化下,其实多读了次
		{
			printf("j:%d\n",j);
			if(i == j)
			{
				phoneQmd(PID,PID,phone,phone,HASH_SIZE,HASH_SIZE,infileNum,j,fp, outfileNum);
				continue;
			}
//			for(k=0;k<Array;++k)Initial(PID_2[i],NUM/Array);
			InitialHash(HASH_SIZE_2,Hash);
//			memset(phone_2,0,sizeof(Index)*Hash*Col);
			char infile2[1000];
			sprintf(infile2,"%s/%d.txt",argv[1],j);
			printf("beginning readfile:\n");
			ReadFile(PID_2,phone_2,infile2,HASH_SIZE_2);
			printf("ending readfile:\n");
			phoneQmd(PID,PID_2,phone,phone_2,HASH_SIZE,HASH_SIZE_2,infileNum,j,fp, outfileNum);
			printf("endding:--j--%d:\n",j);
		}
	}
//	Traverse(PID,phone);
	free(phone);
	free(phone_2);
	for( i = 0 ; i < Array ; i++)
	{
		free(PID[i]);
		free(PID_2[i]);
	}
	return 0;
}
