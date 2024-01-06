#include "readckt.h"

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

size_t node_eval_pfs(NSTRUC *np){
    if (np->type == BRCH)//there is no way we will find PI because PI is in lev0, we start from lev1;
    {
        np->value_pfs =  np->unodes[0]->value_pfs;
    }
    
    else if (np->type == XOR)
    {
        np->value_pfs = np->unodes[0]->value_pfs;
       
            for (int k = 1; k < np->fin; k++)
            {
                np->value_pfs = np->value_pfs ^(np->unodes[k]->value_pfs);                                
            }
        
    }
    else if (np->type == OR)
    {
        np->value_pfs = np->unodes[0]->value_pfs;
        for (int k = 0; k < np->fin; k++)
        {
        np->value_pfs = np->value_pfs | (np->unodes[k]->value_pfs);
        }
    }
    else if (np->type == NOR)
    {
        np->value_pfs = np->unodes[0]->value_pfs;
        for (int k = 0; k < np->fin; k++)
        {
        np->value_pfs = np->value_pfs | (np->unodes[k]->value_pfs);
        }
        np->value_pfs = ~np->value_pfs;
    }
    else if (np->type == NOT)
    {
    np->value_pfs = ~np->unodes[0]->value_pfs;
    }
    else if (np->type == NAND)
    {
        np->value_pfs = np->unodes[0]->value_pfs;
        for (int k = 0; k < np->fin; k++)
        {
        np->value_pfs = np->value_pfs & (np->unodes[k]->value_pfs);
        }
        np->value_pfs = ~np->value_pfs;
    }

    else if (np->type == AND)
    {
        np->value_pfs = np->unodes[0]->value_pfs;
        for (int k = 0; k < np->fin; k++)
        {
        np->value_pfs = np->value_pfs & (np->unodes[k]->value_pfs);
        }
    }
    else if (np->type == XNOR)
    {
        np->value_pfs = np->unodes[0]->value_pfs;
            for (int k = 1; k < np->fin; k++)
            {
                np->value_pfs = np->value_pfs ^(np->unodes[k]->value_pfs);                                
            }
        np->value_pfs = ~np->value_pfs;
    } 
    else if (np->type == BUF)
    {
        np->value_pfs = np->unodes[0]->value_pfs;
    }  
}   

void pfs_single(vector<int> input_nodes, vector<int> input_vals, std::unordered_map<int, size_t[2]> fault_mask){
NSTRUC *np;
std::list<int> node_list;
int max_level = 0;
for (int i = 0;i <Nnodes;i++)
    {
        if(max_level < Node[i].level){
            max_level = Node[i].level;
        }
    }
//Primary Inputs
for (int f = 0; f <= max_level; f++){
        for (int i= 0; i < Nnodes; i++){
            if (Node[i].level == f){
            node_list.push_back(Node[i].indx);//Create Node_list, in level order
            }
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
                        Node[j].value = input_vals.at(k);
                    }                    
                }                
            }            
        }

while (!node_list.empty()){
    int node_list_indx = node_list.front();
    np = &Node[node_list_indx];
    if (Node[node_list_indx].level == 0){

        if (Node[node_list_indx].value == 1){
            Node[node_list_indx].value_pfs = std::numeric_limits<size_t>::max();
        }
        
        else{
            Node[node_list_indx].value_pfs = 0;
        }
    
        if (fault_mask.find(Node[node_list_indx].num) != fault_mask.end())
            {
            size_t and_mask = fault_mask[Node[node_list_indx].num][0];
            size_t or_mask = fault_mask[Node[node_list_indx].num][1];
            Node[node_list_indx].value_pfs = (Node[node_list_indx].value_pfs & and_mask) | or_mask;
        }
    node_list.pop_front();
    }
    else{//Rest of Nodes
        node_eval_pfs(np);
  
        if (fault_mask.find(Node[node_list_indx].num) != fault_mask.end()){
            size_t and_mask = fault_mask[Node[node_list_indx].num][0];
            size_t or_mask = fault_mask[Node[node_list_indx].num][1];
            Node[node_list_indx].value_pfs = (Node[node_list_indx].value_pfs & and_mask) | or_mask;
        }
    node_list.pop_front();
    }

}

}
void pfs_atpg_tpg(){
    NSTRUC *np;
    std::string input_file_tp = "podem_atpg_tpg.txt";
    std::string input_file_fl = "fault_list_for_atpg.txt";
    std::string output_file = "fault_list_after_atpg.txt";
    int width;
    width = sizeof(size_t)*8;
    int node_num;
    //Extract File Path
    int first_space, second_space;
    // cp[strlen(cp) - 1] = '\0';
    // std::string cp_string = std::string(cp);
    // first_space = cp_string.find(" ");
    // input_file_tp = cp_string.substr(0, first_space);
    // second_space = cp_string.find(" ", first_space + 1);
    // input_file_fl = cp_string.substr(first_space + 1, second_space - first_space - 1);
    // output_file = cp_string.substr(second_space + 1);

    std::ifstream infile_tp(input_file_tp);
    std::ifstream infile_fl(input_file_fl);
    std::ofstream outfile(output_file);
    vector<int>input_nodes;
    string line;
    getline(infile_tp,line);
    istringstream iss(line);
    string node_str;
    while (std::getline(iss, node_str, ',')) {
            // get one row of input file and read input nodes
            node_num = stoi(node_str);
            input_nodes.push_back(node_num);
    }
    size_t fault_detected = 0;
    std::string fault;
    std::string fault_num[width - 1];
    std::unordered_map<int, size_t[2]> fault_masks;
    lev();
    std::unordered_set<std::string> uniqueFaults;
    while (getline(infile_tp,line)){
        istringstream vector_stream(line);
        string input_val;
        vector<int> input_vals;
        int fault_count = 0;
        fault_detected = 0;
        fault_masks.clear();
        while (std::getline(vector_stream, input_val, ',')) {
            if(input_val.compare("x") != 0){
                input_vals.push_back(stoi(input_val));
            }
            else{
                input_vals.push_back(2);
            }
        }
        
        infile_fl.clear();
        infile_fl.seekg(0, std::ios::beg);
        while (std::getline(infile_fl, fault)) {
            if (!fault.empty() && fault.back() == '\n') {
                fault.pop_back();
            }
            if (fault[0] == '\0'){
                break;
            }
            if (fault_count == width - 1){
                pfs_single(input_nodes, input_vals, fault_masks);
                //Initialize Mask
                for (int i = 0; i < Npo; i++){
                    if (Poutput[i]->value_pfs & 1){
                        fault_detected |= std::numeric_limits<size_t>::max()^Poutput[i]->value_pfs;;
                    }
                    else{
                        fault_detected |= 0 ^ Poutput[i]->value_pfs;
                    }
                }
                for (int i = 0; i < fault_count; i++){
                    if ((fault_detected >> (i + 1)) & 1) {
                        uniqueFaults.insert(fault_num[i]);
                    }
                }
                //Reset
                fault_count = 0;
                fault_detected = 0;
                fault_masks.clear();
            }
            //initialize fault_mask 
            fault_num[fault_count] = std::string(fault);
            std::istringstream iss(fault);
            std::string node_num_str;
            std::getline(iss, node_num_str, '@');
            int node_num = std::stoi(node_num_str);
            //if (fault_masks.count(node_num) > 0)
    
            if (fault_masks.find(node_num) == fault_masks.end())
            {
                fault_masks[node_num][0] = std::numeric_limits<size_t>::max();
                fault_masks[node_num][1] = 0;
            }
            //Or/And Mask Calculation and Data Handling
            size_t fault_mask_bit;
            if (!fault.empty() && fault.back() == '0'){
                    fault_mask_bit = ~(size_t(1) << (fault_count + 1));
                    fault_masks[node_num][0] &= fault_mask_bit;
                }
            else {
                    fault_mask_bit = (size_t(1) << (fault_count + 1));
                    fault_masks[node_num][1] |= fault_mask_bit;
                }
            fault_count++;
        }
        pfs_single(input_nodes, input_vals, fault_masks);
        
        for (int i = 0; i < Npo; i++){
            // std::cout << Poutput[i]->value_pfs << std::endl;
            if (Poutput[i]->value_pfs & 1){
                fault_detected |= std::numeric_limits<size_t>::max()^Poutput[i]->value_pfs;;
                    }
            else{
                fault_detected |= 0 ^ Poutput[i]->value_pfs;
                    }
        }
       
        for (int i = 0; i < fault_count; i++){
            if ((fault_detected >> (i + 1)) & 1) {
                    uniqueFaults.insert(fault_num[i]);
            }
        }
        //for(int i = 0; i < Nnodes; i ++){
          //  np = &Node[i];
            //std::cout << np->num << " " << np->value_pfs << std::endl;
        //}
        infile_fl.clear();  // Clear any error flags
        infile_fl.seekg(0, std::ios::beg);
        input_vals.clear();
    }
 for (const auto& element : uniqueFaults) {
        outfile << element << std::endl;
        }

        PFS_Fault_Count = uniqueFaults.size();
       // std::cout << PFS_Fault_Count << std::endl;
    infile_tp.close();
    infile_fl.close();
    outfile.close();
}

