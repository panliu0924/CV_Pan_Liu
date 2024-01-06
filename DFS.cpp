#include "readckt.h"

void rtpg(){
    FILE *fd;
    cp[strlen(cp)-1] = '\0';
    char* tp_count_s;
    int tp_count;
    char* mode;
    char* tp_file;
    tp_count_s= strtok(cp," ");
    tp_count = atoi(tp_count_s);
    mode = strtok(NULL," ");
    
    tp_file = strtok(NULL," ");
    if((fd = fopen(tp_file,"w")) == NULL){
        printf("File does not exist!\n");
        return;
    }

    NSTRUC* np;
    for(int i = 0; i < Npi; i++){
        fprintf(fd, "%d",Pinput[i]->num);
        if((i+1) != Npi)
            fprintf(fd,",");
    }
    fprintf(fd,"\n");

    int val;
    srand(time(NULL));
    for(int i = 0; i < tp_count; i++){
        for(int j = 0; j < Npi; j++){
            if(strcmp(mode,"b") == 0){
                val = rand() %2;
            }
            else if(strcmp(mode,"t") == 0){
                val = rand() %3;
            }
            if(val == 2){
                fprintf(fd,"x");
            }
            else{
                fprintf(fd,"%d",val);
            }
            if((j+1) != Npi)
                fprintf(fd,",");
        }
        if((i+1) != tp_count)
            fprintf(fd,"\n");
    }
    fclose(fd);
}

void rfl(){
    FILE *fd;
    cp[strlen(cp)-1] = '\0';
    if((fd = fopen(cp,"w")) == NULL){
        printf("File does not exist!\n");
        return;
    }
    NSTRUC* np;
    vector<int> ssa_zero;
    vector<int> ssa_one;
    int node_num;
    int stucK_at_val;
    for (int i = 0; i < Nnodes; i++)
    {
        np = &Node[i];
        
        if(np->type == 0){
            // current node is PI, check if it has fanout
            ssa_zero.push_back(np->num);
            ssa_one.push_back(np->num);
        }
        if(np->type == 1){
            // current node is branch
            ssa_zero.push_back(np->num);
            ssa_one.push_back(np->num);
        }
    }
    for (int i = 0; i < ssa_zero.size(); i++)
    {
        fprintf(fd,"%d@0\n",ssa_zero[i]);
    }
    for (int i = 0; i < ssa_one.size(); i++)
    {
        fprintf(fd,"%d@1\n",ssa_one[i]);
        //if(i+1 != ssa_one.size()) fprintf(fd,"\n");
    }
    fclose(fd);        
}

pair<vector<int>,vector<int>> rfl_for_atpg(){
    NSTRUC* np;
    pair<vector<int>,vector<int>> ssa_zero_one;
    int node_num;
    int stucK_at_val;
    FILE *fd;
    if((fd = fopen("rfl_atpg.txt","w")) == NULL){
        printf("File does not exist!\n");
        return ssa_zero_one ;
    }

    for (int i = 0; i < Nnodes; i++)
    {
        np = &Node[i];
        
        if(np->type == 0){
            // current node is PI, check if it has fanout
            ssa_zero_one.first.push_back(np->num);
            ssa_zero_one.second.push_back(np->num);
        }
        if(np->type == 1){
            // current node is branch
            ssa_zero_one.first.push_back(np->num);
            ssa_zero_one.second.push_back(np->num);
        }
    }
    for (int i = 0; i < ssa_zero_one.first.size(); i++)
    {
        fprintf(fd,"%d@0\n",ssa_zero_one.first[i]);
    }
    for (int i = 0; i < ssa_zero_one.second.size(); i++)
    {
        fprintf(fd,"%d@1\n",ssa_zero_one.second[i]);
        //if(i+1 != ssa_one.size()) fprintf(fd,"\n");
    }
    fclose(fd);
    return ssa_zero_one;
}

