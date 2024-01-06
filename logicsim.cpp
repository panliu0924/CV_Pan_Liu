#include "readckt.h"


bool c_LEV(){
	NSTRUC *np;
	for(int i = 0; i < Nnodes; i++){
		np = &Node[i];
		if(np->level == -1)
			return false;
	}
	return true;	
}


void lev() {
//Initiate
	//Outfile Setup
FILE *outfile = NULL;
cp[strlen(cp)-1] = '\0';
outfile = fopen("lev_out.txt", "w");
	//Variable Setup
NSTRUC *np, *np_in;
//std::queue<NSTRUC*> c_LEV;
int level = 0;
bool levelize= false;
	


//Now we format the txt outcome file
	//Find the name of the circuit only, without other symbols or suffixes
std::string c_name;
int dot = inp_name.find_last_of(".");
int slash = inp_name.find_last_of("/");
if (slash == -1) {
    c_name = inp_name.substr(0, dot);} 
else {
    c_name = inp_name.substr(slash + 1, inp_name.substr(slash + 1).find('.'));}
	//Print in the correct format	

// c.FI: set of fanin-lines of c.E
for (int i = 0; i < Nnodes; i++) {
    if (Node[i].type == IPT) {
        Node[i].level = 0;}
    else{
        Node[i].level =-1;}
//        c_LEV.push(&Node[i]);}
}

//4 while ∃c ∈ C, c.LEV = none do
//5 if v.LEV 6= none, ∀v ∈ c.FI, then
//6 c.LEV = max(v.LEV, ∀v ∈ c.FI) + 1

while (!c_LEV()) {
//	NSTRUC* np_in = c_LEV.front();
//    c_LEV.pop();
    for(int i = 0; i < Nnodes; i++){
        np = &Node[i];
        int fanin_level = -1;
        for (int j = 0; j < np->fin; j++) {
            np_in = np->unodes[j];
            if (np_in -> level == -1) {
                levelize = false;
                break;
            }
            levelize = true;
            fanin_level = std::max(fanin_level, np_in->level);
        }
        if(levelize)
            np->level = fanin_level + 1;
    }
	// for (int j = 0; j < np->fout; j++) {
    //     c_LEV.push(np->dnodes[j]);
    // }
	fprintf(outfile, "%s\n", c_name.c_str());
	fprintf(outfile, "#PI: %d\n", Npi);
	fprintf(outfile, "#PO: %d\n", Npo);
	fprintf(outfile, "#Nodes: %d\n", Nnodes);
	fprintf(outfile, "#Gates: %d\n", Ngates);
	for (int i = 0; i < Nnodes; i++){
   	    np = &Node[i];
   	    fprintf(outfile, "%d %d\n", np->num, np->level);}
    }
fclose(outfile);
Gstate = CKTLEV;
}


