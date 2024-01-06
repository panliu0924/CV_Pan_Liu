#include "readckt.h"

std::size_t strlen(const char* start) {
   const char* end = start;
   for( ; *end != '\0'; ++end)
      ;
   return end - start;
}


/*-----------------------------------------------------------------------
input: gate type
output: string of the gate type
called by: pc
description:
    The routine receive an integer gate type and return the gate type in
    character string.
-----------------------------------------------------------------------*/
std::string gname(int tp){
    switch(tp) {
        case 0: return("PI");
        case 1: return("BRANCH");
        case 2: return("XOR");
        case 3: return("OR");
        case 4: return("NOR");
        case 5: return("NOT");
        case 6: return("NAND");
        case 7: return("AND");
        case 8: return("XNOR");
    }
    return "";
}


/*-----------------------------------------------------------------------
input: nothing
output: nothing
called by: main 
description:
    The routine prints ot help inormation for each command.
-----------------------------------------------------------------------*/
void help(){
    printf("READ filename - ");
    printf("read in circuit file and creat all data structures\n");
    printf("PC - ");
    printf("print circuit information\n");
    printf("HELP - ");
    printf("print this help information\n");
    printf("QUIT - ");
    printf("stop and exit\n");
}


/*-----------------------------------------------------------------------
input: nothing
output: nothing
called by: main 
description:
    Set Done to 1 which will terminates the program.
-----------------------------------------------------------------------*/
void quit(){
    Done = 1;
}

/*======================================================================*/

/*-----------------------------------------------------------------------
input: nothing
output: nothing
called by: cread
description:
    This routine clears the memory space occupied by the previous circuit
    before reading in new one. It frees up the dynamic arrays Node.unodes,
    Node.dnodes, Node.flist, Node, Pinput, Poutput, and Tap.
-----------------------------------------------------------------------*/
void clear(){
    int i;
    for(i = 0; i<Nnodes; i++) {
        delete(Node[i].unodes);
        delete(Node[i].dnodes);
    }
    delete(Node);
    delete[](Pinput);
    delete[](Poutput);
    Gstate = EXEC;
}

/*-----------------------------------------------------------------------
input: nothing
output: nothing
called by: cread
description:
    This routine allocatess the memory space required by the circuit
    description data structure. It allocates the dynamic arrays Node,
    Node.flist, Node, Pinput, Poutput, and Tap. It also set the default
    tap selection and the fanin and fanout to 0.
-----------------------------------------------------------------------*/
void allocate(){
    int i;
    //Node = (NSTRUC *) malloc(Nnodes * sizeof(NSTRUC));
    Node = new NSTRUC[Nnodes];
    //Pinput = (NSTRUC **) malloc(Npi * sizeof(NSTRUC *));
   // Poutput = (NSTRUC **) malloc(Npo * sizeof(NSTRUC *));
    Pinput = new NSTRUC*[Npi];
    Poutput = new NSTRUC*[Npo];

    for(i = 0; i<Nnodes; i++) {
        Node[i].indx = i;
        Node[i].fin = Node[i].fout = 0;
    }
}


