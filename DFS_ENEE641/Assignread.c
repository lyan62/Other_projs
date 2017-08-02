#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/*!macros, constants and self-defined variable types starts here*/
#define INT_MAX 1000000   /*infinite*/
#define true    1
#define false   0
typedef unsigned char bool;  /*boolean*/
enum Color
{
    WHITE,
    GRAY,
    BLACK
};

/*!function handles declaration starts here*/
void PrintHelp();
void PrintWarnning();
bool DataImport(char* inFileName);
bool DataExport(char* outFileName, bool IsHasCyc);
bool ValidateFileName(char* filename, char* extension);
void ResetData();
bool DFSTplgOrdering();
bool DFSVisiting(int ver);

/*!variables declaration starts here*/
int*  graph         = NULL;
int   verNum        = 0;
int*  ingoing       = NULL;
int*  outgoing      = NULL;
int*  verVisitCount = NULL;
int*  edeVisitCount = NULL;
int*  color         = NULL;
int*  result        = NULL;

int   resType  = 0;             /* B = 0 or C = 1 */
int   index    = 0;

/*!main starts here*/
int main(int argc, char* argv[])
{
    if(argc == 2)
    {
        if(strcmp(argv[1], "--help") == 0)
        {
            PrintHelp();
        }
        else
        {
            PrintWarnning();
        }

        return -1;
    }
    else if(argc != 3)
    {
        PrintWarnning();
        return -1;
    }

    /* argc == 3*/
    /*! innovation 1 : validate the file name with its extension */
    if(ValidateFileName(argv[1], ".txt") && ValidateFileName(argv[2], ".txt"))
    {
        char* fileName = (char*)malloc((strlen(argv[1]) + 1)*sizeof(char));

        strcpy(fileName, argv[1]);
        int i = strlen(fileName) - 1;

        /* C:\users\liwenyan\desktop\in1.txt */
        while(fileName[i] != '\\' && fileName[i] != '/' && i >= 0)
        {
            i--;
        }

        /*! innovation 2 : absolute path is permitted here */
        if(i>=0)
        {
            strcpy(fileName, fileName + i + 1);
        }

        if(strcmp(fileName,"in1.txt") != 0)
        {
            resType = 1;  /* C */
        }
    }
    else
    {
        puts("Error in file extension");
        return -1;
    }

    if(!DataImport(argv[1]))
    {
        puts("Error in DataImport");
        return -1;
    }

    if(!DFSTplgOrdering())
    {
        DataExport(argv[2], true);
        ResetData();
        return 0;
    }

    if(!DataExport(argv[2], false))
    {
        puts("Error in DataExport");
        return -1;
    }

    ResetData();
    return 0;
}

void PrintHelp()
{
    puts("DFS-based topological ordering: I accept only two valid arguments:");
    puts("the first one is the file name of input-data with an extention .txt");
    puts("the second one is the file name of output-data with an extention .txt");
    puts("use me with following command: ./<myname> input.txt output.txt");
}

void PrintWarnning()
{
    puts("Error in arguments !");
    puts("Type \"--help\" for usage !\n");
}

