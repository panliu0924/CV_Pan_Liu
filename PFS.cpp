#include "readckt.h"

typedef struct fault_struc {
   unsigned indx;             /* node index */
   int sa;                    /* stuck at fault type */
   bool detect;               /* detactable */
} FSTRUC;

vector<FSTRUC> fl;            /* fault list */

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

void pfs(){
    NSTRUC *np;
    std::string input_file_tp;
    std::string input_file_fl;
    std::string output_file;
    int width;
    width = sizeof(size_t)*8;
    int node_num;
    //Extract File Path
    int first_space, second_space;
    cp[strlen(cp) - 1] = '\0';
    std::string cp_string = std::string(cp);
    first_space = cp_string.find(" ");
    input_file_tp = cp_string.substr(0, first_space);
    second_space = cp_string.find(" ", first_space + 1);
    input_file_fl = cp_string.substr(first_space + 1, second_space - first_space - 1);
    output_file = cp_string.substr(second_space + 1);

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


void pfs_atpg1(){
    NSTRUC *np;
    std::string input_file_tp = "podem_atpg.txt";
    std::string input_file_fl = "complete_fault_list.txt";
    std::string output_file = "total_detect_fl.txt";
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





void pfs_atpg(){
    NSTRUC *np;
    std::string input_file_tp = "podem_atpg.txt";
    std::string input_file_fl = "complete_fault_list.txt";
    std::string output_file = "Rfl_detected_fault_list.txt";
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






 