pair<vector<int>,vector<int>> complete_fault_list(){
    NSTRUC* np;
    pair<vector<int>,vector<int>> ssa_zero_one;
    int node_num;
    int stucK_at_val;
    FILE *fd;
    if((fd = fopen("complete_fault_list.txt","w")) == NULL){
        printf("File does not exist!\n");
        return ssa_zero_one ;
    }

    for (int i = 0; i < Nnodes; i++)
    {
        np = &Node[i];
        ssa_zero_one.first.push_back(np->num);
        ssa_zero_one.second.push_back(np->num);
    }
    for (int i = 0; i < ssa_zero_one.first.size(); i++)
    {
        fprintf(fd,"%d@0\n",ssa_zero_one.first[i]);
    }
    for (int i = 0; i < ssa_zero_one.second.size(); i++)
    {
        fprintf(fd,"%d@1\n",ssa_zero_one.second[i]);
        //if(i+1 != ssa_one.size()) fprintf(fd,"\n");
    }
    fclose(fd);
    return ssa_zero_one;
}











void dfs_single(vector<int>input_nodes, vector<int>input_vals){//This is for a single test pattern
    logicsim_simple(input_nodes, input_vals);
    // cp[strlen(cp)-1] = '\0';
    std::list<int> node_list;
    NSTRUC *np, *np_in;
    for (int f = 0; f <= max_level; f++){
        for (int i= 0; i < Nnodes; i++){
            if (Node[i].level == f){
            node_list.push_back(Node[i].indx);//Create Node_list, in level order
            }
        }
    }
    //To get 
    //We need to create a fault list for every node in the circuit
    //there are Nnodes number of nodes with each having a vector of size 1
    std::vector<std::vector<int>> Faults_Covered(Nnodes, std::vector<int>(1));
    //Faults that are covered by the node
    //Now pick out every PI, fault_list.push_back

    for (int i = 0; i < Nnodes; i++)//iterate through all nodes to create fault list for every node
    {    
    int fault;
        Node[i].fault.clear();
            if (Node[i].value == 0){
                fault = 1;
                Node[i].fault.push_back(std::make_pair(Node[i].num, fault));
            }
            if (Node[i].value == 1){
                fault = 0;
                Node[i].fault.push_back(std::make_pair(Node[i].num, fault));
            }
            if (Node[i].value == 2){
                Node[i].fault.clear();        
            }
    }
    //Now iterate through the node_list to check for controlling value
while (!node_list.empty()) {
    for (int i =0; i < Nnodes; i++)
    {
        if (Node[i].level == 0){ //This is PI, Already have Fault_List
            node_list.pop_front();//pop node from the node_list
        }
        else {
          np = &Node[i];
          int x_flag = 0;
          int c_flag = 0;
             //Which node contains the controlling value, so we 
            for (int j = 0; j<np->fin; j++){ //Input nodes
                int c;
                if (np->type == BRCH){
                    for (int k=0; k<np->fin;k++){ //Find all the Fan in nodes ( Should be only one)
                       np_in = np->unodes[k];
                       for (const std::pair<int, int>& element : np_in->fault) { //Find the faults in the fault_list of the Fan_in Node
                           Node[i].fault.push_back(element); //Give the fault to the current node by push_back
                           //Now the brch has the fault of itself and of the stem(fan_in)
                            }
                    }
                }
                if (np->type == BUF){
                     for (int k=0; k<np->fin;k++){ //Find all the Fan in nodes (SHould be only one)
                       np_in = np->unodes[k];
                       for (const std::pair<int, int>& element : np_in->fault) { //Find the faults in the fault_list of the Fan_in Node
                           np->fault.push_back(element); //Give the fault to the current node by push_back
                           //Now the BUFFER has the fault of itself and of the fan_in
                            }
                    }
                }
                if (np->type == NOT){
                    for (int k=0; k<np->fin;k++){ //Find all the Fan in nodes ( Should be only one)
                       np_in = np->unodes[k];
                       for (const std::pair<int, int>& element : np_in->fault) { //Find the faults in the fault_list of the Fan_in Node
                           np->fault.push_back(element); //Give the fault to the current node by push_back
                           //Now the NOT gate has the fault of itself and of the fan_in
                            }
                    }
                }
                if (np->type == OR){//Gate is OR
                    c = 1;  x_flag = 0; c_flag = 0; //c=control value for the gate
                    //x and c flag means containing X and Control Value
                    std::vector<int> control_node;
                    std::vector<int> non_control_node;
                    for (int k=0; k<np->fin;k++){//Need to check all fin nodes
                    np_in = np->unodes[k];//Need to process fin node values and faults
                        if(np_in->value == 2) x_flag = 1; //Contains X
                        if(np_in->value == c) {  //Contains a Controlling Value
                        c_flag = 1;
                        control_node.push_back(np_in->indx);//Need to add the Node indx of this Fin to the Control_node
                        //the control_nodes node indexes are to be locating fault list 
                        }
                        else{
                            non_control_node.push_back(np_in->indx);
                        }
                    } 
                    if(x_flag == 0){
                        if(c_flag == 1){
                            std::vector<std::pair<int, int>> intersection;
                            std::vector<std::pair<int, int>> current_faults;
                            for (int f = 0; f < control_node.size(); f++) {  //Access all the control node index
                                int control_indx = control_node[f];
                                current_faults = Node[control_indx].fault;
                                std::sort(current_faults.begin(), current_faults.end());
                                
                                if (f==0){
                                    intersection = current_faults;
                                }
                                else {
                                    std::vector<std::pair<int, int>> temp_intersection;
                                    std::sort(intersection.begin(), intersection.end());
                                    std::set_intersection(intersection.begin(), intersection.end(),current_faults.begin(), current_faults.end(),std::back_inserter(temp_intersection));
                                    intersection = temp_intersection;
                                } 
                            }//end of intersection of the control input nodes
                            //Now we reduce any possible non_controlling input faults(Need to see if A-B-C = A - AnB - AnC for sets)
                            if (non_control_node.size() != 0){
                                for (auto f = 0; f < non_control_node.size(); f++){
                                    int non_control_indx = non_control_node[f];
                                    std::vector<std::pair<int, int>> inputB_fault;
                                    std::vector<std::pair<int, int>> temp;
                                    inputB_fault = Node[non_control_indx].fault;
                                    std::sort(intersection.begin(), intersection.end());
                                    std::sort(inputB_fault.begin(), inputB_fault.end());
                                    std::set_difference(intersection.begin(), intersection.end(), inputB_fault.begin(), inputB_fault.end(), std::back_inserter(temp));
                                    intersection = temp;
                                    //We Reduce noncontrolling faults from the intersection. In other words delete duplicate
                                    //update it to "intersection"
                                }
                            }
                            //Now we begin Union between the intersected nodes and the fault of the current node
                            std::vector<std::pair<int, int>> temp_union;
                            current_faults = np->fault;
                            std::sort(current_faults.begin(), current_faults.end());
                            std::sort(intersection.begin(), intersection.end());
                            set_union(current_faults.begin(), current_faults.end(),intersection.begin(), intersection.end(), std::back_inserter(temp_union));
                            np->fault = temp_union;
                            //Finish Union, now we have the full list of faults for the node
                        }
                    
                        else{//No controlling values
                             for (int f = 0; f < np->fin; f++) {  //Access all the input node index
                                std::vector<std::pair<int, int>> temp_union;
                                std::vector<std::pair<int, int>> current_faults;
                                std::vector<std::pair<int, int>> input_faults;
                                np_in = np->unodes[f];
                                current_faults = np->fault;
                                input_faults = np_in->fault;
                                std::sort(current_faults.begin(), current_faults.end());
                                std::sort(input_faults.begin(), input_faults.end());
                                std::set_union(current_faults.begin(), current_faults.end(),input_faults.begin(), input_faults.end(), std::back_inserter(temp_union));
                                np->fault = temp_union;//To avoid memory leak
                            }//end of union. We now have the full fault list for the current node
                        }
                        }
                    else {//if x_flag == 1
                        np->fault.clear();
                        }

                    }
                if (np->type == AND){
                    c = 0;  x_flag = 0; c_flag = 0; //c=control value for the gate
                    //x and c flag means containing X and Control Value
                    std::vector<int> control_node;
                    std::vector<int> non_control_node;
                    for (int k=0; k<np->fin;k++){//Need to check all fin nodes
                    np_in = np->unodes[k];//Need to process fin node values and faults
                        if(np_in->value == 2) {x_flag = 1; }//Contains X
                        else if(np_in->value == c) {  //Contains a Controlling Value
                        c_flag = 1;
                        control_node.push_back(np_in->indx);//Need to add the Node indx of this Fin to the Control_node
                        //the control_nodes node indexes are to be locating fault list 
                        }
                        else{
                            non_control_node.push_back(np_in->indx);
                        }
                    } 
                    if(x_flag == 0){
                        if(c_flag == 1){
                            std::vector<std::pair<int, int>> intersection;
                            std::vector<std::pair<int, int>> current_faults;
                            for (int f = 0; f < control_node.size(); f++) {  //Access all the control node index
                                int control_indx = control_node[f];
                                current_faults = Node[control_indx].fault;
                                std::sort(current_faults.begin(), current_faults.end());
                                
                                if (f==0){
                                    intersection = current_faults;
                                }
                                else {
                                    std::vector<std::pair<int, int>> temp_intersection;
                                    std::sort(intersection.begin(), intersection.end());
                                    std::set_intersection(intersection.begin(), intersection.end(),current_faults.begin(), current_faults.end(),std::back_inserter(temp_intersection));
                                    intersection = temp_intersection;
                                } 
                            }//end of intersection of the control input nodes
                            //Now we reduce any possible non_controlling input faults(Need to see if A-B-C = A - AnB - AnC for sets)
                            if (non_control_node.size() != 0){
                                for (int f = 0; f < non_control_node.size(); f++){
                                   int non_control_indx = non_control_node[f];
                                    std::vector<std::pair<int, int>> inputB_fault;
                                    std::vector<std::pair<int, int>> temp;
                                    inputB_fault = Node[non_control_indx].fault;
                                    std::sort(intersection.begin(), intersection.end());
                                    std::sort(inputB_fault.begin(), inputB_fault.end());
                                    std::set_difference(intersection.begin(), intersection.end(), inputB_fault.begin(), inputB_fault.end(), std::back_inserter(temp));
                                    intersection = temp;
                                    //We Reduce noncontrolling faults from the intersection. In other words delete duplicate
                                    //update it to "intersection"
                                }
                            }
                            //Now we begin Union between the intersected nodes and the fault of the current node
                            std::vector<std::pair<int, int>> temp_union;
                            current_faults = np->fault;
                            std::sort(current_faults.begin(), current_faults.end());
                            std::sort(intersection.begin(), intersection.end());
                            set_union(current_faults.begin(), current_faults.end(),intersection.begin(), intersection.end(), std::back_inserter(temp_union));
                            np->fault = temp_union;
                            //Finish Union, now we have the full list of faults for the node
                        }
                    
                        else{//No controlling values
                             for (int f = 0; f < np->fin; f++) {  //Access all the input node index
                                std::vector<std::pair<int, int>> temp_union;
                                std::vector<std::pair<int, int>> current_faults;
                                std::vector<std::pair<int, int>> input_faults;
                                np_in = np->unodes[f];
                                current_faults = np->fault;
                                input_faults = np_in->fault;
                                std::sort(current_faults.begin(), current_faults.end());
                                std::sort(input_faults.begin(), input_faults.end());
                                std::set_union(current_faults.begin(), current_faults.end(),input_faults.begin(), input_faults.end(), std::back_inserter(temp_union));
                                np->fault = temp_union;
                            }//end of union. We now have the full fault list for the current node
                        }
                        }
                    else {//if x_flag == 1
                        np->fault.clear();
                        }
                
                }
            
            if (np->type == NAND){
                    c = 0;  x_flag = 0; c_flag = 0; //c=control value for the gate
                    //x and c flag means containing X and Control Value
                    std::vector<int> control_node;
                    std::vector<int> non_control_node;
                    for (int k=0; k<np->fin;k++){//Need to check all fin nodes
                    np_in = np->unodes[k];//Need to process fin node values and faults
                        if(np_in->value == 2) {x_flag = 1; }//Contains X
                        else if(np_in->value == c) {  //Contains a Controlling Value
                        c_flag = 1;
                        control_node.push_back(np_in->indx);//Need to add the Node indx of this Fin to the Control_node
                        //the control_nodes node indexes are to be locating fault list 
                        }
                        else{
                            non_control_node.push_back(np_in->indx);
                        }
                    } 
                    if(x_flag == 0){
                        if(c_flag == 1){
                            std::vector<std::pair<int, int>> intersection;
                            std::vector<std::pair<int, int>> current_faults;    
                            for (int f = 0; f < control_node.size(); f++) {  //Access all the control node index
                                int control_indx = control_node[f];
                                current_faults = Node[control_indx].fault;
                                std::sort(current_faults.begin(), current_faults.end());
                                
                                if (f==0){
                                    intersection = current_faults;
                                }
                                else {
                                    std::vector<std::pair<int, int>> temp_intersection;
                                    std::sort(intersection.begin(), intersection.end());
                                    std::set_intersection(intersection.begin(), intersection.end(),current_faults.begin(), current_faults.end(),std::back_inserter(temp_intersection));
                                    intersection = temp_intersection;
                                } 
                            }//end of intersection of the control input nodes
                            //Now we reduce any possible non_controlling input faults(Need to see if A-B-C = A - AnB - AnC for sets)
                            if (non_control_node.size() != 0){
                                for (int f = 0; f < non_control_node.size(); f++){
                                    int non_control_indx = non_control_node[f];
                                    std::vector<std::pair<int, int>> inputB_fault;
                                    std::vector<std::pair<int, int>> temp;
                                    inputB_fault = Node[non_control_indx].fault;
                                    std::sort(intersection.begin(), intersection.end());
                                    std::sort(inputB_fault.begin(), inputB_fault.end());
                                    std::set_difference(intersection.begin(), intersection.end(), inputB_fault.begin(), inputB_fault.end(), std::back_inserter(temp));
                                    intersection = temp;
                                    //We Reduce noncontrolling faults from the intersection. In other words delete duplicate
                                    //update it to "intersection"
                                }
                            }
                            //Now we begin Union between the intersected nodes and the fault of the current node
                            std::vector<std::pair<int, int>> temp_union;
                            current_faults = np->fault;
                            std::sort(current_faults.begin(), current_faults.end());
                            std::sort(intersection.begin(), intersection.end());
                            set_union(current_faults.begin(), current_faults.end(),intersection.begin(), intersection.end(), std::back_inserter(temp_union));
                            np->fault = temp_union;
                            //Finish Union, now we have the full list of faults for the node
                        }
                    
                        else{//No controlling values
                             for (int f = 0; f < np->fin; f++) {  //Access all the input node index
                                std::vector<std::pair<int, int>> temp_union;
                                std::vector<std::pair<int, int>> input_faults;
                                std::vector<std::pair<int, int>> current_faults;
                                current_faults = np->fault;
                                np_in = np->unodes[f];
                                input_faults = np_in->fault;
                                std::sort(current_faults.begin(), current_faults.end());
                                std::sort(input_faults.begin(), input_faults.end());
                                std::set_union(current_faults.begin(), current_faults.end(),input_faults.begin(), input_faults.end(), std::back_inserter(temp_union));
                                np->fault = temp_union;
                            }//end of union. We now have the full fault list for the current node
                        }
                        }
                    else {//if x_flag == 1
                        np->fault.clear();
                        }
                
                }
                if (np->type == NOR){
                    c = 1;  x_flag = 0; c_flag = 0; //c=control value for the gate
                    //x and c flag means containing X and Control Value
                    std::vector<int> control_node;
                    std::vector<int> non_control_node;
                    for (int k=0; k<np->fin;k++){//Need to check all fin nodes
                    np_in = np->unodes[k];//Need to process fin node values and faults
                        if(np_in->value == 2) {x_flag = 1; }//Contains X
                        else if(np_in->value == c) {  //Contains a Controlling Value
                        c_flag = 1;
                        control_node.push_back(np_in->indx);//Need to add the Node indx of this Fin to the Control_node
                        //the control_nodes node indexes are to be locating fault list 
                        }
                        else{
                            non_control_node.push_back(np_in->indx);
                        }
                    } 
                    if(x_flag == 0){
                        if(c_flag == 1){
                            std::vector<std::pair<int, int>> intersection;
                            std::vector<std::pair<int, int>> current_faults;
                            for (int f = 0; f < control_node.size(); f++) {  //Access all the control node index
                                int control_indx = control_node[f];
                                current_faults= Node[control_indx].fault;
                                std::sort(current_faults.begin(), current_faults.end());
                                
                                if (f==0){
                                    intersection = current_faults;
                                }
                                else {
                                    std::vector<std::pair<int, int>> temp_intersection;
                                    std::sort(intersection.begin(), intersection.end());
                                    std::set_intersection(intersection.begin(), intersection.end(),current_faults.begin(), current_faults.end(),std::back_inserter(temp_intersection));
                                    intersection = temp_intersection;
                                } 
                            }//end of intersection of the control input nodes
                            //Now we reduce any possible non_controlling input faults(Need to see if A-B-C = A - AnB - AnC for sets)
                            if (non_control_node.size() != 0){
                                for (int f = 0; f < non_control_node.size(); f++){
                                   int non_control_indx = non_control_node[f];
                                    std::vector<std::pair<int, int>> inputB_fault;
                                    std::vector<std::pair<int, int>> temp;
                                    inputB_fault = Node[non_control_indx].fault;
                                    std::sort(intersection.begin(), intersection.end());
                                    std::sort(inputB_fault.begin(), inputB_fault.end());
                                    std::set_difference(intersection.begin(), intersection.end(), inputB_fault.begin(), inputB_fault.end(), std::back_inserter(temp));
                                    intersection = temp;
                                    //We Reduce noncontrolling faults from the intersection. In other words delete duplicate
                                    //update it to "intersection"
                                }
                            }
                            //Now we begin Union between the intersected nodes and the fault of the current node
                            std::vector<std::pair<int, int>> temp_union;
                            current_faults = np->fault;
                            std::sort(current_faults.begin(), current_faults.end());
                            std::sort(intersection.begin(), intersection.end());
                            set_union(current_faults.begin(), current_faults.end(),intersection.begin(), intersection.end(), std::back_inserter(temp_union));
                            np->fault = temp_union;
                            //Finish Union, now we have the full list of faults for the node
                        }
                    
                        else{//No controlling values
                             for (int f = 0; f < np->fin; f++) {  //Access all the input node index
                                std::vector<std::pair<int, int>> temp_union;
                                std::vector<std::pair<int, int>> current_faults;
                                std::vector<std::pair<int, int>> input_faults;
                                np_in = np->unodes[f];
                                current_faults = np->fault;
                                input_faults = np_in->fault;
                                std::sort(current_faults.begin(), current_faults.end());
                                std::sort(input_faults.begin(), input_faults.end());
                                std::set_union(current_faults.begin(), current_faults.end(),input_faults.begin(), input_faults.end(), std::back_inserter(temp_union));
                                np->fault = temp_union;
                            }//end of union. We now have the full fault list for the current node
                        }
                        }
                    else {//if x_flag == 1
                        np->fault.clear();
                        }
                
                }
                if (np->type == XOR){
                    x_flag = 0;
                    std::vector<int> input_faults;
                    for (int k=0; k<np->fin;k++){//Need to check all fin nodes
                    np_in = np->unodes[k];//Need to process fin node values and faults
                        if(np_in->value == 2) {x_flag = 1; }//Contains X
                        else {int np_in_indx = np_in->indx;
                            input_faults.push_back(np_in_indx);}
                    }    
                    if(x_flag == 0){
                            std::vector<std::pair<int, int>> temp_union;
                            std::vector<std::pair<int, int>> temp_faults;
                            std::vector<std::pair<int, int>> current_faults;
                            for (int f = 0; f < input_faults.size(); f++) {  //Access all the control node index
                                int control_indx = input_faults[f];
                                current_faults = Node[control_indx].fault;
                                std::sort(current_faults.begin(), current_faults.end());
                                if (f==0){
                                    temp_faults = current_faults;
                                }
                                else {
                                    std::vector<std::pair<int, int>> temp_difference;
                                    std::sort(temp_faults.begin(), temp_faults.end());
                                    std::set_difference(temp_faults.begin(), temp_faults.end(),temp_faults.begin(), temp_faults.end(),std::back_inserter(temp_difference));
                                    temp_faults = temp_difference;//Ready to be processed
                                    //We have Union - Intersection loop here
                                } 

                            }//end of intersection of the control input nodes
                            //Now we reduce any possible non_controlling input faults(Need to see if A-B-C = A - AnB - AnC for sets)
                            //Now we begin Union between the intersected nodes and the fault of the current node
                            std::sort(current_faults.begin(), current_faults.end());
                            std::sort(temp_faults.begin(), temp_faults.end());
                            set_union(current_faults.begin(), current_faults.end(),temp_faults.begin(), temp_faults.end(), std::back_inserter(temp_union));
                            np->fault = temp_union;
                            //Finish Union, now we have the full list of faults for the node
                        
                        }
                    else {//if x_flag == 1
                        np->fault.clear();
                        }
                
                }

                if (np->type == XNOR){
                    x_flag = 0;
                    std::vector<int> input_faults;
                    for (int k=0; k<np->fin;k++){//Need to check all fin nodes
                    np_in = np->unodes[k];//Need to process fin node values and faults
                        if(np_in->value == 2) {x_flag = 1; }//Contains X
                        else {input_faults.push_back(np_in->indx);}
                    }    
                    if(x_flag == 0){
                            std::vector<std::pair<int, int>> temp_union;
                            std::vector<std::pair<int, int>> temp_faults;
                            for (int f = 0; f < input_faults.size(); f++) {  //Access all the control node index
                                int control_indx = input_faults[f];
                                std::vector<std::pair<int, int>>& current_faults = Node[control_indx].fault;
                                std::sort(current_faults.begin(), current_faults.end());
                                
                                if (f==0){
                                    temp_faults = current_faults;
                                }
                                else {
                                    std::vector<std::pair<int, int>> temp_intersection;
                                    std::vector<std::pair<int, int>> temp_difference;
                                    std::sort(temp_faults.begin(), temp_faults.end());
                                    std::set_intersection(temp_faults.begin(), temp_faults.end(),current_faults.begin(), current_faults.end(),std::back_inserter(temp_difference));
                                    temp_faults = temp_difference;//Ready to be processed
                                    //We have Union - Intersection loop here
                                } 

                            }//end of intersection of the control input nodes
                            //Now we reduce any possible non_controlling input faults(Need to see if A-B-C = A - AnB - AnC for sets)
                            //Now we begin Union between the intersected nodes and the fault of the current node
                            std::vector<std::pair<int, int>> current_faults;
                            current_faults = np->fault;
                            std::sort(current_faults.begin(), current_faults.end());
                            std::sort(temp_faults.begin(), temp_faults.end());
                            set_union(current_faults.begin(), current_faults.end(),temp_faults.begin(), temp_faults.end(), std::back_inserter(temp_union));
                            np->fault = temp_union;
                            //Finish Union, now we have the full list of faults for the node
                        
                        }
                    else {//if x_flag == 1
                        np->fault.clear();
                        }
                
                }
            //DO the same with NAND, NOR, AND, OR gate, with different controlling values
            //Different for XOR, XNOR gate, need to check both input and output
                //for Xor, 
                    //if output == 0
                        //for input nodes
            //Always C = 1 for not gate and brch

    }
    node_list.pop_front();
    
}
}

}
}