void pfs_tpg(std::unordered_set<std::string> uniqueFaults){
    NSTRUC *np;
    std::string input_file_tp = "random_pattern.txt";
    std::string input_file_fl = "complete_fault_list.txt";
    std::string output_file = "fault_list_remain.txt";
    int width;
    width = sizeof(size_t)*8;
    int node_num;
    //Extract File Path
    int first_space, second_space;
    // cp[strlen(cp) - 1] = '\0';
    // std::string cp_string = std::string(cp);
    // first_space = cp_string.find(" ");
    // input_file_tp = cp_string.substr(0, first_space);
    // second_space = cp_string.find(" ", first_space + 1);
    // input_file_fl = cp_string.substr(first_space + 1, second_space - first_space - 1);
    // output_file = cp_string.substr(second_space + 1);

    std::ifstream infile_tp(input_file_tp);
    std::ifstream infile_fl(input_file_fl);
    std::ofstream outfile(output_file);
    vector<int>input_nodes;
    string line;
    getline(infile_tp,line);
    istringstream iss(line);
    string node_str;
    while (std::getline(iss, node_str, ',')) {
            // get one row of input file and read input nodes
            node_num = stoi(node_str);
            input_nodes.push_back(node_num);
    }
    size_t fault_detected = 0;
    std::string fault;
    std::string fault_num[width - 1];
    std::unordered_map<int, size_t[2]> fault_masks;
    lev();
    std::unordered_set<std::string> uniqueFaults;
    while (getline(infile_tp,line)){
        istringstream vector_stream(line);
        string input_val;
        vector<int> input_vals;
        int fault_count = 0;
        fault_detected = 0;
        fault_masks.clear();
        while (std::getline(vector_stream, input_val, ',')) {
            if(input_val.compare("x") != 0){
                input_vals.push_back(stoi(input_val));
            }
            else{
                input_vals.push_back(2);
            }
        }
        
        infile_fl.clear();
        infile_fl.seekg(0, std::ios::beg);
        while (std::getline(infile_fl, fault)) {
            if (!fault.empty() && fault.back() == '\n') {
                fault.pop_back();
            }
            if (fault[0] == '\0'){
                break;
            }
            if (fault_count == width - 1){
                pfs_single(input_nodes, input_vals, fault_masks);
                //Initialize Mask
                for (int i = 0; i < Npo; i++){
                    if (Poutput[i]->value_pfs & 1){
                        fault_detected |= std::numeric_limits<size_t>::max()^Poutput[i]->value_pfs;;
                    }
                    else{
                        fault_detected |= 0 ^ Poutput[i]->value_pfs;
                    }
                }
                for (int i = 0; i < fault_count; i++){
                    if ((fault_detected >> (i + 1)) & 1) {
                        uniqueFaults.insert(fault_num[i]);
                    }
                }
                //Reset
                fault_count = 0;
                fault_detected = 0;
                fault_masks.clear();
            }
            //initialize fault_mask 
            fault_num[fault_count] = std::string(fault);
            std::istringstream iss(fault);
            std::string node_num_str;
            std::getline(iss, node_num_str, '@');
            int node_num = std::stoi(node_num_str);
            //if (fault_masks.count(node_num) > 0)
    
            if (fault_masks.find(node_num) == fault_masks.end())
            {
                fault_masks[node_num][0] = std::numeric_limits<size_t>::max();
                fault_masks[node_num][1] = 0;
            }
            //Or/And Mask Calculation and Data Handling
            size_t fault_mask_bit;
            if (!fault.empty() && fault.back() == '0'){
                    fault_mask_bit = ~(size_t(1) << (fault_count + 1));
                    fault_masks[node_num][0] &= fault_mask_bit;
                }
            else {
                    fault_mask_bit = (size_t(1) << (fault_count + 1));
                    fault_masks[node_num][1] |= fault_mask_bit;
                }
            fault_count++;
        }
        pfs_single(input_nodes, input_vals, fault_masks);
        
        for (int i = 0; i < Npo; i++){
            // std::cout << Poutput[i]->value_pfs << std::endl;
            if (Poutput[i]->value_pfs & 1){
                fault_detected |= std::numeric_limits<size_t>::max()^Poutput[i]->value_pfs;;
                    }
            else{
                fault_detected |= 0 ^ Poutput[i]->value_pfs;
                    }
        }
       
        for (int i = 0; i < fault_count; i++){
            if ((fault_detected >> (i + 1)) & 1) {
                    uniqueFaults.insert(fault_num[i]);
            }
        }
        //for(int i = 0; i < Nnodes; i ++){
          //  np = &Node[i];
            //std::cout << np->num << " " << np->value_pfs << std::endl;
        //}
        infile_fl.clear();  // Clear any error flags
        infile_fl.seekg(0, std::ios::beg);
        input_vals.clear();
    }
 for (const auto& element : uniqueFaults) {
        outfile << element << std::endl;
        }

        PFS_Fault_Count = uniqueFaults.size();
       // std::cout << PFS_Fault_Count << std::endl;
    infile_tp.close();
    infile_fl.close();
    outfile.close();
}

