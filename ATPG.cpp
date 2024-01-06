#include "readckt.h"



void atpg(){
    time_t begin, end;
    time(&begin);
    char* output_file;
    char* report_file;
    char* useless;
    //double fault_get_number=0;
    double fault_total_number=0;
    double fault_coverage=0;
    double seconds=0;
    FILE *fd ,*fd_output_file,*fd_podem;
    int first_time =1;
    cp[strlen(cp)-1] = '\0';
    useless = strtok(cp," ");
    output_file = strtok(NULL," ");
    report_file = strtok(NULL," ");
    pair<vector<int>,vector<int>> ssa_zero_one = rfl_for_atpg();
    pair<vector<int>,vector<int>> complete_fl = complete_fault_list();
    fault_total_number= 2*complete_fl.first.size();
    int first_number =0;
    int second_number =0;
    scoap_simplified();
    if((fd_podem = fopen("podem_atpg.txt","w")) == NULL){
        printf("File does not exist!\n");
        return;
    }

    if((fd_output_file = fopen(output_file,"w")) == NULL){
        printf("File does not exist!\n");
        return;
    }

    for (int i = 0; i < ssa_zero_one.first.size(); i++)
    {
        podem_for_atpg(ssa_zero_one.first[i],0,fd_podem,fd_output_file,first_time);
        first_time =0;

        printf("nodenum_first is:%d\n",ssa_zero_one.first[i]);
    }
    for (int i = 0; i < ssa_zero_one.second.size(); i++)
    {
        podem_for_atpg(ssa_zero_one.second[i],1,fd_podem,fd_output_file,first_time);
        first_time =0;
       // printf("im insde ss1\n");
       printf("nodenum_first is:%d\n",ssa_zero_one.second[i]);
    }
    printf("imhere");
    fclose(fd_podem);
    fclose(fd_output_file);



    pfs_atpg();

    std::unordered_set<std::string> complete_fault;
    ifstream file("complete_fault_list.txt");
    string line;
    if (file.is_open()){
        while(getline(file,line)){
            complete_fault.insert(line);
        }
        file.close();
    }
    else{
        std::cout << "Unable to open file!" << std::endl;
    }
    ifstream file1("Rfl_detected_fault_list.txt");
    std::unordered_set<std::string> Rfl_dectected_fault;
    if (file1.is_open()){
        while(getline(file1,line)){
            Rfl_dectected_fault.insert(line);
        }
        file1.close();
    }
    else{
        std::cout << "Unable to open file!" << std::endl;
    }

    for (const auto& fault : Rfl_dectected_fault) {
        complete_fault.erase(fault);//now complete_fault = complete fault - RFL_detect fault
    }

    FILE *fd_remain;
    fd_remain = fopen("fl_remain.txt","w");
    for (const auto& fault : complete_fault) {
        fprintf(fd_remain, "%s\n", fault.c_str());
    }
    fclose(fd_remain);

    pair<vector<int>,vector<int>> remain_fault_list;
    ifstream file2("fl_remain.txt");
    string line1;
    if (file2.is_open()){
        while(getline(file2,line1)){
            stringstream ss(line1);
            string token;
            getline(ss,token,'@');
            first_number = stoi(token);
            getline(ss,token);
            second_number = stoi(token);
            if (second_number == 0){
                remain_fault_list.first.push_back(first_number);
            }
            else if(second_number == 1){
                remain_fault_list.second.push_back(first_number);
            }            
        }
        file2.close();
    }
    else{
        std::cout << "Unable to open file!" << std::endl;
    }

    if((fd_podem = fopen("podem_atpg.txt","a+")) == NULL){//generate tp_total
        printf("File does not exist!\n");
        return;
    }

    if((fd_output_file = fopen(output_file,"a+")) == NULL){
        printf("File does not exist!\n");
        return;
    }
    fseek(fd_podem, 0, SEEK_END);
    fseek(fd_output_file, 0, SEEK_END);

    for (int i = 0; i < remain_fault_list.first.size(); i++)
    {
        podem_for_atpg(remain_fault_list.first[i],0,fd_podem,fd_output_file,first_time);
        first_time =0;

        printf("nodenum_sec_1 is:%d\n",remain_fault_list.first[i]);
    }
    //printf("remain_fl_sec_size:%d\n\n",remain_fault_list.second.size());
    for (int i = 0; i < remain_fault_list.second.size(); i++)
    {
        podem_for_atpg(remain_fault_list.second[i],1,fd_podem,fd_output_file,first_time);
        first_time =0;
       // printf("im insde ss1\n");
       printf("nodenum_sec_0 is:%d\n",remain_fault_list.second[i]);
       //printf("i is :%d\n",i);
    }
    fclose(fd_podem);
    fclose(fd_output_file);    
    pfs_atpg1();

    
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



void atpg_for_tpg(){
    //time_t begin, end;
    //time(&begin);
    //char* output_file;
    //char* report_file;
    //char* useless;
    //double fault_get_number=0;
    //double fault_total_number=0;
    //double fault_coverage=0;
    //double seconds=0;
    FILE *fd ,*fd_output_file,*fd_podem;
    int first_time =1;
    // cp[strlen(cp)-1] = '\0';
    // useless = strtok(cp," ");
    // output_file = strtok(NULL," ");
    // report_file = strtok(NULL," ");
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

        //printf("nodenum is:%d\n",ssa_zero_one.first[i]);
    }
    for (int i = 0; i < ssa_zero_one.second.size(); i++)
    {
        podem_for_atpg(ssa_zero_one.second[i],1,fd_podem,fd_output_file,first_time);
        first_time =0;
       // printf("im insde ss1\n");
       //printf("nodenum is:%d\n",ssa_zero_one.second[i]);
    }
    fclose(fd_podem);
    fclose(fd_output_file);
    //printf("im here");

    
    pfs_atpg_tpg();
//     fault_total_number= 2*ssa_zero_one.first.size();
//     fault_coverage = (PFS_Fault_Count/fault_total_number)*100;
//     printf("fault coverage is:%lf\n",fault_coverage);
//     if((fd = fopen(report_file, "w")) == NULL) {
//       printf("File %s does not exist!\n",report_file);
//       return ;
//    }   
//     //time(&end);
//     //seconds = difftime(end,begin);
//     printf("fault_total_number is:%lf\n\n",fault_total_number);
//     printf("fault_get_number is:%lf\n\n",PFS_Fault_Count);
//     fprintf(fd, "Circuit:%s\n",inp_name.c_str());
//     fprintf(fd, "Algorithm:PODEM\n");
//     fprintf(fd, "Fault Coverage:%lf%\n",fault_coverage);
//     fprintf(fd, "Time:%lf seconds\n",seconds);
//     fclose(fd);    
}