//UPDATE this function to check if the propagated input has at_fault == true
// if yes, inherit that to the output and eval using 3 and 4 instead of 0 and 1
void node_eval(NSTRUC* np){
    int c;
    int i;
    int x_flag;
    int dom_flag;
    int D_flag,D1_flag;
    int out;
    int fault = 0;
    if(np->type == IPT){
        if(np->at_fault != 0){
            if(np->value == 1 && np->at_fault == 1){
                np->value = 3;
            }
            else if(np->value == 0 && np->at_fault == 2){
                np->value = 4;
            }
            /*
            else if(np->value != 2){
                np->value = -1;
            }*/
        }
    }
    else if (np->type == BRCH || np->type == BUF)//there is no way we will find PI because PI is in lev0, we start from lev1;
    {  
        np->value = np->unodes[0]->value;
        if(np->at_fault != 0){
            if(np->value == 1 && np->at_fault == 1){
                np->value = 3;
            }
            else if(np->value == 0 && np->at_fault == 2){
                np->value = 4;
            }
            /*
            else if(np->value != 2){
                np->value = -1;
            }*/
        }
        //printf("num:%d, num->value:%d, at_fault: %d\n", np->num, np->value,np->at_fault);
    }
    else if (np->type == XOR)
    {
        np->value = np->unodes[0]->value;
        int this_val = np->value;
        /*if(np->value == 3){
            d_count++;
            fault = 0;
            np->value  = 1;
        }
        if(np->value == 4){
            dn_count++;
            fault = 1;
            np->value  = 0;
        }
        */
        if(np->value != 2){
            for (int k = 1; k < np->fin; k++)
            {
                int next_val = np->unodes[k]->value;
                if(next_val == 2){
                    this_val = 2;
                    break;
                }
                else if(next_val == 0){
                    if(this_val == 0)
                        this_val = 0;
                    if(this_val == 1)
                        this_val = 1;
                    if(this_val == 3)
                        this_val = 3;
                    if(this_val == 4)
                        this_val = 4;           
                }
                else if(next_val == 1){
                    if(this_val == 0)
                        this_val = 1;
                    if(this_val == 1)
                        this_val = 0;
                    if(this_val == 3)
                        this_val = 4;
                    if(this_val == 4)
                        this_val = 3;           
                }
                else if(next_val == 3){
                    if(this_val == 0)
                        this_val = 3;
                    if(this_val == 1)
                        this_val = 4;
                    if(this_val == 3)
                        this_val = 0;
                    if(this_val == 4)
                        this_val = 1;           
                }
                else if(next_val == 4){
                    if(this_val == 0)
                        this_val = 4;
                    if(this_val == 1)
                        this_val = 3;
                    if(this_val == 3)
                        this_val = 1;
                    if(this_val == 4)
                        this_val = 0;           
                }
                /*int next_input_val = np->unodes[k]->value;
                if(np->unodes[k]->value == 2){
                    np->value = 2;
                    break;
                }
                if(np->unodes[k]->value == 3){
                    d_count++;
                    fault = 1;
                    next_input_val = 0;
                }
                if(np->unodes[k]->value == 4){
                    dn_count++;
                    fault = 1;
                    next_input_val = 1;
                }
                np->value = np->value ^(next_input_val);
                */             
            }
            /*if(fault == 1){
                np->value = 4-(np->value*2)+np->value;
            }  */    
            np->value = this_val;
        }
        if(np->at_fault != 0){
            if(np->value == 1 && np->at_fault == 1){
                np->value = 3;

            }
            else if(np->value == 0 && np->at_fault == 2){
                np->value = 4;
            }
            /*
            else if(np->value != 2){
                np->value = -1;
            }*/
        }    
    }

    else if (np->type == OR)
    {
        c = 1; i = 0; x_flag = 0; dom_flag = 0;
        D_flag = 0; D1_flag = 0;
        for (int k = 0; k < np->fin; k++)
        {   
            if(np->unodes[k]->value == 2) x_flag = 1;
            else if(np->unodes[k]->value == c) dom_flag = 1;
            else if(np->unodes[k]->value == 3){
                D_flag = 1;
            }
            else if(np->unodes[k]->value == 4){
                 D1_flag = 1;
            }
        }   
        if(dom_flag == 1) np->value = c^i;
        else if(x_flag == 1) np->value = 2;
        else if((D_flag == 1) && (D1_flag ==1)) np->value = c^i;
        else if (D1_flag == 1) np->value = 4;
        else if (D_flag == 1) np->value = 3;
        else np->value = !(c^i);
        if(np->at_fault != 0){
            if(np->value == 1 && np->at_fault == 1){
                np->value = 3;

            }
            else if(np->value == 0 && np->at_fault == 2){
                np->value = 4;
            }
            /*
            else if(np->value != 2){
                np->value = -1;
            }*/
        }
    }

    else if (np->type == NOR)
    {
        c = 1; i = 1; x_flag = 0; dom_flag = 0;
        D_flag = 0; D1_flag = 0;
        for (int k = 0; k < np->fin; k++)
        {   
            if(np->unodes[k]->value == 2) x_flag = 1;
            else if(np->unodes[k]->value == c) dom_flag = 1;
            else if(np->unodes[k]->value == 3){
                D_flag = 1;
            }
            else if(np->unodes[k]->value == 4){
                 D1_flag = 1;
            }
        }
        if(dom_flag == 1) np->value = c^i;
        else if(x_flag == 1) np->value = 2;
        else if((D_flag == 1) && (D1_flag ==1)) np->value = c^i;
        else if (D1_flag == 1) np->value = 3;
        else if (D_flag == 1) np->value = 4;
        else np->value = !(c^i);
        if(np->at_fault != 0){
            if(np->value == 1 && np->at_fault == 1){
                np->value = 3;

            }
            else if(np->value == 0 && np->at_fault == 2){
                np->value = 4;
            }
            /*
            else if(np->value != 2){
                np->value = -1;
            }*/
        }
    }

    else if (np->type == NOT)
    {
        if(np->unodes[0]->value ==2){
            np->value = 2;
        }
        else if(np->unodes[0]->value == 3 ) 
            np->value = 4; 
        else if(np->unodes[0]->value == 4 ) 
            np->value = 3; 
        else 
            np->value = !np->unodes[0]->value;
        if(np->at_fault != 0){
            if(np->value == 1 && np->at_fault == 1){
                np->value = 3;

            }
            else if(np->value == 0 && np->at_fault == 2){
                np->value = 4;
            }
            /*
            else if(np->value != 2){
                np->value = -1;
            }*/
        }
    }

    else if (np->type == NAND)
    {
        c = 0; i = 1; x_flag = 0; dom_flag = 0;
        D_flag = 0; D1_flag = 0;
        for (int k = 0; k < np->fin; k++)
        {   
            if(np->unodes[k]->value == 2) x_flag = 1;
            else if(np->unodes[k]->value == c) dom_flag = 1;
            else if(np->unodes[k]->value == 3){
                D_flag = 1;
            }
            else if(np->unodes[k]->value == 4){
                 D1_flag = 1;
            }
        }
        if(dom_flag == 1) np->value = c^i;
        else if(x_flag == 1) np->value = 2;
        else if((D_flag == 1) && (D1_flag ==1)) np->value = c^i;
        else if (D1_flag == 1) np->value = 3;
        else if (D_flag == 1) np->value = 4;
        else np->value = !(c^i);
        if(np->at_fault != 0){
            if(np->value == 1 && np->at_fault == 1){
                np->value = 3;

            }
            else if(np->value == 0 && np->at_fault == 2){
                np->value = 4;
            }
            /*
            else if(np->value != 2){
                np->value = -1;
            }*/
        }
    }

    else if (np->type == AND)
    {
        c = 0; i = 0; x_flag = 0; dom_flag = 0;
        D_flag = 0; D1_flag = 0;
        for (int k = 0; k < np->fin; k++)
        {   
            if(np->unodes[k]->value == 2) x_flag = 1;
            else if(np->unodes[k]->value == c) dom_flag = 1;
            else if(np->unodes[k]->value == 3){
                D_flag = 1;
            }
            else if(np->unodes[k]->value == 4){
                 D1_flag = 1;
            }
        }
        if(dom_flag == 1) np->value = c^i;
        else if(x_flag == 1) np->value = 2;
        else if((D_flag == 1) && (D1_flag ==1)) np->value = c^i;
        else if (D1_flag == 1) np->value = 4;
        else if (D_flag == 1) np->value = 3;
        else np->value = !(c^i);
        if(np->at_fault != 0){
            if(np->value == 1 && np->at_fault == 1){
                np->value = 3;

            }
            else if(np->value == 0 && np->at_fault == 2){
                np->value = 4;
            }
            /*
            else if(np->value != 2){
                np->value = -1;
            }*/
        }
    }
    else if (np->type == XNOR)
    {
        np->value = np->unodes[0]->value;
        int this_val = np->value;
        if(np->value != 2){
            for (int k = 1; k < np->fin; k++)
            {
                int next_val = np->unodes[k]->value;
                if(next_val == 2){
                    this_val = 2;
                    break;
                }
                else if(next_val == 0){
                    if(this_val == 0)
                        this_val = 1;
                    if(this_val == 1)
                        this_val = 0;
                    if(this_val == 3)
                        this_val = 4;
                    if(this_val == 4)
                        this_val = 3;           
                }
                else if(next_val == 1){
                    if(this_val == 0)
                        this_val = 0;
                    if(this_val == 1)
                        this_val = 1;
                    if(this_val == 3)
                        this_val = 3;
                    if(this_val == 4)
                        this_val = 4;           
                }
                else if(next_val == 3){
                    if(this_val == 0)
                        this_val = 4;
                    if(this_val == 1)
                        this_val = 3;
                    if(this_val == 3)
                        this_val = 1;
                    if(this_val == 4)
                        this_val = 0;           
                }
                else if(next_val == 4){
                    if(this_val == 0)
                        this_val = 3;
                    if(this_val == 1)
                        this_val = 4;
                    if(this_val == 3)
                        this_val = 0;
                    if(this_val == 4)
                        this_val = 1;           
                }
                /*int next_input_val = np->unodes[k]->value;
                if(np->unodes[k]->value == 2){
                    np->value = 2;
                    break;
                }
                if(np->unodes[k]->value == 3){
                    d_count++;
                    fault = 1;
                    next_input_val = 0;
                }
                if(np->unodes[k]->value == 4){
                    dn_count++;
                    fault = 1;
                    next_input_val = 1;
                }
                np->value = np->value ^(next_input_val);
                */             
            }
            /*if(fault == 1){
                np->value = 4-(np->value*2)+np->value;
            }  */    
            np->value = this_val;
        }
        if(np->at_fault != 0){
            if(np->value == 1 && np->at_fault == 1){
                np->value = 3;

            }
            else if(np->value == 0 && np->at_fault == 2){
                np->value = 4;
            }
            /*
            else if(np->value != 2){
                np->value = -1;
            }*/
        }    
    }
}