void rtpg_tpg(){
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
    FILE* outputFile = fopen("random_pattern.txt", "w");
        if (!outputFile) {
    }
    FILE* inputFile = fopen(tp_file, "r");
        if (!inputFile) {
        fclose(outputFile);
        return;
    }
    char buffer[2048];
    while (fgets(buffer, sizeof(buffer), inputFile) != nullptr) {
        fprintf(outputFile, "%s", buffer);
    }
    fclose(outputFile);
}
void scoap(){
    FILE *fd;
    int max_level =0;
    cp[strlen(cp)-1] = '\0';

    if((fd = fopen(cp,"w")) == NULL){
        printf("File does not exist!\n");
        return;
    }
    lev();

    for (int i = 0; i < Nnodes; i++)
    {        
        Node[i].CC0 = -1;
        Node[i].CC1 = -1;
        Node[i].C0 = -1;        
    }
    for (int i = 0; i < Nnodes; i++)//initialization
    {
        if (Node[i].level == 0)
        {
            Node[i].CC0 =1;
            Node[i].CC1 =1;
        }
        if (Node[i].fout ==0)
        {
            Node[i].C0 = 0;
            if(max_level < Node[i].level)
                max_level = Node[i].level;
        }
    }

    for (int i = 1; i <= max_level; i++)// compute logic,start from lev1
    {
        for (int j = 0; j < Nnodes; j++)
        {
            if (Node[j].level == i)//find which node ->which level
            {
                if (Node[j].type == BRCH)//there is no way we will find PI because PI is in lev0, we start from lev1;
                {
                    Node[j].CC0 =  Node[j].unodes[0]->CC0;
                    Node[j].CC1 =  Node[j].unodes[0]->CC1;
                }

                else if (Node[j].type == XOR)
                {
                            int temp1,temp2,temp3,temp4;
                            temp1 = Node[j].unodes[0]->CC0 + Node[j].unodes[1]->CC0;
                            temp2 = Node[j].unodes[0]->CC1 + Node[j].unodes[1]->CC1;
                            if (temp1<temp2)
                            {
                                Node[j].CC0 =temp1 +1;
                            }
                            else
                            {
                                Node[j].CC0 = temp2 +1;
                            }
                            temp3 = Node[j].unodes[0]->CC1 + Node[j].unodes[1]->CC0;
                            temp4 = Node[j].unodes[1]->CC1 + Node[j].unodes[0]->CC0;
                            if (temp3 <temp4)
                            {
                               Node[j].CC1 =temp3 +1;
                            }
                            else
                            {
                                 Node[j].CC1 =temp4 +1;
                            }
                            
                            
                                                        
                            
                }

                else if (Node[j].type == OR)
                {
                            Node[j].CC0 = Node[j].unodes[0]->CC0;
                            Node[j].CC1 = Node[j].unodes[0]->CC1;
                             for (int k = 1; k < Node[j].fin; k++)
                            {
                                Node[j].CC0 = Node[j].CC0 + (Node[j].unodes[k]->CC0);
                                if (Node[j].unodes[k]->CC1 <Node[j].CC1)
                                {
                                    Node[j].CC1 = Node[j].unodes[k]->CC1;
                                }
                                
                            }
                            Node[j].CC0 =Node[j].CC0 +1;
                            Node[j].CC1 =Node[j].CC1 +1;

                }

                else if (Node[j].type == NOR)
                {
                            int temp1,temp2;
                            temp1 = Node[j].unodes[0]->CC0;
                            temp2 = Node[j].unodes[0]->CC1;
                             for (int k = 1; k < Node[j].fin; k++)
                            {
                                temp1 = temp1 + (Node[j].unodes[k]->CC0);
                                if (Node[j].unodes[k]->CC1 <temp2)
                                {
                                    temp2 = Node[j].unodes[k]->CC0;
                                }
                            }
                            Node[j].CC1 = temp1 +1;
                            Node[j].CC0 = temp2 +1;
                }

                
                else if (Node[j].type == NOT)
                {
                            Node[j].CC0 = Node[j].unodes[0]->CC1 +1;
                            Node[j].CC1 = Node[j].unodes[0]->CC0 +1;

                }

                else if (Node[j].type == NAND)
                {
                            int temp1,temp2;
                            temp1 = Node[j].unodes[0]->CC0;
                            temp2 = Node[j].unodes[0]->CC1;
                             for (int k = 1; k < Node[j].fin; k++)
                            {
                                temp2 = temp2 + (Node[j].unodes[k]->CC1);
                                if (Node[j].unodes[k]->CC0 <temp1)
                                {
                                    temp1 = Node[j].unodes[k]->CC0;
                                }
                            }
                            Node[j].CC1 = temp1 +1;
                            Node[j].CC0 = temp2 +1;
                }
                

                else if (Node[j].type == AND)
                {
                            int temp1,temp2;
                            temp1 = Node[j].unodes[0]->CC0;
                            temp2 = Node[j].unodes[0]->CC1;
                             for (int k = 1; k < Node[j].fin; k++)
                            {
                                temp2 = temp2 + (Node[j].unodes[k]->CC1);
                                if (Node[j].unodes[k]->CC0 <temp1)
                                {
                                    temp1 = Node[j].unodes[k]->CC0;
                                }
                            }
                            Node[j].CC1 = temp2 +1;
                            Node[j].CC0 = temp1 +1;
                }



                else if (Node[j].type == XNOR)
                {
                    int temp1,temp2,temp3,temp4;
                    temp1 = Node[j].unodes[0]->CC0 + Node[j].unodes[1]->CC0;
                    temp2 = Node[j].unodes[0]->CC1 + Node[j].unodes[1]->CC1;
                    temp3 = Node[j].unodes[0]->CC1 + Node[j].unodes[1]->CC0;
                    temp4 = Node[j].unodes[1]->CC1 + Node[j].unodes[0]->CC0;

                    if (temp3<temp4)
                    {
                        Node[j].CC0 = temp3 +1;
                    }
                    else
                    {
                        Node[j].CC0 = temp4 +1;
                    }
                    
                    if (temp1<temp2)
                    {
                       Node[j].CC1 = temp1 +1;
                    }
                    else
                    {
                        Node[j].CC1 = temp2 +1;
                    }                    
                } 

                else if (Node[j].type == BUF)
                {
                    Node[j].CC0 =  Node[j].unodes[0]->CC0 + 1;
                    Node[j].CC1 =  Node[j].unodes[0]->CC1 + 1;
                } 

            }           
        }
    }

    for (int i = max_level; i >=1; i--)// compute logic,start from max_level
    {
        //printf("i = %d\n",i);
        //printf("max_level = %d\n",max_level);
        for (int j = 0; j < Nnodes; j++)
        {
            //printf("j = %d\n",j);
            //printf("gatetype:%d\n",Node[j].type);
            //printf("level now is :%d\n",Node[j].level);

            if (Node[j].level == i)//find which node ->which level
            {
                if (Node[j].type == BRCH)//there is no way we will find PI because PI is in lev0, we start from lev1;
                {
                    //printf("im inside branch\n");
                    int temp,temp1;
                    temp =  Node[j].unodes[0]->indx;
                    temp1 = Node[temp].dnodes[0]-> C0;
                    for (int k = 1; k < Node[temp].fout; k++)
                    {
                        if (Node[temp].dnodes[k]->C0 <temp1)
                        {
                            temp1 =Node[temp].dnodes[k]->C0;
                        }
                        
                    }
                    
                    Node[temp].C0 = temp1;
                }

                else if (Node[j].type == XOR)
                {
                    int temp_x,temp_y,temp_x_CC0,temp_x_CC1,temp_y_CC0,temp_y_CC1;
                    temp_x =  Node[j].unodes[0]->indx;
                    temp_y = Node[j].unodes[1]->indx;
                    temp_x_CC0 = Node[temp_x].CC0;
                    temp_x_CC1 = Node[temp_x].CC0;
                    temp_y_CC0 = Node[temp_y].CC0;
                    temp_y_CC1 = Node[temp_y].CC1;
                    if (temp_x_CC0<temp_x_CC1)
                    {
                        Node[temp_y].C0 = temp_x_CC0 + Node[j].C0 +1;
                    }
                    else
                    {
                        Node[temp_y].C0 = temp_x_CC1 + Node[j].C0 +1;
                    }

                    if (temp_y_CC0<temp_y_CC1)
                    {
                        Node[temp_x].C0 = temp_y_CC0 + Node[j].C0 +1;
                    }
                    else
                    {
                        Node[temp_x].C0 = temp_y_CC1 + Node[j].C0 +1;
                    }                            
                }

                else if (Node[j].type == OR)
                {
                    int temp,temp_store_CC0,temp2;
                    for (int k = 0; k < Node[j].fin; k++)
                    {
                        temp = Node[j].unodes[k]->indx;
                        temp_store_CC0 = 0;
                        for (int z = 0; z < Node[j].fin; z++)
                        {
                            if ((Node[j].unodes[z]->indx) != temp)
                            {
                                temp2 = Node[j].unodes[z]->indx;
                                temp_store_CC0 = temp_store_CC0 + Node[temp2].CC0;
                            }

                        }
                        Node[temp].C0 = temp_store_CC0 +Node[j].C0 +1;
                    }
                }

                else if (Node[j].type == NOR)
                {
                    int temp,temp_store_CC0,temp2;
                    for (int k = 0; k < Node[j].fin; k++)
                    {
                        temp = Node[j].unodes[k]->indx;
                        temp_store_CC0 = 0;
                        for (int z = 0; z < Node[j].fin; z++)
                        {
                            if ((Node[j].unodes[z]->indx) != temp)
                            {
                                temp2 = Node[j].unodes[z]->indx;
                                temp_store_CC0 = temp_store_CC0 + Node[temp2].CC0;
                            }

                        }
                        Node[temp].C0 = temp_store_CC0 +Node[j].C0 +1;
                    }
                }

                
                else if (Node[j].type == NOT)
                {
                    int temp;
                    temp =  Node[j].unodes[0]->indx;
                    Node[temp].C0 = Node[j].C0 +1;

                }

                else if (Node[j].type == NAND)
                {
                    int temp,temp_store_CC1,temp2;
                    for (int k = 0; k < Node[j].fin; k++)
                    {
                        temp = Node[j].unodes[k]->indx;
                        temp_store_CC1 = 0;
                        for (int z = 0; z < Node[j].fin; z++)
                        {
                            if ((Node[j].unodes[z]->indx) != temp)
                            {
                                temp2 = Node[j].unodes[z]->indx;
                                temp_store_CC1 = temp_store_CC1 + Node[temp2].CC1;
                            }

                        }
                        Node[temp].C0 = temp_store_CC1 +Node[j].C0 +1;
                    }
                }
                

                else if (Node[j].type == AND)
                {
                    int temp,temp_store_CC1,temp2;
                    for (int k = 0; k < Node[j].fin; k++)
                    {
                        temp = Node[j].unodes[k]->indx;
                        temp_store_CC1 = 0;
                        for (int z = 0; z < Node[j].fin; z++)
                        {
                            if ((Node[j].unodes[z]->indx) != temp)
                            {
                                temp2 = Node[j].unodes[z]->indx;
                                temp_store_CC1 = temp_store_CC1 + Node[temp2].CC1;
                            }

                        }
                        Node[temp].C0 = temp_store_CC1 +Node[j].C0 +1;
                    }
                }



                else if (Node[j].type == XNOR)
                {
                    int temp_x,temp_y,temp_x_CC0,temp_x_CC1,temp_y_CC0,temp_y_CC1;
                    temp_x =  Node[j].unodes[0]->indx;
                    temp_y = Node[j].unodes[1]->indx;
                    temp_x_CC0 = Node[temp_x].CC0;
                    temp_x_CC1 = Node[temp_x].CC0;
                    temp_y_CC0 = Node[temp_y].CC0;
                    temp_y_CC1 = Node[temp_y].CC1;
                    if (temp_x_CC0<temp_x_CC1)
                    {
                        Node[temp_y].C0 = temp_x_CC0 + Node[j].C0 +1;
                    }
                    else
                    {
                        Node[temp_y].C0 = temp_x_CC1 + Node[j].C0 +1;
                    }

                    if (temp_y_CC0<temp_y_CC1)
                    {
                        Node[temp_x].C0 = temp_y_CC0 + Node[j].C0 +1;
                    }
                    else
                    {
                        Node[temp_x].C0 = temp_y_CC1 + Node[j].C0 +1;
                    }                     
                } 

                else if (Node[j].type == BUF)
                {
                    int temp;
                    temp =  Node[j].unodes[0]->indx;
                    Node[temp].C0 = Node[j].C0 +1;
                } 

            }
        }           
    }

   

for (int i = 0; i < Nnodes; i++)
{
    //fprintf(fd,"node:%d,CC0:%d,CC1:%d,C0:%d\n",Node[i].num,Node[i].CC0,Node[i].CC1,Node[i].C0);
    fprintf(fd,"%d,%d,%d,%d\n",Node[i].num,Node[i].CC0,Node[i].CC1,Node[i].C0);
}
fclose(fd);

}

