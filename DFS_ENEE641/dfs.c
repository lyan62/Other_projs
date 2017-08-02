#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/*!macros, constants and self-defined variable types starts here*/
#define INT_MAX 1000000
#define true    1
#define false   0
typedef unsigned char bool;
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
int*  graph    = NULL;
int   verNum   = 0;
int*  ingoing  = NULL;
int*  outgoing = NULL;
int*  d        = NULL;
int*  f        = NULL;
int*  color    = NULL;
int*  result   = NULL;
int   time     = 0;

int   resType  = 0;
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

    if(ValidateFileName(argv[1], ".txt") && ValidateFileName(argv[2], ".txt"))
    {
        char* fileName = (char*)malloc((strlen(argv[1]) + 1)*sizeof(char));

        strcpy(fileName, argv[1]);
        int i = strlen(fileName) - 1;

        while(fileName[i] != '\\' && fileName[i] != '/' && i >= 0)
        {
            i--;
        }

        /*!absolute path is permitted here*/
        if(i>=0)
        {
            strcpy(fileName, fileName + i + 1);
        }

        if(strcmp(fileName,"in1.txt") != 0)
        {
            resType = 1;
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
    FILE* infile = fopen(inFileName,"r+");
    if(infile == NULL)
    {
        return false;
    }

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

    graph    = (int*)malloc((verNum+1)*(verNum+1)*sizeof(int));
    ingoing  = (int*)malloc((verNum+1)*sizeof(int));
    outgoing = (int*)malloc((verNum+1)*sizeof(int));
    color    = (int*)malloc((verNum+1)*sizeof(int));
    result   = (int*)malloc( verNum   *sizeof(int));
    d        = (int*)malloc((verNum+1)*sizeof(int));
    f        = (int*)malloc((verNum+1)*sizeof(int));

    int i,j;
    for(i = 1; i <= verNum; i++)
    {
        for(j = 1; j <= verNum; j++)
        {
            *(graph + verNum*i + j) = INT_MAX;
        }

        color[i] = WHITE;
        ingoing[i] = 0;
        outgoing[i] = 0;
        d[i] = 0;
        f[i] = 0;
    }

    int s = -1,e = -1;
    while(!feof(infile))
    {
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

            //printf("%d %d\n",s,e);

            *(graph + verNum*s + e) = 1;
            if(s != e)
            {
                outgoing[s] += 1;
                ingoing[e]  += 1;
            }
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
        fputs("This graph has a cycle!",outfile);
        return true;
    }

    int i;
    for(i = 0; i != verNum; i++)
    {
        fprintf(outfile, "%d ", result[i]);
    }
    fprintf(outfile, "\n");

    if(resType == 0)
    {
        /* B */
    }
    else if(resType == 1)
    {
        /* C */
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

    if(d != NULL)
    {
        free(d);
        d = NULL;
    }

    if(f != NULL)
    {
        free(f);
        f = NULL;
    }

    verNum   = 0;
    time     = 0;

    index    = 0;
}

bool DFSTplgOrdering()
{
    int i;
    for(i = 1; i <= verNum; i++)
    {
        if( outgoing[i] == 0 )
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
    /*printf("%d\n",ver);*/
    /*printf("%d\n",color[ver]);*/
    /*printf("  %d\n",1);*/
    if(color[ver] == BLACK)
    {
        return true;
    }
    /*printf("  %d\n",2);*/
    if( ingoing[ver] == 0)
    {
        if( color[ver] == WHITE)
        {
            color[ver] = BLACK;
            /*printf("->%d\n",index);*/
            result[index++] = ver;
        }
        return true;
    }
    /*printf("  %d\n",3);*/
    if(color[ver] != BLACK)
    {
        color[ver] = GRAY;
    }

    time++;
    d[ver] = time;
    /*printf("  %d\n",4);*/
    int i;
    for(i = 1; i <= verNum; i++)
    {
        if(*(graph + i * verNum + ver) < INT_MAX)
        {
            if(color[i] == GRAY)
            {
                return false;
            }
            /*printf("  %d\n",41);*/
            if(!DFSVisiting(i))
            {
                /*printf("  %d\n",411);*/
                return false;
            }
            /*printf("  %d\n",42);*/
        }
    }
    /*printf("  %d\n",5);*/
    if( color[ver] == GRAY)
    {
        color[ver] = BLACK;
        result[index++] = ver;
    }

    time++;
    f[ver] = time;
    /*printf("  %d\n",6);*/
    return true;
}