/*-----------------------------------------------------------------------
input: circuit description file name
output: nothing
called by: main
description:
    This routine reads in the circuit description file and set up all the
    required data structure. It first checks if the file exists, then it
    sets up a mapping table, determines the number of nodes, PI's and PO's,
    allocates dynamic data arrays, and fills in the structural information
    of the circuit. In the ISCAS circuit description format, only upstream
    nodes are specified. Downstream nodes are implied. However, to facilitate
    forward implication, they are also built up in the data structure.
    To have the maximal flexibility, three passes through the circuit file
    are required: the first pass to determine the size of the mapping table
    , the second to fill in the mapping table, and the third to actually
    set up the circuit information. These procedures may be simplified in
    the future.
-----------------------------------------------------------------------*/
std::string inp_name = "";
void cread(){
    char buf[MAXLINE];
    int ntbl, *tbl, i, j, k, nd, tp, fo, fi, ni = 0, no = 0;
    FILE *fd;
    NSTRUC *np;
    cp[strlen(cp)-1] = '\0';
    if((fd = fopen(cp,"r")) == NULL){
        printf("File does not exist!\n");
        return;
    }
    inp_name = cp;
    
    if(Gstate >= CKTLD) clear();
    Nnodes = Npi = Npo = ntbl = Ngates = 0;
    while(fgets(buf, MAXLINE, fd) != NULL) {
        if(sscanf(buf,"%d %d", &tp, &nd) == 2) {
            if(ntbl < nd) ntbl = nd;
            Nnodes ++;
            if(tp == PI) Npi++;
            else if(tp == PO) Npo++;
        }
    }
    tbl = new int[++ntbl];

    fseek(fd, 0L, 0);
    i = 0;
    while(fgets(buf, MAXLINE, fd) != NULL) {
        if(sscanf(buf,"%d %d", &tp, &nd) == 2) tbl[nd] = i++;
    }
    allocate();

    fseek(fd, 0L, 0);
    while(fscanf(fd, "%d %d", &tp, &nd) != EOF) {
        np = &Node[tbl[nd]];
        np->num = nd;
        
        if(tp == PI) Pinput[ni++] = np;
        else if(tp == PO) Poutput[no++] = np;
        
        switch(tp) {
            case PI:
            case PO:
            case GATE:
                fscanf(fd, "%d %d %d", &np->type, &np->fout, &np->fin);
		    if((np->type != IPT) && (np->type != BRCH))
			    Ngates++;
                break;
            case FB:
                np->fout = np->fin = 1;
                fscanf(fd, "%d", &np->type);
                break;
            default:
                printf("Unknown node type!\n");
                exit(-1);
        }
       // np->unodes = (NSTRUC **) malloc(np->fin * sizeof(NSTRUC *));
        //np->dnodes = (NSTRUC **) malloc(np->fout * sizeof(NSTRUC *));
        np->unodes = new NSTRUC*[np->fin];
        np->dnodes = new NSTRUC*[np->fout];
        for(i = 0; i < np->fin; i++) {
            fscanf(fd, "%d", &nd);
            np->unodes[i] = &Node[tbl[nd]];
        }
        for(i = 0; i < np->fout; np->dnodes[i++] = NULL);
    }
    for(i = 0; i < Nnodes; i++) {
        for(j = 0; j < Node[i].fin; j++) {
            np = Node[i].unodes[j];
            k = 0;
            while(np->dnodes[k] != NULL) k++;
            np->dnodes[k] = &Node[i];
        }
    }
    fclose(fd);
    Gstate = CKTLD;
    printf("==> OK");
}

/*-----------------------------------------------------------------------
input: nothing
output: nothing
called by: main
description:
    The routine prints out the circuit description from previous READ command.
-----------------------------------------------------------------------*/
void pc(){
    int i, j;
    NSTRUC *np;
    std::string gname(int);
   
    printf(" Node   Type \tIn     \t\t\tOut    \n");
    printf("------ ------\t-------\t\t\t-------\n");
    for(i = 0; i<Nnodes; i++) {
        np = &Node[i];
        printf("\t\t\t\t\t");
        for(j = 0; j<np->fout; j++) printf("%d ",np->dnodes[j]->num);
        printf("\r%5d  %s\t", np->num, gname(np->type).c_str());
        for(j = 0; j<np->fin; j++) printf("%d ",np->unodes[j]->num);
        printf("\n");
    }
    printf("Primary inputs:  ");
    for(i = 0; i<Npi; i++) printf("%d ",Pinput[i]->num);
    printf("\n");
    printf("Primary outputs: ");
    for(i = 0; i<Npo; i++) printf("%d ",Poutput[i]->num);{
    printf("\n\n");
    printf("Number of nodes = %d\n", Nnodes);
    printf("Number of primary inputs = %d\n", Npi);
    printf("Number of primary outputs = %d\n", Npo);}
}