void scoap_simplified(){
    FILE *fd;
    int max_level =0;
    lev();

    for (int i = 0; i < Nnodes; i++)
    {        
        Node[i].CC0 = -1;
        Node[i].CC1 = -1;
        Node[i].C0 = -1;        
    }
    for (int i = 0; i < Nnodes; i++)//initialization
    {
        if (Node[i].level == 0)
        {
            Node[i].CC0 =1;
            Node[i].CC1 =1;
        }
        if (Node[i].fout ==0)
        {
            Node[i].C0 = 0;
            if(max_level < Node[i].level)
                max_level = Node[i].level;
        }
    }

    for (int i = 1; i <= max_level; i++)// compute logic,start from lev1
    {
        for (int j = 0; j < Nnodes; j++)
        {
            if (Node[j].level == i)//find which node ->which level
            {
                if (Node[j].type == BRCH)//there is no way we will find PI because PI is in lev0, we start from lev1;
                {
                    Node[j].CC0 =  Node[j].unodes[0]->CC0;
                    Node[j].CC1 =  Node[j].unodes[0]->CC1;
                }

                else if (Node[j].type == XOR)
                {
                            int temp1,temp2,temp3,temp4;
                            temp1 = Node[j].unodes[0]->CC0 + Node[j].unodes[1]->CC0;
                            temp2 = Node[j].unodes[0]->CC1 + Node[j].unodes[1]->CC1;
                            if (temp1<temp2)
                            {
                                Node[j].CC0 =temp1 +1;
                            }
                            else
                            {
                                Node[j].CC0 = temp2 +1;
                            }
                            temp3 = Node[j].unodes[0]->CC1 + Node[j].unodes[1]->CC0;
                            temp4 = Node[j].unodes[1]->CC1 + Node[j].unodes[0]->CC0;
                            if (temp3 <temp4)
                            {
                               Node[j].CC1 =temp3 +1;
                            }
                            else
                            {
                                 Node[j].CC1 =temp4 +1;
                            }
                            
                            
                                                        
                            
                }

                else if (Node[j].type == OR)
                {
                            Node[j].CC0 = Node[j].unodes[0]->CC0;
                            Node[j].CC1 = Node[j].unodes[0]->CC1;
                             for (int k = 1; k < Node[j].fin; k++)
                            {
                                Node[j].CC0 = Node[j].CC0 + (Node[j].unodes[k]->CC0);
                                if (Node[j].unodes[k]->CC1 <Node[j].CC1)
                                {
                                    Node[j].CC1 = Node[j].unodes[k]->CC1;
                                }
                                
                            }
                            Node[j].CC0 =Node[j].CC0 +1;
                            Node[j].CC1 =Node[j].CC1 +1;

                }

                else if (Node[j].type == NOR)
                {
                            int temp1,temp2;
                            temp1 = Node[j].unodes[0]->CC0;
                            temp2 = Node[j].unodes[0]->CC1;
                             for (int k = 1; k < Node[j].fin; k++)
                            {
                                temp1 = temp1 + (Node[j].unodes[k]->CC0);
                                if (Node[j].unodes[k]->CC1 <temp2)
                                {
                                    temp2 = Node[j].unodes[k]->CC0;
                                }
                            }
                            Node[j].CC1 = temp1 +1;
                            Node[j].CC0 = temp2 +1;
                }

                
                else if (Node[j].type == NOT)
                {
                            Node[j].CC0 = Node[j].unodes[0]->CC1 +1;
                            Node[j].CC1 = Node[j].unodes[0]->CC0 +1;

                }

                else if (Node[j].type == NAND)
                {
                            int temp1,temp2;
                            temp1 = Node[j].unodes[0]->CC0;
                            temp2 = Node[j].unodes[0]->CC1;
                             for (int k = 1; k < Node[j].fin; k++)
                            {
                                temp2 = temp2 + (Node[j].unodes[k]->CC1);
                                if (Node[j].unodes[k]->CC0 <temp1)
                                {
                                    temp1 = Node[j].unodes[k]->CC0;
                                }
                            }
                            Node[j].CC1 = temp1 +1;
                            Node[j].CC0 = temp2 +1;
                }
                

                else if (Node[j].type == AND)
                {
                            int temp1,temp2;
                            temp1 = Node[j].unodes[0]->CC0;
                            temp2 = Node[j].unodes[0]->CC1;
                             for (int k = 1; k < Node[j].fin; k++)
                            {
                                temp2 = temp2 + (Node[j].unodes[k]->CC1);
                                if (Node[j].unodes[k]->CC0 <temp1)
                                {
                                    temp1 = Node[j].unodes[k]->CC0;
                                }
                            }
                            Node[j].CC1 = temp2 +1;
                            Node[j].CC0 = temp1 +1;
                }



                else if (Node[j].type == XNOR)
                {
                    int temp1,temp2,temp3,temp4;
                    temp1 = Node[j].unodes[0]->CC0 + Node[j].unodes[1]->CC0;
                    temp2 = Node[j].unodes[0]->CC1 + Node[j].unodes[1]->CC1;
                    temp3 = Node[j].unodes[0]->CC1 + Node[j].unodes[1]->CC0;
                    temp4 = Node[j].unodes[1]->CC1 + Node[j].unodes[0]->CC0;

                    if (temp3<temp4)
                    {
                        Node[j].CC0 = temp3 +1;
                    }
                    else
                    {
                        Node[j].CC0 = temp4 +1;
                    }
                    
                    if (temp1<temp2)
                    {
                       Node[j].CC1 = temp1 +1;
                    }
                    else
                    {
                        Node[j].CC1 = temp2 +1;
                    }                    
                } 

                else if (Node[j].type == BUF)
                {
                    Node[j].CC0 =  Node[j].unodes[0]->CC0 + 1;
                    Node[j].CC1 =  Node[j].unodes[0]->CC1 + 1;
                } 

            }           
        }
    }

    for (int i = max_level; i >=1; i--)// compute logic,start from max_level
    {
        //printf("i = %d\n",i);
        //printf("max_level = %d\n",max_level);
        for (int j = 0; j < Nnodes; j++)
        {
            //printf("j = %d\n",j);
            //printf("gatetype:%d\n",Node[j].type);
            //printf("level now is :%d\n",Node[j].level);

            if (Node[j].level == i)//find which node ->which level
            {
                if (Node[j].type == BRCH)//there is no way we will find PI because PI is in lev0, we start from lev1;
                {
                    //printf("im inside branch\n");
                    int temp,temp1;
                    temp =  Node[j].unodes[0]->indx;
                    temp1 = Node[temp].dnodes[0]-> C0;
                    for (int k = 1; k < Node[temp].fout; k++)
                    {
                        if (Node[temp].dnodes[k]->C0 <temp1)
                        {
                            temp1 =Node[temp].dnodes[k]->C0;
                        }
                        
                    }
                    
                    Node[temp].C0 = temp1;
                }

                else if (Node[j].type == XOR)
                {
                    int temp_x,temp_y,temp_x_CC0,temp_x_CC1,temp_y_CC0,temp_y_CC1;
                    temp_x =  Node[j].unodes[0]->indx;
                    temp_y = Node[j].unodes[1]->indx;
                    temp_x_CC0 = Node[temp_x].CC0;
                    temp_x_CC1 = Node[temp_x].CC0;
                    temp_y_CC0 = Node[temp_y].CC0;
                    temp_y_CC1 = Node[temp_y].CC1;
                    if (temp_x_CC0<temp_x_CC1)
                    {
                        Node[temp_y].C0 = temp_x_CC0 + Node[j].C0 +1;
                    }
                    else
                    {
                        Node[temp_y].C0 = temp_x_CC1 + Node[j].C0 +1;
                    }

                    if (temp_y_CC0<temp_y_CC1)
                    {
                        Node[temp_x].C0 = temp_y_CC0 + Node[j].C0 +1;
                    }
                    else
                    {
                        Node[temp_x].C0 = temp_y_CC1 + Node[j].C0 +1;
                    }                            
                }

                else if (Node[j].type == OR)
                {
                    int temp,temp_store_CC0,temp2;
                    for (int k = 0; k < Node[j].fin; k++)
                    {
                        temp = Node[j].unodes[k]->indx;
                        temp_store_CC0 = 0;
                        for (int z = 0; z < Node[j].fin; z++)
                        {
                            if ((Node[j].unodes[z]->indx) != temp)
                            {
                                temp2 = Node[j].unodes[z]->indx;
                                temp_store_CC0 = temp_store_CC0 + Node[temp2].CC0;
                            }

                        }
                        Node[temp].C0 = temp_store_CC0 +Node[j].C0 +1;
                    }
                }

                else if (Node[j].type == NOR)
                {
                    int temp,temp_store_CC0,temp2;
                    for (int k = 0; k < Node[j].fin; k++)
                    {
                        temp = Node[j].unodes[k]->indx;
                        temp_store_CC0 = 0;
                        for (int z = 0; z < Node[j].fin; z++)
                        {
                            if ((Node[j].unodes[z]->indx) != temp)
                            {
                                temp2 = Node[j].unodes[z]->indx;
                                temp_store_CC0 = temp_store_CC0 + Node[temp2].CC0;
                            }

                        }
                        Node[temp].C0 = temp_store_CC0 +Node[j].C0 +1;
                    }
                }

                
                else if (Node[j].type == NOT)
                {
                    int temp;
                    temp =  Node[j].unodes[0]->indx;
                    Node[temp].C0 = Node[j].C0 +1;

                }

                else if (Node[j].type == NAND)
                {
                    int temp,temp_store_CC1,temp2;
                    for (int k = 0; k < Node[j].fin; k++)
                    {
                        temp = Node[j].unodes[k]->indx;
                        temp_store_CC1 = 0;
                        for (int z = 0; z < Node[j].fin; z++)
                        {
                            if ((Node[j].unodes[z]->indx) != temp)
                            {
                                temp2 = Node[j].unodes[z]->indx;
                                temp_store_CC1 = temp_store_CC1 + Node[temp2].CC1;
                            }

                        }
                        Node[temp].C0 = temp_store_CC1 +Node[j].C0 +1;
                    }
                }
                

                else if (Node[j].type == AND)
                {
                    int temp,temp_store_CC1,temp2;
                    for (int k = 0; k < Node[j].fin; k++)
                    {
                        temp = Node[j].unodes[k]->indx;
                        temp_store_CC1 = 0;
                        for (int z = 0; z < Node[j].fin; z++)
                        {
                            if ((Node[j].unodes[z]->indx) != temp)
                            {
                                temp2 = Node[j].unodes[z]->indx;
                                temp_store_CC1 = temp_store_CC1 + Node[temp2].CC1;
                            }

                        }
                        Node[temp].C0 = temp_store_CC1 +Node[j].C0 +1;
                    }
                }



                else if (Node[j].type == XNOR)
                {
                    int temp_x,temp_y,temp_x_CC0,temp_x_CC1,temp_y_CC0,temp_y_CC1;
                    temp_x =  Node[j].unodes[0]->indx;
                    temp_y = Node[j].unodes[1]->indx;
                    temp_x_CC0 = Node[temp_x].CC0;
                    temp_x_CC1 = Node[temp_x].CC0;
                    temp_y_CC0 = Node[temp_y].CC0;
                    temp_y_CC1 = Node[temp_y].CC1;
                    if (temp_x_CC0<temp_x_CC1)
                    {
                        Node[temp_y].C0 = temp_x_CC0 + Node[j].C0 +1;
                    }
                    else
                    {
                        Node[temp_y].C0 = temp_x_CC1 + Node[j].C0 +1;
                    }

                    if (temp_y_CC0<temp_y_CC1)
                    {
                        Node[temp_x].C0 = temp_y_CC0 + Node[j].C0 +1;
                    }
                    else
                    {
                        Node[temp_x].C0 = temp_y_CC1 + Node[j].C0 +1;
                    }                     
                } 

                else if (Node[j].type == BUF)
                {
                    int temp;
                    temp =  Node[j].unodes[0]->indx;
                    Node[temp].C0 = Node[j].C0 +1;
                } 

            }
        }           
    }

}

