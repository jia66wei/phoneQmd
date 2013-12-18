#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

#define Hash 1000
#define Col  26000

#define NUM  5000000000

/*
 * @jiawei
 * 与其他版本的不同点是必须经过排序
 * */
int32_t count[Hash][Col];
int32_t count2[Hash]; //count:统计每个中第三维的个数; count: 统计第二维的个数
typedef struct Index
{
	int64_t pid;
	int64_t index;
	int32_t count;
}Index;


void Initial()
{
	memset(count2,0,sizeof(int32_t)*Hash);
}

int Find(Index *phone,int64_t lhv)
{
	int pos= -1;
	int hash = lhv % Hash;
	int left = 0, right = count2[hash]-1;
	int mid;
//	printf("***%d****%d**%d****\n",right+1,lhv,hash);
	while(left <= right)
	{
		mid =   (( right + left)) >> 1;
//		printf("%d,%d,%d,%d\n",left,mid,right,phone[hash][mid][0]);
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

void InsertKey(Index *phone, int64_t key,int64_t index, int32_t count)
{
//	printf("%lld---%lld---%d\n",key,index,count);
	int hash = key % Hash ;
	int len = count2[hash]-1;
	while(len >= 0 && phone[hash*Col+len].pid > key )
	{
		phone[hash*Col+len+1].pid=phone[hash*Col+len].pid;
		phone[hash*Col+len+1].index=phone[hash*Col+len].index;
		phone[hash*Col+len+1].count=phone[hash*Col+len].count;
		--len;
	}	
	phone[hash*Col+len+1].pid = key;
	phone[hash*Col+len+1].index = index;
	phone[hash*Col+len+1].count = count;
	count2[hash]++;
	//return len+1;
}

/*
void Connect(int64_t *PID,int64_t *phone,int64_t index, int32_t count)
{
	int32_t hash = index % Hash;
	int32_t posCol = Find(phone,lhv);
  	if( -1 == posCol)
	{
		posCol = InsertKey(phone,lhv);
	}	
	//	phone[hash][posCol][count[hash][posCol]] = rhv;
		phone[hash*grade + posCol*Row+count[hash][posCol]] = rhv;
		count[hash][posCol]++;
}
*/

void Traverse(int64_t *PID,Index *phone)
{
	printf("Traverse begining\n");
	int i,j,k;
	for(i = 0; i < Hash ; i++)
	{
		for(j = 0; j < count2[i] ; j++)
		{
			printf("---%lld---",phone[i*Col +j].pid);
			for(k = 0; k < phone[i*Col +j].count ; k++)
			{
//				printf("%d\t",phone[i][j][k]);	
				printf("%lld\t",PID[phone[i*Col +j].index +k]);
			}
			printf("\n");
		}
	}
	printf("Traverse endding\n");
}

void computeQmd(int64_t *PID, Index *phone,int64_t pid1, int64_t pid2)
{
	int flag12 =0, flag21= 0;			
	int hash1 = pid1 % Hash;
	int hash2 = pid2 % Hash;
	int pos1 = Find(phone,pid1);
	int pos2 = Find(phone,pid2);
	int share = 0;
	int i, j;
	for(i = 0 ; pos1 >= 0 && i < phone[hash1*Col+pos1].count;++i)
	{
		if(PID[phone[hash1*Col+pos1].index + i] == pid2) 
		{
			flag12 = 1;
			continue;
		}
	}
	for(i = 0 ; pos2 >= 0 && i <  phone[hash2*Col+pos2].count ; ++i)
	{
		if(PID[phone[hash2*Col+pos2].index + i] == pid1) 
		{
			flag21 = 1;
			continue;
		}
	}
	for(i = 0 ; pos1 >=0 && i < phone[hash1*Col+pos1].count ; ++i)
	{
		for(j = 0 ; pos2 >= 0 && j< phone[hash2*Col+pos2].count ;++j)
		{
			if(PID[phone[hash1*Col+pos1].index + i] == PID[phone[hash2*Col+pos2].index + j])
			{
				++share;	
				continue;
			}
		}
	}

	float w12 = flag12*32 + flag21*8 + share*0.5;
//	float w21 = flag21*32 + flag12*8 + share*0.5;
	printf("%lld\t%lld\t%f\t%d\t%d\t%d\n",pid1,pid2,w12,flag12,flag21,share);
//	printf("%lld\t%lld\t%f\t%d\t%d\t%d\n",pid2,pid1,w21,flag21,flag12,share);

}

void phoneQmd(int64_t *PID, Index *phone)
{
	int i,j,k;
	for(i = 0; i < Hash ; i++)
	{
		for(j = 0; j < count2[i] ; j++)
		{
			int32_t C = phone[i*Col + j].count;	
			int32_t in = phone[i*Col + j].index;	
			for(k = 0; k < C ; ++k)
			{
//				printf("--%lld--%lld\n",phone[i*Col+j].pid, PID[in+k]);
				computeQmd(PID,phone,phone[i*Col+j].pid, PID[in+k]); // 可以优化
			}
		}
	}
}

void ReadFile(int64_t *PID,Index *phone, char *infile)
{
	FILE *fp;	
	if((fp=fopen(infile,"r"))==NULL)
	{ //文本只读方式重新打开文件
		printf("cannot open file\n");
		return ;
	}
	int64_t key=0, value=0;
	int64_t index = 0, pre = 0, count = 0;
	while(fscanf(fp,"%lld%lld",&key,&value) != EOF)
	{
//		printf("%lld\t%lld\n",key,value);
		if(0 == index)
		{
			pre = key;
			PID[index] = value;
			++index;
			++count;
			continue;	
		}
		if( pre != key)
		{
			InsertKey(phone,pre,index-count,count);
			count = 0;
			pre = key;
		}
		PID[index] = value;
		++index;
		++count;
	}
	InsertKey(phone,pre,index-count,count);


}

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("need input file\n");
		return 0;
	}
	int64_t *PID_1 = (int64_t *)malloc(sizeof(int64_t)*NUM);
	int64_t *PID = (int64_t *)malloc(sizeof(int64_t)*NUM);
	Index *phone = (Index *)malloc(sizeof(Index)*Hash*Col);
	Initial();
	ReadFile(PID,phone,argv[1]);
//	Traverse(PID,phone);
	phoneQmd(PID,phone);
	free(phone);
	free(PID);
	return 0;
}