void dfs(){
    lev();
    // cp[strlen(cp)-1] = '\0';
    // char* input_file;
    std::string input_file;
    std::string output_file;
    // input_file= strtok(cp," ");
    input_file = std::string(cp).substr(0, std::string(cp).find(" "));
    output_file = std::string(cp).substr(std::string(cp).find(" ") + 1);
    // std::cout << cp << " " << input_file << " " << output_file << std::endl;
    NSTRUC *np;
    // if (!outfile.is_open()) {
    //     std::cerr << "Error opening file dfs_fault.txt for writing." << std::endl;
    //     return;
    // }
    
    ifstream in_file(input_file);
    std::vector<std::pair<int, int>> all_faults;

    string line;
    int line_count = 0;
    getline(in_file,line);
    istringstream iss(line);
    string node_str;
    int node_num;
    vector<int> input_nodes;//for loop this to identify Node_ID
    vector<vector<int>> all_tp;//for loop in the loop above to identify Value for Each Node_ID
	//node_num = input_nodes.at(0)
	//np = &nodes[node_num]
	//np->value = tp.at(0)

    while (std::getline(iss, node_str, ',')) {
        // get one row of input file and read input nodes
        node_num = stoi(node_str);
        input_nodes.push_back(node_num);
    }
    
    while (getline(in_file,line))
    {
        istringstream vector_stream(line);
        string input_val;
        vector<int> input_vals;


        while (std::getline(vector_stream, input_val, ',')) {
        // get one row of input file and read input nodes
            if(input_val.compare("x") != 0){
                input_vals.push_back(stoi(input_val));
            }
            else{
                input_vals.push_back(2);
            }
        }
        dfs_single(input_nodes, input_vals);
        input_vals.clear();

        // for (int i = 0; i < Nnodes; i++){
        //     np = &Node[i];
        //     if(Node[i].level = max_level){
        //             std::vector<std::pair<int, int>> element;
        //             element= np->fault;
        //             std::sort(element.begin(), element.end());
        //             std::vector<std::pair<int, int>> temp_faults;
        //             std::sort(all_faults.begin(), all_faults.end());
        //             all_faults.insert(all_faults.end(), element.begin(), element.end());
        //         }
        //     }

        for(int i = 0; i < Npo; i++){
            std::sort(Poutput[i]->fault.begin(), Poutput[i]->fault.end());
            std::sort(all_faults.begin(), all_faults.end());
            std::vector<std::pair<int, int>> temp_union;
            std::set_union(all_faults.begin(), all_faults.end(), Poutput[i]->fault.begin(), Poutput[i]->fault.end(), back_inserter(temp_union));
            all_faults = temp_union;
        }

        // for (int i = 0; i < Npo; i++){
        //     std::cout << Poutput[i]->num << " ";
        //     for (int j = 0; j<Poutput[i]->fault.size(); j++){
        //         std::cout << Poutput[i]->fault[j].first<<"@"<< Poutput[i]->fault[j].second << ", ";
        //     }
        //     std::cout << "\n";
        // }
            
        
        all_tp.push_back(input_vals);
    }
    
    std::ofstream outfile(output_file);
    for (auto faults :all_faults){
          outfile << faults.first <<  "@" << faults.second << "\n";
        //  std::cout << faults.first <<  "@" << faults.second << "\n"; 
    }
    
    // for (int i = 0; i < Nnodes; i++){
    //     std::cout << Node[i].num << " ";
    //     for (int j = 0; j<Node[i].fault.size(); j++){
    //         std::cout << Node[i].fault[j].first<<"@"<< Node[i].fault[j].second << ", ";
    //     }
    //     std::cout << "\n";
    // }
    // for (auto elem :all_tp)
    // {  
    //     for(auto tp : elem){
    //         cout << tp << ',';
    //     }
    //      cout << endl;
    // }
    
}