/**/
void podem_for_atpg(int node_num,int fault_value,FILE *fd,FILE *fd1,int first_time){
    int node_indx;
    //FILE*fd1;
    bool success = false;    
    // FIXME NEED OPEN FILE AND WRITE
    // if((fd = fopen("podem_atpg.txt","w")) == NULL){
    //     printf("File does not exist!\n");
    //     return;
    // }
    // if((fd1 = fopen(input_file,"w")) == NULL){
    //     printf("File does not exist!\n");
    //     return;
    // }


    //set all node vals to x
    for(int i = 0; i < Nnodes; i++){
        Node[i].value = 2;
        Node[i].at_fault = 0;
        if(Node[i].num == node_num)
            node_indx = i;
    }
    //printf("im at podem");
    (&Node[node_indx])->at_fault = fault_value+1;
    success = podem_run(node_indx,fault_value); 
    
    //FIXME write PIs to output_file as a generated test pattern.
    if(success)
    {
        //printf("I come inside\n\n");
        if(first_time){
            for(int i = 0; i < Npi; i++){
            fprintf(fd, "%d",Pinput[i]->num);
            fprintf(fd1, "%d",Pinput[i]->num);
            if((i+1) != Npi){
               fprintf(fd,",");
               fprintf(fd1,","); 
            }
                
        }
        fprintf(fd,"\n");
        fprintf(fd1,"\n");
        }


        for(int i = 0; i < Npi; i++){
            if(Pinput[i]->value == 2){
                fprintf(fd, "0");
                fprintf(fd1, "0");
            }
            else if(Pinput[i]->value == 3){
                fprintf(fd, "1");
                fprintf(fd1, "1");
            }
            else if(Pinput[i]->value == 4){
                fprintf(fd, "0");
                fprintf(fd1, "0");
            }
            else {
                fprintf(fd, "%d",Pinput[i]->value);
                fprintf(fd1, "%d",Pinput[i]->value);
                
                
            }
            if((i+1) != Npi){
                fprintf(fd,",");
                fprintf(fd1,",");
            }
        }   
        fprintf(fd,"\n");
        fprintf(fd1,"\n");
    }

    

}