void logicsim_simple(vector<int>input_nodes,vector<int>tp){
    lev();
    NSTRUC *np;
    int node_num =0;

    for (int i = 0;i <Nnodes;i++)
    {
        if(max_level < Node[i].level){
            max_level = Node[i].level;
        }
    }

    for (int j = 0; j < Nnodes; j++)//give input to node.value once
        {
            if (Node[j].level == 0)
            {
                for (int k = 0; k < input_nodes.size(); k++)
                {
                    if (input_nodes.at(k) == Node[j].num)
                    {
                        Node[j].value = tp.at(k);
                        //printf("node:%d, value:%d, level:%d, gatetype:%d\n",Node[j].num,Node[j].value,Node[j].level,Node[j].type);
                    }                    
                }                
            }            
        }

    // for (int j = 0; j < input_nodes.size(); j++)//give input to node.value once
    //     {
    //         node_num = input_nodes.at(j);
    //         np = &Node[node_num];
    //         np -> value =tp.at(j);
    //         printf("node_num: %d,",node_num);
    //         printf("node:%d, value:%d, level:%d, gatetype:%d\n",Node[j].num,Node[j].value,Node[j].level,Node[j].type);
    //     }






    //now we begin from level 1,and we need to caculate the value of lev1;
    for (int f = 1; f <= max_level; f++)// compute logic,start from lev1
        {
            for (int j = 0; j < Nnodes; j++)
            {
                if (Node[j].level == f)//find which node ->which level
                {
                    np = &Node[j];
                    //printf("Node index comes in :%d, Node level:%d\n",Node[j].num,Node[j].level);
                    node_eval(np);
                }           
            }  
        }


}