bool DataImport(char* inFileName)
{
    FILE* infile = fopen(inFileName,"rw+");
    if(infile == NULL)
    {
        return false;
    }

    /* file pointer manipulation */
    fseek(infile,0,SEEK_END);
    fprintf(infile,"\n");

    rewind(infile);

    verNum = -INT_MAX;
    while(!feof(infile))
    {
        int tmpInt;
        fscanf(infile,"%d",&tmpInt);
        if(tmpInt > verNum)
        {
            verNum = tmpInt;
        }
    }

    rewind(infile);

    /*! innovation #3 : dynamic memory allocatiing */
    graph         = (int*)malloc((verNum+1)*(verNum+1)*sizeof(int));
    ingoing       = (int*)malloc((verNum+1)*sizeof(int));
    outgoing      = (int*)malloc((verNum+1)*sizeof(int));
    color         = (int*)malloc((verNum+1)*sizeof(int));
    result        = (int*)malloc( verNum   *sizeof(int));
    verVisitCount = (int*)malloc((verNum+1)*sizeof(int));
    edeVisitCount = (int*)malloc((verNum+1)*(verNum+1)*sizeof(int));

    int i,j;
    for(i = 1; i <= verNum; i++)
    {
        for(j = 1; j <= verNum; j++)
        {
            *(graph + (verNum + 1)*i + j) = INT_MAX;
            *(edeVisitCount + (verNum + 1)*i + j) = 0;
        }

        color[i] = WHITE;
        ingoing[i] = 0;
        outgoing[i] = 0;
        verVisitCount[i] = 0;
    }

    int s = -1,e = -1;
    while(!feof(infile))
    {
        /*! innnovation #4 : no any assumption was taken */
        int lineLen = 0;
        while(true)
        {
            char ch = fgetc(infile);
            if(feof(infile))
            {
                goto ENDF;
            }

            if(ch != '\n')
            {
                lineLen++;
            }
            else
            {
                break;
            }
        }

        //printf("lineLen:  %d\n",lineLen);

        fseek(infile,-(lineLen + 1) - 1,SEEK_CUR);

        char* buffer = (char*)malloc((lineLen + 3)*sizeof(char));
        fgets(buffer,lineLen + 2,infile);

        sscanf(buffer,"%d",&s);
        bool firstDone = false;
        for(i = 0; buffer[i] != 0; i++)
        {
            e = 0;
            if(buffer[i] >= '0' && buffer[i] <= '9')
            {
                int j = 1;
                int k = 0;
                while(buffer[i + j] >= '0' && buffer[i + j] <= '9')
                {
                    j++;
                }

                k = j;
                while(k)
                {
                    e += (buffer[i + j - k] - '0') * pow(10,k-1);
                    k--;
                }

                i += j;

                if(e != 0 && !firstDone)
                {
                    firstDone = true;
                    continue;
                }
            }

            if(e == 0)
            {
               continue;
            }

            printf("%d %d\n",s,e);

            *(graph + (verNum + 1)*s + e) = 1;
            if(s != e)
            {
                outgoing[s] += 1;
                ingoing[e]  += 1;
            }
            printf("outgoing&ingoing");
            printf("%d %d\n",outgoing[s],ingoing[e]);
        }
    }

    ENDF:
    fclose(infile);

    return true;
}

bool DataExport(char* outFileName, bool isHasCyc)
{
    FILE* outfile = fopen(outFileName,"w");
    if(outfile == NULL)
    {
        return false;
    }

    if(isHasCyc)
    {
        fputs("This graph has a cycle!\n",outfile);
    }
    else
    {
        int i;
        for(i = 0; i != verNum; i++)
        {
            fprintf(outfile, "%d ", result[i]);
        }
        fprintf(outfile, "\n");
    }

    if(resType == 0)       /* B */
    {
        int i,j;
        int verChargedCount = 0;
        int edeChargedCount = 0;

        for(i = 1; i <= verNum; i++)
        {
            fprintf(outfile,"Vertex %d :total %d operations (C commands) charged to Vertex %d in unary\n",i,verVisitCount[i],i);
            verChargedCount += verVisitCount[i];
        }

        for(i = 1; i <= verNum; i++)
        {
            for(j = 1; j <= verNum; j++)
            {
                fprintf(outfile,"Edge from Vertex number %d to Vertex number %d: total %d operations (C commands) charged to this edge in unary\n",
                        i,
                        j,
                        *(edeVisitCount + (verNum+1)*i +j)
                        );
                edeChargedCount += *(edeVisitCount + (verNum+1)*i +j);
            }
        }

        fprintf(outfile,"Total number of operations charged to all vertices is : %d\n",verChargedCount);
        fprintf(outfile,"Total number of operations charged to edges is : %d\n",edeChargedCount);
        fprintf(outfile,"Total number of operations is : %d\n",verChargedCount + edeChargedCount);
    }
    else if(resType == 1)  /* C */
    {
        int i,j;
        int verChargedCount = 0;
        int edeChargedCount = 0;

        for(i = 1; i <= verNum; i++)
        {
            /*fprintf(outfile,"Vertex %d :total %d operations (C commands) charged to Vertex %d in unary\n",i,verVisitCount[i],i);*/
            verChargedCount += verVisitCount[i];
        }

        for(i = 1; i <= verNum; i++)
        {
            for(j = 1; j <= verNum; j++)
            {
                /*fprintf(outfile,"Edge from Vertex number %d to Vertex number %d: total %d operations (C commands) charged to this edge in unary\n",
                        i,
                        j,
                        *(edeVisitCount + (verNum+1)*i +j)
                        );*/
                edeChargedCount += *(edeVisitCount + (verNum+1)*i +j);
            }
        }

        fprintf(outfile,"Total number of operations charged to all vertices is : %d\n",verChargedCount);
        fprintf(outfile,"Total number of operations charged to edges is : %d\n",edeChargedCount);
        fprintf(outfile,"Total number of operations is : %d\n",verChargedCount + edeChargedCount);
    }
    else
    {
        return false;
    }

    fclose(outfile);
    return true;
}