// where do we update dfrontier: start of podem_run
// where do we check if fault in activated: at podem_run do a for loop
// where do we update 0 1 to D D': imply -> node_eval
// where do we check if fault in at po: at podem_run do a for loop

void imply(int indx, int val){
    NSTRUC* np = &Node[indx];
    if( np->type != IPT){
        printf("ERROR, IMPLY RECEIVE A NON PI INPUT\n");
    }
    np->value = val;
    node_eval(np);
    list<NSTRUC*> node_list;
    for(int i = 0; i < np->fout; i++){
        //printf("IN FOR LOOP\n");
        node_list.push_back(np->dnodes[i]);
    }
    while(node_list.empty() != 1){
        //printf("IN WHILE LOOP\n");
        np = node_list.front();
        node_eval(np);
        if(np->value != 2){
            // propagate
            for(int i = 0; i < np->fout; i++){
                node_list.push_back(np->dnodes[i]);
            }
        }
        /*
        else{
            for(int i = 0; i < np->fin; i++){
                if(np->unodes[i]->value == 4 ||np->unodes[i]->value == 3){
                    //found d frontier
                    if(d_frontier_set.insert(np->dnodes[i]->indx).second == false){
                        
                    }
                }
            }
        }
        */
        node_list.pop_front();
    }
    // printf("\n\n\n");
    // for (int i = 0; i < Nnodes; i++)
    // {
    //     printf("node:%d, value:%d\n",Node[i].num,Node[i].value);
    // }
    // printf("\n\n\n");
}