void test(){
    cp[strlen(cp)-1] = '\0';
    //printf("im here");
    vector<int> input_nodes ={1,2,3,6,7};
    vector<int> tp = {1,0,1,1,1};
    //lev();
    logicsim_simple(input_nodes,tp);
    FILE*fd_write = NULL;

    fd_write = fopen("test_for_lgsimp.txt","w");
    for (int i = 0; i < Nnodes; i++)
        {
            //if (Node[i].fout == 0)
            {
                fprintf(fd_write,"node:%d, value:%d, level:%d, gatetype:%d\n",Node[i].num,Node[i].value,Node[i].level,Node[i].type);
                //fprintf(outputfp,"%d,%d\n",Node[i].num,Node[i].value);
            }            
        }
    fclose(fd_write);
}






void logicsim(){
    NSTRUC *np;
    FILE *fd_write;


//    int* input_array = malloc(sizeof(int) * col*pis);
//    int* output_array;


    cp[strlen(cp)-1] = '\0';
    char* input_file;
    char* output_file;
    input_file= strtok(cp," ");
    output_file = strtok(NULL," ");
    int flag = 0;
    int flag1= 0;

    
    ifstream in_file(input_file);

    if((fd_write = fopen(output_file,"w")) == NULL){
        printf("File does not exist!\n");
        return;
    }

    string line;
    int line_count = 0;

    //printf("first read\n");
    while(getline(in_file,line)){   //读input file里面一共有几行
        line_count = line_count + 1;
        //printf("%s\n",line.c_str());
    }

    int* input_array = (int*) malloc(sizeof ( int ) * (line_count-1) * Npi);
    int* output_array =  (int*)malloc(sizeof ( int ) * (line_count-1) * Npo);
    /*
    int** input_array = (int**)malloc(sizeof(int*) * (line_count-1));
    for(int i = 0; i < line_count-1;i++){
        int* row_array = (int*)malloc(sizeof(int)*Npi);
        input_array[i] = row_array; 
    }*/

    in_file.clear();             //
    in_file.seekg(0, std::ios::beg);
    //printf("second read\n");
    getline(in_file,line);//
    //printf("%s\n",line.c_str());
    istringstream iss(line);
    int* Npi_input = (int*) malloc(sizeof(int)*Npi);//
    int* Npo_output = (int*) malloc(sizeof(int)*Npo);

    string node_str;
    int node_num;
    int count = 0;
    while (std::getline(iss, node_str, ',')) {
        // get one row of input file and read input nodes
        node_num = stoi(node_str);
        // printf("node num is:%d\n",node_num);
        // printf("count is:%d\n",count);
        Npi_input[count] = node_num;
        count = count+1;
    }
// for (int i = 0; i < Npi; i++)
// {
//     printf("Npi_input:%d\n",Npi_input[i]);
// }
    
//printf("third read\n");

    for (int i = 0; i < line_count-1; i++)
    {
        getline(in_file,line);
        istringstream input_vec(line);
        string input_val;
        count = 0;

        while (std::getline(input_vec, input_val, ',')) {
        // get one row of input file and read input nodes
            if(input_val.compare("x") != 0){
                input_array[i*Npi+count] = stoi(input_val);
            }
            else{
                input_array[i*Npi+count] = 2;
            }
            count = count + 1;
        }
        //printf("%s\n",line.c_str());
    }
    
// fprintf(fd_write,"start print input_array\n");
//     for (int i = 0; i < (line_count-1); i++)
//     {
//         for (int k = 0; k < Npi; k++)
//         {
//             fprintf(fd_write,"%d,",input_array[i*Npi+k]);
//         }
//         printf("\n");
        
//     }
// fprintf(fd_write,"print over\n");




//printf("line count is:%d\n",line_count);






lev();



    //int  value;
    int max_level = 0;

    for (int i = 0;i <Nnodes;i++)
    {
        if(max_level < Node[i].level){
            max_level = Node[i].level;
        }
    }

    



    count = 0;
    for (int i = 0; i < Nnodes; i++){//put the output index into Npo_outpu
            if (Node[i].fout == 0)
            {
            Npo_output[count]=Node[i].num;
            count = count +1;
            }
    }
    


for (int i = 0; i < (line_count-1); i++)//hom many test patterns total we have (how many line)
    {
        for (int j = 0; j < Nnodes; j++)//give input to node.value once
        {
            if (Node[j].level == 0)
            {
                for (int k = 0; k < Npi; k++)
                {
                    if (Npi_input[k] == Node[j].num)
                    {
                        Node[j].value = input_array[i*Npi+k];
                        // printf("%d\n",Node[j].value);
                        // printf("%d\n",j);
                        //break;
                    }
                    
                }
                
            }
            
        }

        //now we begin from level 1,and we need to caculate the value of lev1;
        for (int f = 1; f <= max_level; f++)// compute logic,start from lev1
            {
                for (int j = 0; j < Nnodes; j++)
                {
                    if (Node[j].level == f)//find which node ->which level
                    {
                        np = &Node[j];
                        //printf("Node index comes in :%d, Node level:%d\n",Node[j].num,Node[j].level);
                        node_eval(np);
                    }           
                }  
            }



        for (int z = 0; z < Nnodes; z++){//put one pattern output to output array;
                    if (Node[z].fout == 0)
                    {
                        for (int k = 0; k < Npo; k++)
                        {
                            if (Npo_output[k] == Node[z].num)
                            {
                                output_array[i*Npo+k] = Node[z].value;
                            }
                            
                        }
                    }
            }
    }



// for (int i = 0; i < Npi; i++)
// {
//     printf("Npi_input:%d\n",Npi_input[i]);
// }




//     //now we start printing the PO
    /*for (int i = 0; i < Nnodes; i++){
            if (Node[i].fout == 0)
            {
                fprintf(fd_write,"%d,%d\n",Node[i].num,Node[i].value);
                //fprintf(fd,"node:%d, value:%d, level:%d, gatetype:%d\n",Node[i].num,Node[i].value,Node[i].level,Node[i].type);
                //printf("going inside");
            }
    }*/
    


    for (int i = 0; i < Npo; i++)
    {
        if (i<Npo-1)
        {
            fprintf(fd_write,"%d,",Npo_output[i]);
        }
        else
        {
            fprintf(fd_write,"%d\n",Npo_output[i]);
        }
    }

    for (int i = 0; i <(line_count-1); i++)
    {
        for (int j = 0; j < Npo; j++)
        {
            if (j<Npo-1)
            {
                if (output_array[i*Npo+j]!=2)
                {
                    fprintf(fd_write,"%d,",output_array[i*Npo+j]);
                }
                else
                {
                    fprintf(fd_write,"x,");
                }
                
            }
            else
            {
                if (output_array[i*Npo+j]!=2)
                {
                    fprintf(fd_write,"%d\n",output_array[i*Npo+j]);
                }
                else
                {
                    fprintf(fd_write,"x\n");
                }
            }
        }        
    }
    
// fprintf(fd_write,"start print output_array\n");
//      for (int i = 0; i < (line_count-1); i++)
//     {
//         for (int k = 0; k < Npo; k++)
//         {
//             fprintf(fd_write,"%d,",output_array[i*Npi+k]);
//             //printf("im here");
//         }
//         printf("\n");
        
//     }
// fprintf(fd_write,"print over\n");




    for (int i = 0; i < Nnodes; i++)
        {
            //if (Node[i].fout == 0)
            {
                fprintf(fd_write,"node:%d, value:%d, level:%d, gatetype:%d\n",Node[i].num,Node[i].value,Node[i].level,Node[i].type);
                //fprintf(outputfp,"%d,%d\n",Node[i].num,Node[i].value);
            }
            
        }

    fclose(fd_write);

    free(input_array);
    free(Npo_output);
    free(Npi_input);
    free(output_array);
}