bool ValidateFileName(char* filename, char* extension)
{
    char suffix[5] = {0,0,0,0,0};
    int  fileNameStrlen = strlen(filename);
    int  i;

    if( fileNameStrlen <= 4 )
    {
        return false;
    }

    for(i = 0; i != 4; i++)
    {
        suffix[i] = filename[fileNameStrlen - 4 + i];
    }

    if(strcmp(suffix, extension) != 0)
    {
        return false;
    }

    return true;
}

void ResetData()
{
    if(graph != NULL)
    {
        free(graph);
        graph = NULL;
    }

    if(ingoing != NULL)
    {
        free(ingoing);
        ingoing = NULL;
    }

    if(outgoing != NULL)
    {
        free(outgoing);
        outgoing = NULL;
    }

    if(result != NULL)
    {
        free(result);
        result = NULL;
    }

    if(verVisitCount != NULL)
    {
        free(verVisitCount);
        verVisitCount = NULL;
    }

    if(edeVisitCount != NULL)
    {
        free(edeVisitCount);
        edeVisitCount = NULL;
    }

    verNum   = 0;

    index    = 0;
}

bool DFSTplgOrdering()
{
    int i;
    for(i = 1; i <= verNum; i++)
    {
        if( outgoing[i] == 0 )    /* the most grand child */
        {
            if(!DFSVisiting(i))
            {
                return false;
            }
        }
    }

    return true;
}

bool DFSVisiting(int ver)
{
    if(color[ver] == BLACK)
    {
        return true;
    }

    if( ingoing[ver] == 0)      /* the most grand parent */
    {
        if( color[ver] == WHITE)
        {
            color[ver] = BLACK;
            verVisitCount[ver]++;
            result[index++] = ver;
            printf("ingoing0:");
            printf("%d\n",ver);
            printf("%d\n",index);
        }
        return true;
    }

    if(color[ver] != BLACK)
    {
        color[ver] = GRAY;
        verVisitCount[ver]++;
    }

    int i;
    for(i = 1; i <= verNum; i++)
    {
        if(*(graph + i * (verNum + 1) + ver) < INT_MAX)
        {
            (*(edeVisitCount + (verNum + 1)*ver + i))++;

            if(color[i] == GRAY)
            {
                return false;
            }

            if(!DFSVisiting(i))
            {
                return false;
            }
        }
    }

    if( color[ver] == GRAY)
    {
        color[ver] = BLACK;
        verVisitCount[ver]++;
        result[index++] = ver;
        printf("colorgray:");
        printf("%d\n",ver);
        printf("%d\n",index);

    }

    return true;
}