// value 3 == D 1/0, value 4 == D' 0/1
bool podem_run(int indx, int ssa){
    // first scan all nodes for d frontier and if fault at PO
    //printf("im inside podem_run\n");
    list<int> d_frontier;
    bool fault_activated = 0;
    //printf("podem run start\n");
    //check if fault is activated
    /*if((&Node[indx])->value == ssa){
        return false;
    }*/
    for (int i = 0; i < Nnodes; i++)
    {
        NSTRUC* np = &Node[i];
        if(np->value == 2){
            if(np->type == BRCH){
                if(np->unodes[0]->value != 2 ){
                    np->value = np->unodes[0]->value;
                    for(int j = 0; j < np->fout; j++){
                        node_eval(np->dnodes[j]);
                    }
                }       
            }
        }
    }
    if(ssa == 0 && (&Node[indx])->value == 3){
        fault_activated = 1;
    }
    else if(ssa == 1 && (&Node[indx])->value == 4){
        fault_activated = 1;
    }
    else if( (&Node[indx])->value != 2 || (&Node[indx])->value == ssa){
        //fault node got assigned to non fault and non x value 
        //which means the propagated value is wrong
        // or node_eval did not update 0 1 to D D'
        // failure due to fault not activatable
        //(&Node[indx])->value = 2;
        return false;
    }
    //printf("podem run start 2\n");
    //check if fault is at po
    for ( int i = 0; i < Npo; i++)
    {
        if(Poutput[i]->value  == 3 || Poutput[i]->value  == 4){
                // D or D' at PO
                return true;
            }
    }
    //update dfrontier
    //printf("podem run start 3 \n");
    if(fault_activated == 1){
        for (int i = Nnodes-1; i >= 0; i--)
        {
            NSTRUC* np = &Node[i];
            int D_flag = 0;
            if(np->value == 2){
                for(int j = 0; j < np->fin; j++){
                    
                    if(np->unodes[j]->value == 3 || np->unodes[j]->value == 4 ){
                        //found fault in input
                        D_flag = 1;
                        if(np->type == BRCH || np->type == BUF || np->type == NOT){
                            printf("BUG IN SEARCHING FOR D FRONTIER: A GATE WITHOUT INPUT CHOICE MIGHT BE ADDED\n");
                        }
                        break;
                    }
                }
                if(D_flag == 1){
                    // add to d frontier
                    d_frontier.push_back(i);
                }
            }
        }
    }
    // check if d frontier is empty 
    // and return fail
    //printf("podem run start 4\n");
    if(d_frontier.size() == 0 &&fault_activated ==1 ){
        return false;
    }
    //printf("podem run start 5\n");
    //printf("obj: indx %d, node num %d, ssa %d, val: %d\n", indx, Node[indx].num, ssa,Node[indx].value);
    //printf("input num %d, input val %d \n", Node[indx].unodes[0]->num,  Node[indx].unodes[0]->value);
    pair<int,int> obj_pair = obj(indx,ssa,fault_activated,d_frontier);
    //printf("obj return: indx %d, node num %d, val %d\n", obj_pair.first, Node[indx].num, obj_pair.second);
    //printf("activated: %d\n", fault_activated);
    //printf("podem run start 6\n");
    
    pair<int,int> PI = backtrace(obj_pair.first,obj_pair.second);
    //printf("line num:%d,value:%d\n",Node[PI.first].num,PI.second);
    //printf("imply start\n");
    imply(PI.first,PI.second);
    //printf("imply finish\n");
    //printf("line num:%d,value after imply:%d\n\n",Node[PI.first].num,Node[PI.first].value);
    if(podem_run(indx,ssa) == true){
        return true;
    }
    imply(PI.first,(PI.second+1)%2);
    if(podem_run(indx,ssa) == true){
        return true;
    }
    imply(PI.first,2);
    return false;
}

pair<int,int> obj(int indx, int val, bool fault_activated, 
    list<int> d_frontier){
    NSTRUC* np = &Node[indx];
    pair<int,int> obj_return;
    int min_c = 1000000000;

    int xor_flag;
    int j,c;
    if(fault_activated == false){
        obj_return.first = indx;
        if( val == 0)
            obj_return.second = 1;
        else
            obj_return.second = 0;        
    }
    else{
        //fault activated, propagate the fault along d frontier to POs
        for(int i = 0; i < d_frontier.size(); i++){
            np = &Node[d_frontier.front()];
            // gate
            if(np->type == OR || np->type == NOR){
                c = 1;
                //np = np->unodes[0];
            }
            else if(np->type == XOR || np->type == XNOR){
                xor_flag = 1;
            }
            else if(np->type == AND|| np->type == NAND){
                c = 0;
            }
            for(int j = 0; j < np->fin; j++){
                // input line
                if(np->unodes[j]->value == 2){
                    // possible choice, take notes
                    int return_val = (c+1)%2;
                    if(return_val == 1){
                        if(np->unodes[j]->CC1 < min_c){
                            min_c = np->unodes[j]->CC1;
                            obj_return.first = np->unodes[j]->indx;
                            obj_return.second = return_val;
                        }
                    }
                    else{
                        if(np->unodes[j]->CC0 < min_c){
                            min_c = np->unodes[j]->CC0;
                            obj_return.first = np->unodes[j]->indx;
                            obj_return.second = return_val;
                        }
                    }
                }
            }
            d_frontier.pop_front();
        }
    }
    return obj_return;
}

