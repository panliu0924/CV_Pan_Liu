#pragma once
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include <ctype.h>
#include <cstring>
#include <stdlib.h>
#include <queue>
#include <fstream>
#include <bits/stdc++.h>
#include <time.h> 
#include <utility>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <limits>
#include <unordered_set>
//using namespace chrono;

#define fault_list_len 2*Nnodes

using namespace std;

#define MAXLINE 10000               /* Input buffer size */
#define MAXNAME 31               /* File name size */

#define Upcase(x) ((isalpha(x) && islower(x))? toupper(x) : (x))
#define Lowcase(x) ((isalpha(x) && isupper(x))? tolower(x) : (x))

enum e_com {READ, PC, HELP, QUIT,LEV,LOGICSIM,RTPG,RFL,PODEM/*DLAG,*/};
enum e_state {EXEC, CKTLD,CKTLEV};         /* Gstate values */
enum e_ntype {GATE, PI, FB, PO};    /* column 1 of circuit format */
enum e_gtype {IPT, BRCH, XOR, OR, NOR, NOT, NAND, AND,XNOR,BUF};  /* gate types */

struct cmdstruc {
   char name[MAXNAME];        /* command syntax */
   void (*fptr)();            /* function pointer of the commands */
   enum e_state state;        /* execution state sequence */
};

typedef struct n_struc {
    unsigned indx;             /* node index(from 0 to NumOfLine - 1 */
    unsigned num;              /* line number(May be different from indx */
    enum e_ntype ntype;
    enum e_gtype type;         /* gate type */
    unsigned fin;              /* number of fanins */
    unsigned fout;             /* number of fanouts */
    struct n_struc **unodes;   /* pointer to array of up nodes */
    struct n_struc **dnodes;   /* pointer to array of down nodes */
    int level;                 /* level of the gate output */
    int value;                 /* 5-value of the current node*/
    vector<pair<int,int>> fault;
    int at_fault = 0;
    int CC0,CC1,C0; 
    size_t value_pfs;
} NSTRUC;                     

/*----------------- Command definitions ----------------------------------*/
#define NUMFUNCS 14
void cread(), pc(), help(), quit(), lev(), logicsim(), rtpg(), rfl(), dfs(), test(),/*dlag(),*/podem(),scoap(), atpg(),pfs();
struct cmdstruc command[NUMFUNCS] = {
    {"READ", cread, EXEC},
    {"PC", pc, CKTLD},
    {"HELP", help, EXEC},
    {"QUIT", quit, EXEC},
    {"LEV", lev, CKTLD},
    {"LOGICSIM",logicsim,CKTLD},
    {"RTPG",rtpg,CKTLD},
    {"RFL",rfl,CKTLD},
    {"DFS",dfs,CKTLD},
    {"TEST",test,CKTLD},
    //{"DLAG",dlag,CKTLD},
    {"PODEM",podem ,CKTLD},
    {"SCOAP",scoap ,CKTLD},
    {"ATPG",atpg ,CKTLD},
    {"PFS",pfs ,CKTLD}

};

/*------------------------------------------------------------------------*/
enum e_state Gstate = EXEC;     /* global exectution sequence */
NSTRUC *Node;                   /* dynamic array of nodes */
NSTRUC **Pinput;                /* pointer to array of primary inputs */
NSTRUC **Poutput;               /* pointer to array of primary outputs */
int Nnodes; 			/* number of nodes */
int Npi;                        /* number of primary inputs */
int Npo;                        /* number of primary outputs */
int Done = 0;                   /* status bit to terminate program */
char *cp;              
char inFile[MAXLINE];
int Ngates; 			/* number of nodes */
int max_level;
double PFS_Fault_Count;

void lev(),node_eval(NSTRUC*np),logicsim_simple(vector<int>input_nodes,vector<int>tp),clear(), allocate(),rtpg(),rfl(),dfs_single(vector<int>input_nodes, vector<int>input_vals),dfs(),scoap(),scoap_simplified(),imply(int indx, int val);
bool c_LEV();
bool podem_run(int indx, int ssa);
pair<int,int> obj(int indx, int val, bool fault_activated,list<int> d_frontier);
pair<int,int> backtrace(int indx, int val);
void pfs_atpg();
void pfs();
pair<vector<int>,vector<int>> rfl_for_atpg();
void pfs_atpg1(),pfs_atpg_tpg();