pair<int,int> backtrace(int indx, int val){
    //printf("enter backtrace\n");
    NSTRUC* np = &Node[indx];
    int i;
    int v = val;
    int choice0,choice1, min_c1, min_c0;
    int xor_flag;
    //printf("indx: %d, num: %d, node_indx: %d, type: %d \n",indx, np->num, np->indx, np->type);
    //printf("\nnum: %d, val: %d\n", Node[13].num, Node[13].value);
    while(np->type != IPT){
        xor_flag = 0;
        
        // get i,v and next np
        if(np->type == BRCH || np->type == OR || np->type == AND || np->type == BUF){
            i = 0;
            //np = np->unodes[0];
        }
        else if(np->type == XOR || np->type == XNOR){
            xor_flag = 1;
        }
        else if(np->type == NOR || np->type == NOT || np->type == NAND){
            i = 1;
        }

        v = v^i;
        choice0 = -1;
        choice1 = -1;
        min_c1 = 100000000;
        min_c0 = 100000000;
        //int x_flag;
        for(int j = 0; j < np->fin; j++){
            if(np->unodes[j]->value == 2){
                //x_flag = 1;
                if(np->unodes[j]->CC1 < min_c1){
                    min_c1 = np->unodes[j]->CC1;
                    choice1 = np->unodes[j]->indx;
                }
                if(np->unodes[j]->CC0 < min_c0){
                    min_c0 = np->unodes[j]->CC0;
                    choice0 = np->unodes[j]->indx;
                }
            }
        }

        if(xor_flag == 1){
            if(min_c0 < min_c1){
                np = &Node[choice0];
            }
            else{
                np = &Node[choice1];
            }
        }
        else{
            if(v == 1){
                np = &Node[choice1];
            }
            else{
                np = &Node[choice0];
            }
        }
    }
    pair<int,int> PI;
    PI.first = np->indx;
    PI.second = v;
    //printf("backtrace OVER\n\n");
    return PI;
}


void atpg_for_tpg(){
    time_t begin, end;
    time(&begin);
    char* output_file;
    char* report_file;
    char* useless;
    double fault_get_number=0;
    double fault_total_number=0;
    double fault_coverage=0;
    double seconds=0;
    FILE *fd ,*fd_output_file,*fd_podem;
    int first_time =1;
    cp[strlen(cp)-1] = '\0';
    // useless = strtok(cp," ");
    // output_file = strtok(NULL," ");
    report_file = strtok(NULL," ");
    int first_number=0;
    int second_number=0;
    pair<vector<int>,vector<int>> ssa_zero_one;
    scoap_simplified();
    ifstream file("fault_list_for_atpg.txt");
    string line;
    if (file.is_open()){
        while(getline(file,line)){
            stringstream ss(line);
            string token;
            getline(ss,token,'@');
            first_number = stoi(token);
            getline(ss,token);
            second_number = stoi(token);
            if (second_number == 0){
                ssa_zero_one.first.push_back(first_number);
            }
            else if(second_number == 1){
                ssa_zero_one.second.push_back(first_number);
            }            
        }
        file.close();
    }
    else{
        std::cout << "Unable to open file!" << std::endl;
    }
    

    if((fd_podem = fopen("podem_atpg_tpg.txt","w")) == NULL){
        printf("File does not exist!\n");
        return;
    }

    if((fd_output_file = fopen("useless.txt","w")) == NULL){
        printf("File does not exist!\n");
        return;
    }

    for (int i = 0; i < ssa_zero_one.first.size(); i++)
    {
        podem_for_atpg(ssa_zero_one.first[i],0,fd_podem,fd_output_file,first_time);
        first_time =0;

        printf("nodenum is:%d\n",ssa_zero_one.first[i]);
    }
    for (int i = 0; i < ssa_zero_one.second.size(); i++)
    {
        podem_for_atpg(ssa_zero_one.second[i],1,fd_podem,fd_output_file,first_time);
        first_time =0;
       // printf("im insde ss1\n");
       printf("nodenum is:%d\n",ssa_zero_one.second[i]);
    }
    fclose(fd_podem);
    fclose(fd_output_file);
    //printf("im here");

    
    pfs_atpg_tpg();
fault_total_number= 2*ssa_zero_one.first.size();
fault_coverage = (PFS_Fault_Count/fault_total_number)*100;
printf("fault coverage is:%lf\n",fault_coverage);
if((fd = fopen(report_file, "w")) == NULL) {
printf("File %s does not exist!\n",report_file);
 return ;
}   
time(&end);
seconds = difftime(end,begin);
printf("fault_total_number is:%lf\n\n",fault_total_number);
printf("fault_get_number is:%lf\n\n",PFS_Fault_Count);
fprintf(fd, "Circuit:%s\n",inp_name.c_str());
fprintf(fd, "Algorithm:PODEM\n");
fprintf(fd, "Fault Coverage:%lf%\n",fault_coverage);
fprintf(fd, "Time:%lf seconds\n",seconds);
fclose(fd);    
}

void tpg(){
int counter = 0;
lev();
int PFS_FC_temp;
int PFS_FC = 0;
int Total_Fault = 0;
double random_tp_used = 0;
std::unordered_set<std::string> uniqueFaults;
std::string input_file_fl = "complete_fault_list.txt";
std::ifstream infile_fl(input_file_fl);
std::unordered_set<std::string> fault_list;
std::string line;
while (std::getline(infile_fl, line)) {
    fault_list.insert(line);
    Total_Fault++;
    }
//Extract Complete Fault List

while(true){
    if (counter < 25){
        rtpg_tpg();//Create Random Pattern
        pfs_tpg(uniqueFaults);//Need to call complete Fault List
        random_tp_used++;
        //Now we get full list of uniquefaults that we can continuously add over time
        PFS_FC_temp = PFS_Fault_Count / Total_Fault;
        if (PFS_FC_temp > PFS_FC){
            PFS_FC = PFS_FC_temp;
            counter = 0;
        }
        else{
            counter++;
        }
    }
    else{
        break;
    }
//Erase the faults found in PFS from Complete Fault_List
for (const auto& fault : uniqueFaults) {
fault_list.erase(fault);
}
}
std::string output_file_fl = "fault_list_for_atpg.txt";
std::ofstream outfile_fl(output_file_fl);
for (const auto& element : fault_list) {
        outfile_fl << element << std::endl;
    }

atpg_for_tpg();

}